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
		
//--------------------------------------------------		
IAC_Front::IAC_Front(std::string cod)
{
 	type = IAC_Front::IAC_FRONT_UNKNOWN;
	code = cod;	// 6_6_Ft_Fi_Fc
	int t;
	if (sscanf(code.substr(2,1).c_str(), "%d", &t) == 1) {
		switch (t) {
			case 0:
			case 1:
				type = IAC_Front::IAC_FRONT_STATIONARY;  break;
			case 2:
			case 3:
				type = IAC_Front::IAC_FRONT_WARM;  break;
			case 4:
			case 5:
				type = IAC_Front::IAC_FRONT_COLD;  break;
			case 6:
				type = IAC_Front::IAC_FRONT_OCCLUSION;  break;
			case 7:
				type = IAC_Front::IAC_FRONT_INSTABILITY_LINE;  break;
			case 8:
				type = IAC_Front::IAC_FRONT_INTERTROPICAL;  break;
			case 9:
				type = IAC_Front::IAC_FRONT_CONVERGENCE_LINE;  break;

			default:
				type = IAC_Front::IAC_FRONT_UNKNOWN;
		}
	}
}

//----------------------------------------------------------------
double	IAC_Line::distanceFromPoint(double x, double y)
{
	double dist = HUGE_VAL;
	double d;
	IAC_Point *p, *q;
	
	if (points.size() == 1) {
		p = points.at(0);
		dist = sqrt( (x-p->x)*(x-p->x) + (y-p->y)*(y-p->y) );
	}
	else if (points.size() >= 2) {
		p = points.at(0);
		for (uint j=1; j<points.size(); j++)
		{
			q = points.at(j);
			d = Util::distancePointSegment(x,y, p->x,p->y, q->x,q->y);
			if (d < dist)
				dist = d; 
			p = q;
		}
	}
	
	return dist;
}

//----------------------------------------------------------------
void IacReader::readIacFileContent()
{
	iacFileType  = IAC_UNKNOWN;
	is_NOAA_File = false;
	ok = true;
	endOfFile   = false;
	currentSECTION = -1;
	xmin = ymin =  1e20;
	xmax = ymax = -1e20;
	currentTroughLine = NULL;
	//--------------------------------------
	char *oldlocale = setlocale (LC_NUMERIC, "C");
	zu_rewind(file);
	std::string word;
	std::vector<std::string> vline;
	int i=0;
	while (! endOfFile )
	{
		i++;
		vline = readAndSplitLine();
		if (vline.size() > 0)
			decodeLine(vline);
	}
	setlocale (LC_NUMERIC, oldlocale);
	
	if (iacFileType == IAC_UNKNOWN)
		ok = false;
		
	if (! is_NOAA_File)
		ok = false;		// sorry : can't read other fleetcodes files
		
	//----------------------------------------------------------------
	// compute the nearest isobar of each pressureMinMax point
	//----------------------------------------------------------------
	if (ok) {
		for (uint i=0; i<list_HighLowPressurePoints.size(); i++)
		{
			IAC_HighLowPressurePoint *point = list_HighLowPressurePoints.at(i);
			int 	nearpres = -1;
			double 	dmax 	 = HUGE_VAL;
			for (uint j=0; j<list_Isobars.size(); j++)
			{
				IAC_Isobar *isob = list_Isobars.at(j);
				double dist = isob->distanceFromPoint(point->x, point->y);
				if (dist < dmax) {
					dmax = dist;
					nearpres = isob->value;
				}
			}
			//printf("%s : %2d %4d\n", point->type.c_str(), point->value, nearpres);
			// Adjust pressure value
			if (nearpres > 0) {
				if (point->type == "L")
				{
					// nearpres=1012  point->value=05 => completevalue=1005
					// nearpres=1000  point->value=95 => completevalue=995
					point->nearestisobar = nearpres;
					point->completevalue =
						((int)nearpres/100)*100 + point->value;
					if (point->completevalue > nearpres)
						point->completevalue -= 100;
					if ( abs(point->completevalue - nearpres) > 30 )
						point->completevalue = -1;  // error ?
					
				}
				if (point->type == "H")
				{
					// nearpres=1012  point->value=15 => completevalue=1015
					// nearpres=999  point->value=5 => completevalue=1005
					point->nearestisobar = nearpres;
					point->completevalue =
						((int)nearpres/100)*100 + point->value;
					if (point->completevalue < nearpres)
						point->completevalue += 100;
					if ( abs(point->completevalue - nearpres) > 30 )
						point->completevalue = -1;  // error ?
					
				}
			}
		}
	}
}

//---------------------------------------------------
bool IacReader::getZoneExtension(double *x0,double *y0, double *x1,double *y1)
{
	if (ok) {
		*x0 = xmin;
		*x1 = xmax;
		*y0 = ymin;
		*y1 = ymax;
		return true;
	}
	else
		return false;
}

//---------------------------------------------------
void IacReader::openFile(const std::string fname)
{
    fileName = fname;
    ok = false;
	cleanAllVectors();
    
    //--------------------------------------------------------
    // Ouverture du fichier
    //--------------------------------------------------------
    file = zu_open(fname.c_str(), "rb", ZU_COMPRESS_AUTO);
    if (file != NULL) {
    	readIacFileContent();
		zu_close(file);
	}
}


//--------------------------------------------------------
std::string IacReader::readALine()
{
	std::string str;
	char c;
	bool go = true;
	while (go && zu_read(file, &c, 1)==1)
	{
		if (c=='\n' || c=='=')	// end of line : '\n' or '='
			go = false;
		else
			str += c;
	}
	if ( go ) {
		endOfFile = true;
	}
	return str;
}
//--------------------------------------------------------
bool IacReader::is_alnum(char c)
{
  return (	   (c >= '0' && c <= '9')
  			|| (c >= 'A' && c <= 'Z')
  			|| (c >= 'a' && c <= 'z')
		);
}
//--------------------------------------------------------
bool IacReader::is_fleetChar(char c)
{
  return (	is_alnum(c)
  			|| c=='/'
		);
}

//--------------------------------------------------------
std::vector<std::string> IacReader::readAndSplitLine()
{
	std::vector<std::string> vec;
	unsigned char c;
	std::string str;
	
	std::string line = readALine();

 	//printf("line: %s\n", line.c_str());
	bool acceptAllChars = false;
	if (line.size() >= 5)
	{
		if (	line.substr(0,6)=="ASXX21"
			 || line.substr(0,6)=="FSXX21")
		{
 			//printf("line: %s\n", line.c_str());
			acceptAllChars = true;
		}
		
		if (line.substr(0,5)=="     ") {
			// line begins with an empty word
			// printf("empty\n");
			vec.push_back("empty");
		}
		str = "";
		for (uint i=0; i<line.size(); i++)
		{
			c = line[i];
			if ( (acceptAllChars && (c != ' '))  ||  is_fleetChar(c)) {
				str += c;
			}
			else if (c == ' ')
			{
				if (str!="" &&  (acceptAllChars || str.size()==5)) {
					vec.push_back(str);
					str = "";
				}
			}
		}
		if (str!="" &&  (acceptAllChars || str.size()==5)) {
			vec.push_back(str);
			str = "";
		}
	}
	return vec;
}

//-----------------------------------------------------------
void IacReader::display_vline(std::vector<std::string> &vline)
{
	for (uint i=0; i<vline.size(); i++) {
		printf("'%s' ", vline[i].c_str());
	}
	printf("\n");
}

//-----------------------------------------------------------
bool IacReader::readPosition(std::string word, double *lat, double *lon)
{
bool lonEast_above_100;
bool lonWest_above_100;
lonEast_above_100 = false;
lonWest_above_100 = false;
	
	if (	yyyyyPositionMode == POS_LaLaLoLok_NORTH
		 || yyyyyPositionMode == POS_LaLaLoLok_SOUTH
		 || yyyyyPositionMode == POS_LaLaLoLok_EQUAT
	   )
	{
		std::string LaLa = word.substr(0,2);
		std::string LoLo = word.substr(2,2);
		std::string kstr = word.substr(4,1);
		int k;
		if (sscanf(kstr.c_str(), "%d",  &k)  != 1) return false;
		if (sscanf(LaLa.c_str(), "%lf", lat) != 1) return false;
		if (sscanf(LoLo.c_str(), "%lf", lon) != 1) return false;
		switch (k) {
			case 1: 
			case 6: 
				*lat += 0.5;
				break;
			case 2: 
			case 7: 
				*lon += 0.5;
				break;
			case 3: 
			case 8: 
				*lat += 0.5;
				*lon += 0.5;
				break;
		}
		if (k < 5) {
			if (lonWest_above_100)
			 	*lon =  - *lon - 100;
		}
		else {
			if (lonEast_above_100)
				*lon =  *lon + 100;
		 	else
		 		*lon = - *lon;
		}
	//printf("pos: k=%d  %f  %f\n",  k,*lat,*lon);
	}
	else if (yyyyyPositionMode == POS_iiiD1s1) {
		return false;
	}
	else if (yyyyyPositionMode == POS_QLaLaLoLo) {
		return false;
	}
	else {	// unknown postion mode
		return false;
	}

	if (ymin > *lat)   ymin = *lat;
	if (ymax < *lat)   ymax = *lat;
	if (xmin > *lon)   xmin = *lon;
	if (xmax < *lon)   xmax = *lon;
	return true;
}



