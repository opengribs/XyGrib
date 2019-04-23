/**********************************************************************
XyGrib: meteorological GRIB file viewer
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

#include <ctime>

#include "Util.h"
#include "GriddedReader.h"

int GriddedReader::getDataTypeAlias (int dataType) const
{
	if (dataType == GRB_PRV_WIND_XY2D || dataType==GRB_PRV_WIND_JET)
		return GRB_WIND_VX;
	if (dataType == GRB_PRV_CUR_XY2D)
		return GRB_CUR_VX;
	if (dataType == GRB_PRV_DIFF_TEMPDEW)
		return GRB_DEWPOINT;

	return dataType;
}

//------------------------------------------------------------
bool GriddedReader::hasDataType (int dataType) const
{
	int type = getDataTypeAlias(dataType);
	bool found = false;
	std::set<DataCode> setdata = getAllDataCode ();
	std::set<DataCode>::iterator it;
	for (it=setdata.begin(); !found && it!=setdata.end(); ++it) {
		DataCode dt2 = *it;
		if (dt2.dataType == type)
			found = true;
	}
	return found;  
}

//------------------------------------------------------------
bool GriddedReader::hasData (const DataCode &dtc) const
{
	int type = getDataTypeAlias(dtc.dataType);

	DataCode dtcsearch (type, dtc.getAltitude());
	bool found = false;
	std::set<DataCode> setdata = getAllDataCode ();
	std::set<DataCode>::iterator it;
	for (it=setdata.begin(); !found && it!=setdata.end(); ++it) {
		DataCode dt2 = *it;
		if (dt2 == dtcsearch)
			found = true;
	}
	return found;  
}

//------------------------------------------------------------
std::set<Altitude> GriddedReader::getAllAltitudes (int dataType) const
{
	//DBG("%d", dataType);
	int type = getDataTypeAlias(dataType);

	std::set<Altitude> res;
	std::set<DataCode> setdata = getAllDataCode ();
	for (auto dtc : setdata) {
		if (dtc.dataType == type)
			res.insert (dtc.getAltitude());
	}
	return res; 
}
//------------------------------------------------------------
time_t  GriddedReader::getClosestDateFromNow ()
{
// 	time_t now = time(NULL);
// 	struct tm *pstm;
// 	pstm = gmtime(&now);
// 	now = mktime(pstm);
	
	time_t utcnow = QDateTime::currentDateTime().toUTC().toTime_t();
	return getClosestDateFromDate (utcnow);
}
//------------------------------------------------------------
time_t  GriddedReader::getClosestDateFromDate (time_t date)
{
	std::set<time_t> sdates = getListDates();

	time_t closestdate = 0;
	uint difftime, difftimemin=0xFFFFFFFF;
	
	for (long daterecord : sdates)
	{
		difftime = abs (daterecord-date);
		// find closets date +- 2*24h
		if (difftimemin>difftime && (difftime/3600)<=24*2) {
			difftimemin = difftime;
			closestdate = daterecord;
			//DBG("%d", closestdate);
		}
	}
	return closestdate;
}
//--------------------------------------------------------------
bool GriddedReader::isPointInMap (double x, double y) const
{ 
// 	DBG ("%g %g %g", x,xmin,xmax);
	return x>=xmin && x<=xmax && y>=ymin && y<=ymax; 
}
//------------------------------------------------------------
bool GriddedReader::hasWaveDataType (int dataType)  const
{ 
	bool ok = false;
	switch (dataType) {
		case GRB_PRV_WAV_SIG :
			ok = hasDataType (GRB_WAV_SIG_HT)
			  || hasDataType (GRB_WAV_DIR)
			  || hasDataType (GRB_WAV_PER);
			break;
		case GRB_PRV_WAV_MAX :
			ok = hasDataType (GRB_WAV_MAX_DIR)
			  || hasDataType (GRB_WAV_MAX_HT)
			  || hasDataType (GRB_WAV_MAX_PER);
			break;
		case GRB_PRV_WAV_SWL :
			ok = hasDataType (GRB_WAV_SWL_DIR)
			  || hasDataType (GRB_WAV_SWL_HT)
			  || hasDataType (GRB_WAV_SWL_PER);
			break;
		case GRB_PRV_WAV_WND :
			ok = hasDataType (GRB_WAV_WND_DIR)
			  || hasDataType (GRB_WAV_WND_HT)
			  || hasDataType (GRB_WAV_WND_PER);
			break;
		case GRB_PRV_WAV_PRIM :
			ok = hasDataType (GRB_WAV_PRIM_DIR)
			  || hasDataType (GRB_WAV_PRIM_PER);
			break;
		case GRB_PRV_WAV_SCDY :
			ok = hasDataType (GRB_WAV_SCDY_DIR)
			  || hasDataType (GRB_WAV_SCDY_PER);
			break;
		case GRB_TYPE_NOT_DEFINED :
			ok = hasDataType (GRB_WAV_SIG_HT)
			  || hasDataType (GRB_WAV_DIR)
			  || hasDataType (GRB_WAV_PER)
			  || hasDataType (GRB_WAV_MAX_DIR)
			  || hasDataType (GRB_WAV_MAX_HT)
			  || hasDataType (GRB_WAV_MAX_PER)
			  || hasDataType (GRB_WAV_SWL_DIR)
			  || hasDataType (GRB_WAV_SWL_HT)
			  || hasDataType (GRB_WAV_SWL_PER)
			  || hasDataType (GRB_WAV_WND_DIR)
			  || hasDataType (GRB_WAV_WND_HT)
			  || hasDataType (GRB_WAV_WND_PER)
			  || hasDataType (GRB_WAV_PRIM_DIR)
			  || hasDataType (GRB_WAV_PRIM_PER)
			  || hasDataType (GRB_WAV_SCDY_DIR)
			  || hasDataType (GRB_WAV_SCDY_PER)
			  || hasDataType (GRB_WAV_WHITCAP_PROB);
			break;
		default :
			ok = hasDataType (dataType);
	}
	return ok;
}





