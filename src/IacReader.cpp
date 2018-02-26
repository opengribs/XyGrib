/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008-2012 - Jacques Zaninetti - http://www.zygrib.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include <cassert>

#include "IacReader.h"

//---------------------------------------------------
IacReader::IacReader(const std::string fname)
{
	ok = false;
	endOfFile = true; 
	currentSECTION = -1;
	xmin = ymin =  1e20;
	xmax = ymax = -1e20;
	currentTroughLine = NULL;
	file = NULL;
	openFile(fname);
}
//---------------------------------------------------
IacReader::~IacReader() 
{
	cleanAllVectors();
}


//-----------------------------------------------------------
void IacReader::decodeDataLine_preamble (std::vector<std::string> &vline)
{
	if (vline.size() >= 3)
	{
		if (vline.size()==3
				&&  vline[0]=="10001"
				&&  vline[1].substr(0,3) == "333")
		{
			iacFileType = IAC_ANALYSE;
			
		}
		else if (vline.size()==4
				&&  vline[0]=="65556"
				&&  vline[1].substr(0,3) == "333")
		{
			iacFileType = IAC_FORECAST;
			std::string sdel = vline[3].substr(3,2);
			int del;
			if (sscanf(sdel.c_str(), "%d", &del) == 1)
				iacFileValidHour = del;
			else
				ok = false;
		}
		else
			ok = false;
		
		if (ok)
		{
			std::string x1x1 = vline[1].substr(3,2);
				 if (x1x1 == "00")  yyyyyPositionMode = POS_LaLaLoLok_NORTH;
			else if (x1x1 == "11")  yyyyyPositionMode = POS_LaLaLoLok_SOUTH;
			else if (x1x1 == "22")  yyyyyPositionMode = POS_LaLaLoLok_EQUAT;
			else if (x1x1 == "66")  yyyyyPositionMode = POS_iiiD1s1;
			else if (x1x1 == "88")  yyyyyPositionMode = POS_QLaLaLoLo;
			else ok = false;
		}
	}
	else
		ok = false;
}

//-----------------------------------------------------------
void IacReader::decodeDataLine_header_NOAA (std::vector<std::string> &vline)
{
	// (vline[0]=="ASXX21" || vline[0]=="FSXX21")
	// Sample: FSXX21 EGRR 031800	03=day   18=hour  00=minutes
	is_NOAA_File = false;

	if (vline[1]=="EGRR"
			&& (vline[0]=="ASXX21" || vline[0]=="FSXX21") )
  	{
		std::string sday  = vline[2].substr(0,2);
		std::string shour = vline[2].substr(2,2);
		std::string smin  = vline[2].substr(4,2);
		int val;
		if (sscanf(sday.c_str(), "%d", &val) == 1) {
			if (val>=0 && val<=31)
				iacFileDay = val;
			else
				ok = false;
		}
		if (sscanf(shour.c_str(), "%d", &val) == 1) {
			if (val>=0 && val<24)
				iacFileHour = val;
			else
				ok = false;
		}
		if (sscanf(smin.c_str(), "%d", &val) == 1) {
			if (val>=0 && val<60)
				iacFileMinute = val;
			else
				ok = false;
		}
		if (ok)
			is_NOAA_File = true;
	}
}


//-----------------------------------------------------------
void IacReader::decodeLine (std::vector<std::string> &vline)
{
	if (vline.size() == 0)
		return;
	
	if (vline.size() >= 3
			&& (vline[0]=="ASXX21" || vline[0]=="FSXX21") )
  	{
		decodeDataLine_header_NOAA (vline);
	}
	else if (vline.size() == 3 && vline[0]=="ASPS20")
  	{	// Fiji Fleet codes from NOAA
		decodeDataLine_header_NOAA (vline);
	}
	else if (vline[0].size() != 5) {
		return;		// not a fleet code
	}
		
	int code;
	if (sscanf(vline[0].c_str(), "%d", &code) == 1)
	{
		if (vline.size() == 1)	// one code -> section start
		{
			switch (code)
			{
				case 99900:
					//printf("Start SECTION 0 : High/Low pressure\n");	// High/Low pressure
					currentSECTION = 0;
					break;
				case 99911:
					//printf("Start SECTION 1 : Fronts\n");	// Fronts
					currentSECTION = 1;
					break;
				case 99922:
					//printf("Start SECTION 2 : Isobars\n");	// Isobars
					currentSECTION = 2;
					break;
				case 19191:
					//printf("END OF FILE\n\n");
					currentSECTION = -1;
					break;
				default:
					currentSECTION = -1;
					break;
			}
		}
		else
		{
			if (code == 10001) {
				decodeDataLine_preamble (vline);
			}
			if (code == 65556) {
				decodeDataLine_preamble (vline);
			}
			else {
				decodeDataLine(vline);
			}
		}
	}
	else if (vline[0] == "empty")
	{
		decodeDataLine(vline);
	}
}


//-----------------------------------------------------------
// Section 0 : pressure Low/High + Trough pressure lines
//-----------------------------------------------------------
void IacReader::decodeDataLine_sec_0(std::vector<std::string> &vline)
{
	double lon, lat;

	if (vline[0].substr(0,1) == "9") {  // ignore code '9NNSS'
		vline.erase(vline.begin());
	}
	if (vline.size() == 0) {
		return;
	}
	if (vline.size()>=2
			&& (vline[0].substr(0,2)=="81" || vline[0].substr(0,2)=="85")
		)
	{
		std::string Pt = vline[0].substr(1,1);	// table 3152
		std::string Pc = vline[0].substr(2,1);	// table 3133
		std::string PP = vline[0].substr(3,2);	// PP = pressure on 2 digits
		int pr;
		//printLine(vline);		
		if (sscanf(PP.c_str(), "%d", &pr) == 1) {
			if (Pt=="1") {	// LOW pressure
				if (readPosition(vline[1], &lat, &lon)) {
					list_HighLowPressurePoints.push_back(
							new IAC_HighLowPressurePoint("L", lat, lon, pr));
				}
			}
			else if (Pt=="5") {	// HIGH pressure
				if (readPosition(vline[1], &lat, &lon)) {
					list_HighLowPressurePoints.push_back(
							new IAC_HighLowPressurePoint("H", lat, lon, pr));
				}
			}
		}
	}
	else if (vline[0].substr(0,2) == "83")	// code='83///' : pressure troughline
	{
		//printf("START trough line: \n");
 		IAC_TroughLine *tline = new IAC_TroughLine();
 		assert(tline);
 		currentTroughLine = tline;
		
		for (uint i=1; i<vline.size(); i++) {
			if (readPosition(vline[i], &lat, &lon))
			{
				//printf("%6d : %f %f\n", i,lat,lon);
				IAC_Point *point = new IAC_Point(lat,lon);
				assert(point);		
				tline->addPoint(point);
			}
		}		
		if (tline->points.size() > 0) {
			list_TroughLines.push_back(tline);
		}
		else {
			delete tline;
 			currentTroughLine = NULL;
		}
		//printf("\n");
	}
	else if (vline[0] == "empty")	//  TroughLine continued
	{
		if (currentTroughLine != NULL)
		{
			for (uint i=1; i<vline.size(); i++) {
				if (readPosition(vline[i], &lat, &lon))
				{
					IAC_Point *point = new IAC_Point(lat,lon);
					assert(point);		
					currentTroughLine->addPoint(point);
				}
			}
		}
	}
}

//-----------------------------------------------------------
// Section 1 : Fronts
//-----------------------------------------------------------
void IacReader::decodeDataLine_sec_1_Fronts (std::vector<std::string> &vline)
{
	double lon, lat;
	
	if (vline[0].substr(0,1) == "9") {  // ignore code '9NNSS'
		vline.erase(vline.begin());
	}
	if (vline.size() == 0) {
		return;
	}
	
	if (vline.size()>=2
			&& (vline[0].substr(0,2)=="66")
		)
	{
		currentFront = new IAC_Front(vline[0]);
		assert(currentFront);
		list_Fronts.push_back(currentFront);

		for (uint i=1; i<vline.size(); i++) {
			if (readPosition(vline[i], &lat, &lon))
			{
				IAC_Point *point = new IAC_Point(lat,lon);
				assert(point);
				currentFront->addPoint(point);
			}
		}
	}
	else if (vline[0] == "empty")	//  Isobar continued
	{
		if (currentFront != NULL)
		{
			for (uint i=1; i<vline.size(); i++) {
				if (readPosition(vline[i], &lat, &lon))
				{
					IAC_Point *point = new IAC_Point(lat,lon);
					assert(point);		
					currentFront->addPoint(point);
				}
			}
		}
	}
	else {
		currentFront = NULL;
	}
}

//-----------------------------------------------------------
// Section 2 : Isobars
//-----------------------------------------------------------
void IacReader::decodeDataLine_sec_2_Isobars(std::vector<std::string> &vline)
{
	double lon, lat;
	
	if (vline[0].substr(0,1) == "9") {  // ignore code '9NNSS'
		vline.erase(vline.begin());
	}
	if (vline.size() == 0) {
		return;
	}

	if (vline.size()>=2
			&& (vline[0].substr(0,2)=="44")
		)
	{
		std::string Pval = vline[0].substr(2,3);
		int pr;
		if (sscanf(Pval.c_str(), "%d", &pr) == 1) {
			if (pr < 500)
				pr += 1000;		// 44020->1020hpa

			currentIsobar = new IAC_Isobar(pr);
			assert(currentIsobar);
			list_Isobars.push_back(currentIsobar);
			
			for (uint i=1; i<vline.size(); i++) {
				if (readPosition(vline[i], &lat, &lon))
				{
					IAC_Point *point = new IAC_Point(lat,lon);
					assert(point);		
					currentIsobar->addPoint(point);
				}
			}
		}
	}
	else if (vline[0] == "empty")	//  Isobar continued
	{
		if (currentIsobar != NULL)
		{
			for (uint i=1; i<vline.size(); i++) {
				if (readPosition(vline[i], &lat, &lon))
				{
					IAC_Point *point = new IAC_Point(lat,lon);
					assert(point);		
					currentIsobar->addPoint(point);
				}
			}
		}
	}
	else {
		currentIsobar = NULL;
	}
}
//-----------------------------------------------------------
void IacReader::decodeDataLine(std::vector<std::string> &vline)
{
	switch (currentSECTION) {
		case 0:
			decodeDataLine_sec_0(vline);
			break;
		case 1:
			decodeDataLine_sec_1_Fronts(vline);
			break;
		case 2:
			decodeDataLine_sec_2_Isobars(vline);
			break;
	}
}
//-----------------------------------------------------------
void IacReader::display_text()
{
	//========================================
	// Test: display data
	//========================================
/***************	
	printf("------------------------------------\n");
	printf("IAC_MinMaxPressurePoint's\n");
	for (uint i=0; i<list_HighLowPressurePoints.size(); i++)
	{
		IAC_HighLowPressurePoint *p = list_HighLowPressurePoints[i];
		printf("PRESS:  %s  PP=%02d  (%5.1f,%5.1f)\n", p->type.c_str(),p->value,p->lat,p->lon);
	}
	printf("------------------------------------\n");
	printf("IAC_TroughLine's\n");
	for (uint i=0; i<list_TroughLines.size(); i++)
	{
		IAC_TroughLine *lsp = list_TroughLines[i];
		printf("Trough line: %d ", lsp->points.size());
		for (uint j=0; j<lsp->points.size(); j++)
		{
			printf("(%5.1f,%5.1f) ", lsp->points[j]->y, lsp->points[j]->x);
		}
		printf("\n");
	}
	printf("------------------------------------\n");
	printf("IAC_Isobar's\n");
	for (uint i=0; i<list_Isobars.size(); i++)
	{
		IAC_Isobar *lsp = list_Isobars[i];
		printf("Isobar: %d ", lsp->points.size());
		for (uint j=0; j<lsp->points.size(); j++)
		{
			printf("(%5.1f,%5.1f) ", lsp->points[j]->y, lsp->points[j]->x);
		}
		printf("\n");
	}
****************/	
	printf("------------------------------------\n");
	printf("IAC_Front's\n");
	for (uint i=0; i<list_Fronts.size(); i++)
	{
		IAC_Front *lsp = list_Fronts[i];
		printf("Front: %s type=%d : ", lsp->code.c_str(), lsp->type);
		for (uint j=0; j<lsp->points.size(); j++)
		{
			printf("(%5.1f,%5.1f) ", lsp->points[j]->y, lsp->points[j]->x);
		}
		printf("\n");
	}
	

	printf("------------------------------------\n");
	printf("Zone: X(%5.1f,%5.1f) Y(%5.1f,%5.1f)\n", xmin,xmax, ymin,ymax);
}







