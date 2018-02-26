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

#ifndef GRIDDEDRECORD_H
#define GRIDDEDRECORD_H

#include <cstdio>
#include <cmath>

#include "DataDefines.h"
#include "DataMeteoAbstract.h"

//====================================================================
class GriddedRecord : public DataRecordAbstract
{
	public:
		GriddedRecord ();
		virtual ~GriddedRecord () {}
        virtual bool isOk () const = 0;

        virtual int  getIdCenter () const = 0;
        virtual int  getIdModel ()  const = 0;
        virtual int  getIdGrid ()   const = 0;
		virtual DataCenterModel getDataCenterModel () {return dataCenterModel;}
		
        virtual bool  isDuplicated ()  const   {return duplicated;}
        virtual void  setDuplicated (bool b)   {duplicated = b;}
		
        /** Is a point into the covered space ?
		*/
        virtual bool isXInMap(double x) const
						{ return entireWorldInLongitude 
									|| (x>=xmin && x<=xmax); }
        virtual bool isYInMap(double y) const
						{ return y>=ymin && y<=ymax; }
        virtual bool isPointInMap(double x, double y) const
						{ return isXInMap(x) && isYInMap(y); }
		
        virtual double  getInterpolatedValue (
							DataCode dtc,
							double px, double py,
							bool interpolate=true ) const = 0;
								
		/** Grid type.
		*/
		virtual bool isRegularGrid () const = 0;
		
		/** All records must have (or simulate) a rectangular regular grid.
		*/ 
		virtual double getValueOnRegularGrid ( 
								DataCode dtc, int i, int j ) const = 0;
		virtual double  getInterpolatedValueUsingRegularGrid (
								DataCode dtc, 
								double px, double py,
								bool interpolateValues) const;
						
		virtual int     getNi () const = 0;
        virtual int     getNj () const = 0;
        virtual double  getX (int i) const = 0;
        virtual double  getY (int j) const = 0;
        virtual double  getDeltaX () const = 0;
        virtual double  getDeltaY () const = 0;
								
        virtual int    getTotalNumberOfPoints ()  const = 0;
        virtual double getAveragePointsDensity () const = 0;
		
        virtual double  getXmin () const {return xmin;}
        virtual double  getXmax () const {return xmax;}
        virtual double  getYmin () const {return ymin;}
        virtual double  getYmax () const {return ymax;}
        virtual double  getWidth () const {return xmax-xmin;}
        virtual double  getHeight () const {return ymax-ymin;}
        
		bool   entireWorldInLongitude;
						
	protected:
		double xmin,xmax, ymin,ymax;
		bool   duplicated;
		DataCenterModel    dataCenterModel;
};



#endif
