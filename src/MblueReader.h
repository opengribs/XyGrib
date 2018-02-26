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

#ifndef MBLUEREADER_H
#define MBLUEREADER_H

#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <list>
#include <map>

#include "IrregularGridded.h"
#include "zuFile.h"
#include "MblueRecord.h"
#include "MbzFile.h"

//===============================================================
enum MeteoblueZone
	{
		MBLUE_SWISS,
		MBLUE_EUROPE
	};

//===============================================================
class MblueReader : public IrregularGridReader
{
    public:
		MblueReader ();
        virtual ~MblueReader();
		
		void openFile (const std::string fname, 
					   bool fastInterpolation,
					   LongTaskProgress *taskProgress);

		void setFastInterpolation (bool b);
		
		virtual FileDataType getReaderFileDataType () 
					{return DATATYPE_MBLUE;}
		
		virtual GriddedRecord *getFirstRecord ();
		
		virtual GriddedRecord *getRecord (DataCode dtc, time_t date);
		
		// Rectangle de la zone couverte par les données
        virtual bool getZoneExtension (
					 double *x0,double *y0, double *x1,double *y1);
		
		// Rectangle de toute la zone couverte par le modèle
        static bool getMeteoblueTotalArea (
						MeteoblueZone zone,
						double *x0,double *y0, double *x1,double *y1) ;

        virtual double  getDateInterpolatedValue (
					 DataCode dtc,  double px, double py, time_t date);

		virtual bool hasAltitudeData () const  {return hasAltitude;}
		
		MblueRecord * getMblueRecordByDate (time_t date);
		
        virtual time_t  getFirstRefDate ();
        virtual time_t  getRefDateForData (const DataCode &dtc);
		virtual time_t  getRefDateForDataCenter (const DataCenterModel &dcm);
		
	protected:
		bool hasAltitude;
		
		LongTaskProgress *taskProgress;

		virtual void  openFile (const std::string fname, 
								bool fastInterpolation);
		
		std::map <time_t, MblueRecord  *> mapRecords;


};


#endif
