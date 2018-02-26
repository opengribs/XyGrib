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

#ifndef GRIDDEDREADER_H
#define GRIDDEDREADER_H

#include <iostream>
#include <cmath>
#include <vector>
#include <set>
#include <map>

#include "DataMeteoAbstract.h"
#include "GriddedRecord.h"
#include "zuFile.h"

//---------------------------------------------------------------
// Minimal set of functions provided by a data reader.
//---------------------------------------------------------------
class GriddedReader : public DataReaderAbstract
{
	public:
		GriddedReader () {}
		virtual ~GriddedReader () {}

		bool  isOk ()   {return ok;}

		//virtual void openFile (const std::string fname) = 0;
		long  getFileSize ()          {return fileSize;}
		std::string getFileName ()    {return fileName;}

		/// Give the englobing rectangle of all data.
		virtual bool getZoneExtension 
					(double *x0,double *y0, double *x1,double *y1) = 0;
        virtual double  getXmin () const {return xmin;}
        virtual double  getXmax () const {return xmax;}
        virtual double  getYmin () const {return ymin;}
        virtual double  getYmax () const {return ymax;}

		virtual bool isPointInMap(double x, double y) const;

		/// Value at point(px,py) for a particuliar date.
        virtual double  getDateInterpolatedValue (
					DataCode dtc, double px, double py, time_t date) = 0;
        
        virtual std::set<time_t>  getListDates()   {return setAllDates;}
        virtual int     getNumberOfDates()   {return setAllDates.size();}
        virtual time_t  getFirstDate()       {return setAllDates.size()>0 ?
												 *setAllDates.begin() : 0;}
												 
        virtual time_t  getFirstRefDate () = 0;
        virtual time_t  getRefDateForData (const DataCode &dtc) = 0;
		virtual time_t  getRefDateForDataCenter (const DataCenterModel &dcm) = 0;

		std::set<DataCenterModel>  getAllDataCenterModel () 
										{return setAllDataCenterModel;}

		virtual time_t  getClosestDateFromDate (time_t date);
        virtual time_t  getClosestDateFromNow ();
													 
		virtual GriddedRecord *getFirstRecord() = 0;
		virtual GriddedRecord *getRecord (DataCode dtc, time_t date) = 0;
		
		virtual std::set<DataCode> getAllDataCode () const {return setAllDataCode;}
		virtual bool hasData (const DataCode &dtc) const;
		virtual bool hasDataType (int dataType) const;
		virtual bool hasAltitudeData () const = 0;
		virtual std::set<Altitude> getAllAltitudes (int dataType) const;
		virtual bool hasWaveDataType (int dataType) const;
		
		// To signal malformed grib files 
		virtual bool hasAmbiguousHeader () {return false;}
		
	protected:
        bool   ok;
        std::string fileName;
        long    fileSize;
		double xmin,xmax, ymin,ymax;

        std::set<time_t>   setAllDates;
        std::set<DataCode> setAllDataCode;
		std::set<DataCenterModel>  setAllDataCenterModel;
};


#endif
