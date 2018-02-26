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

#include "Util.h" 
#include "MblueReader.h" 
#include "DataQString.h" 

//-------------------------------------------------------------------
MblueReader::MblueReader ()
{
	ok = false;
}
//-------------------------------------------------------------------
void MblueReader::openFile (const std::string fname, 
						    bool fastInterpolation,
							LongTaskProgress *taskProgress)
{
	this->taskProgress = taskProgress;
	if (fname != "") {
		hasAltitude = false;
		taskProgress->continueDownload = true;
		ok = false;
		openFile (fname, fastInterpolation);
	}
	else
		ok = false;
}
//---------------------------------------------------
MblueReader::~MblueReader ()
{
// 	DBGS("Destroy MblueReader");
	Util::cleanMapPointers (mapRecords);
}
//-------------------------------------------------------------------
bool MblueReader::getMeteoblueTotalArea (
						MeteoblueZone zone,
						double *x0,double *y0, double *x1,double *y1) 
{
	if (zone == MBLUE_SWISS) {
		*x0 = 3;
		*y0 = 44;
		*x1 = 14;
		*y1 = 50;
		return true;
	}
	else {
		return false;
	}
}

//-------------------------------------------------------------------
void  MblueReader::openFile (const std::string fname, bool fastInterpolation)
{
// 	DBG("MblueReader::loadFile '%s'", fname.c_str());
	fileName = fname;
	fileSize = zu_filesize_name (fname.c_str());
	hasAltitude = false;
	ok = true;
	
	taskProgress->setMessage (LTASK_OPEN_FILE);
	taskProgress->setValue (0);
	
	MbzFile mbzfile (fname.c_str(), taskProgress);
	
	if (! mbzfile.isOk()) {
		ok = false;
		return;
	}
	//----------------------------------------------------------
	taskProgress->setMessage (LTASK_ANALYSE_DATA);
	taskProgress->setValue (0);
	
	setAllDates.clear ();
	setAllDataCode.clear ();
	Util::cleanMapPointers (mapRecords);

	// read datacodes
	for (uint32_t i=0; i<mbzfile.vcodes.size(); i++) {
		DataCode dtc (mbzfile.vcodes[i]);
		setAllDataCode.insert (dtc);
//  	DBGQS (DataCodeStr::toString(dtc) );
	}
	
// mbzfile.debugmbz();
	
	// read data (each MBZline give a MbluePoint)
	uint32_t nblines = mbzfile.vlines.size();
	for (uint32_t i=0; taskProgress->continueDownload && i<nblines; i++) 
	{
		if (i%20 == 0)
			taskProgress->setValue ((int)(100*i/nblines));
		
		MbzLine *mbzline = mbzfile.vlines[i];
		
		time_t href = DataRecordAbstract::UTC_mktime 
						(mbzfile.year,mbzfile.month,mbzfile.day,mbzfile.href,0,0);
		time_t hcur = href + 3600* mbzline->hour;
		
		MblueRecord *rec;
		rec = getMblueRecordByDate (hcur);
		if (rec == NULL) {
			setAllDates.insert (hcur);
			rec = new MblueRecord (mbzfile, hcur, fastInterpolation);
			assert (rec);
			mapRecords.insert 
				( std::pair<time_t, MblueRecord  *> (hcur, rec) );
		}
		
		MbluePoint * pt = new MbluePoint (mbzfile.vcodes, mbzline);
		rec->addMbluePoint (pt);
		
		// has new point altitude data ?
		if (! hasAltitude) {
			std::map <uint32_t, float>::const_iterator iter;
			std::map <uint32_t, float> *map = pt->getMapValues ();
			for (iter=map->begin(); iter!=map->end(); iter++) {
				DataCode dtc = iter->first;
				if (dtc.levelType==LV_ISOBARIC
						&& (   dtc.levelValue==850
							|| dtc.levelValue==700
							|| dtc.levelValue==500
							|| dtc.levelValue==300 
							|| dtc.levelValue==200 
							)
				) {
					hasAltitude = true;
				}
			}
		}
	}
	if (! taskProgress->continueDownload) {
		ok = false;
		return;
	}
	//----------------------------------------------------------
	taskProgress->setMessage (LTASK_PREPARE_MAPS);
	taskProgress->setValue (0);
// 	DBG ("nb records = %d", mapRecords.size());
	// finalize all records
	std::map <time_t, MblueRecord  *>::const_iterator iter;
	int i=0, nbrec=mapRecords.size();
	xmin =  1e30;
	xmax = -1e30;
	ymin =  1e30;
	ymax = -1e30;
	for (iter = mapRecords.begin(); 
			taskProgress->continueDownload && iter != mapRecords.end(); iter++) 
	{
		taskProgress->setValue ((int)(100*i/nbrec));
		i ++;
		MblueRecord  *rec = iter->second;
		rec->finalize ();
		if (rec->isOk()) {
			if (xmin > rec->getXmin()) xmin = rec->getXmin();
			if (xmax < rec->getXmax()) xmax = rec->getXmax();
			if (ymin > rec->getYmin()) ymin = rec->getYmin();
			if (ymax < rec->getYmax()) ymax = rec->getYmax();
		}
	}
	
	if (! taskProgress->continueDownload) {
		ok = false;
		return;
	}
	
	ok = true;
	if (setAllDates.size() == 0) {
			ok = false;
	}
}
//---------------------------------------------------------------------------
MblueRecord *  MblueReader::getMblueRecordByDate (time_t date)
{
	std::map <time_t, MblueRecord  *>::const_iterator iter;
	iter = mapRecords.find (date);
	if (iter != mapRecords.end()) {
		MblueRecord  *rec = iter->second;
		return rec;
	}
	return NULL;
}
//---------------------------------------------------------------------------
GriddedRecord * MblueReader::getRecord (DataCode dtc, time_t date)
{
	std::map <time_t, MblueRecord  *>::const_iterator iter;
	iter = mapRecords.find (date);
	if (iter != mapRecords.end()) {
		MblueRecord  *rec = iter->second;
		if (rec->hasData(dtc))
			return rec;
	}
	return NULL;
}

//---------------------------------------------------------------------------
GriddedRecord * MblueReader::getFirstRecord()
{
	if (mapRecords.size() > 0)
		return mapRecords.begin()->second;
	else
		return NULL;
}

//---------------------------------------------------------------------------
double  MblueReader::getDateInterpolatedValue (
							DataCode dtc, double px, double py, time_t date)
{
	MblueRecord *rec = getMblueRecordByDate (date);
	if (rec == NULL) {
		return GRIB_NOTDEF;
	}
	else
		return rec->getInterpolatedValue (dtc, px, py);
}

//-------------------------------------------------------------------
bool MblueReader::getZoneExtension (
			 double *x0,double *y0, double *x1,double *y1)
{
	MblueRecord *rec = mapRecords.begin()->second;
	if (rec && rec->isOk()) {
		return rec->getZoneExtension (x0,y0, x1,y1);
	}
	else {
		return false;
	}
}

//-------------------------------------------------------------------
void MblueReader::setFastInterpolation (bool b)
{
	std::map <time_t, MblueRecord  *>::const_iterator iter;
	for (iter = mapRecords.begin(); iter != mapRecords.end(); iter++) 
	{
		MblueRecord  *rec = iter->second;
		rec->setFastInterpolation (b);
	}
}

//------------------------------------------------------------
time_t  MblueReader::getFirstRefDate ()
{
	return getRefDateForDataCenter (METEOBLUE_NMM4);
}
//------------------------------------------------------------
time_t  MblueReader::getRefDateForData (const DataCode &)
{
	return getRefDateForDataCenter (METEOBLUE_NMM4);
}
//------------------------------------------------------------
time_t  MblueReader::getRefDateForDataCenter (const DataCenterModel &dcm)
{
	std::map <time_t, MblueRecord  *>::const_iterator iter;
	for (iter = mapRecords.begin(); iter != mapRecords.end(); iter++) 
	{
		MblueRecord  *rec = iter->second;
		if (rec && rec->isOk() 
				&& rec->getDataCenterModel()==dcm) {
			return rec->getRecordRefDate();
		}
	}
	return 0;
}



