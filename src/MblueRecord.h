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

#ifndef MBLUERECORD_H
#define MBLUERECORD_H

#include <map>
#include <list>
#include <vector>

#include "IrregularGridded.h"
#include "zuFile.h"
#include "MbzFile.h"


//------------------------------------------------------
class MbluePoint
{
	public:
		MbluePoint ();
		~MbluePoint ();

		MbluePoint (const std::vector <uint32_t> datacodes, 
					const MbzLine *mbzline);
		
		// Interpolate values from other points
		MbluePoint ( double x, double y,
					 int  nbPoints,
					 MbluePoint *p1,
					 MbluePoint *p2,
					 MbluePoint *p3,
					 MbluePoint *p4 );

		bool   hasData  (const DataCode &dtc) const;
		double getValue (const DataCode &dtc) const;
		
		float x, y;
		bool isOk() const {return ok;}
		
		std::map <uint32_t, float> * getMapValues () 
							{ return &mapvalues; }
 		
	private:
		bool ok;
		
		std::map <uint32_t, float> mapvalues;
		
};

//===================================================
// All the data valid at the same time
//===================================================
class MblueRecord : public IrregularGridRecord
{
	public:
		MblueRecord (const MbzFile &mbzfile, 
					 time_t curDate, 
					 bool fastInterpolation
					);
		~MblueRecord ();
		
		virtual std::string  getDataOrigin () const
						 { return "Meteoblue-NMM"; }
		
		void addMbluePoint (MbluePoint *pt);
		void finalize ();	// don't forget me
		bool isOk() const	{return ok;}
		void setFastInterpolation (bool b)
					{fastInterpolation = b;}
		
		
		bool hasData (const DataCode &dtc) const;
		
        virtual double  getInterpolatedValue (
						DataCode dtc,
						double px, double py,
						bool interpolateValues=true ) const;

		bool getZoneExtension (double *x0,double *y0, double *x1,double *y1);
		
        time_t getRecordRefDate () const        { return refDate; }
        time_t getRecordCurrentDate () const    { return curDate; }
	
		std::vector <MbluePoint *> * getListPoints () {return &allPoints;}
		
		/** All records have (or simulate) a rectangular regular grid.
		*/ 
		virtual double getValueOnRegularGrid (DataCode dtc, int i, int j) const;
		double getSmoothPressureMSL (int i, int j) const
					{ return smoothPressureGrid [i+j*Ni]; }
		
		virtual int     getNi () const;
        virtual int     getNj () const;
        virtual double  getX (int i) const;
        virtual double  getY (int j) const;
        virtual double  getDeltaX () const;
        virtual double  getDeltaY () const;
		
        virtual int    getTotalNumberOfPoints ()  const
						{ return ok ? allPoints.size() : 0; }
        virtual double getAveragePointsDensity () const
						{ return ok ? allPoints.size()/((xmax-xmin)*(ymax-ymin)) : 0; }

        virtual int  getIdCenter() const { return 0; }
        virtual int  getIdModel()  const { return 0; }
        virtual int  getIdGrid()   const { return 0; }
        
	private:
		bool ok;
		std::vector <MbluePoint *> allPoints;
        time_t refDate;      // Reference date
        time_t curDate;      // Current date

		std::vector <MbluePoint *> *clusters;	// table of clusters of points
		int clustersNi, clustersNj;	     // number of clusters
		double clusterWidth;
		double clusterHight;
		void  	makeClusters();
		
		MbluePoint **regularGrid;    // Virtual regular grid parameters
		int    Ni, Nj;	
		double Di, Dj;
		void   makeVirtualRegularGrid ();
		
		bool    fastInterpolation;
		
		float  *smoothPressureGrid;	    // try to reduce pressure noise
		void   makeSmoothPressureGrid ();
		
		std::vector <MbluePoint *> & getCluster (
										double x, double y) const;
										
		std::vector <std::vector <MbluePoint *> > *getClustersList (
										double x, double y) const;
										
		std::vector <std::vector <MbluePoint *> > *getClustersList_9max (
										double x, double y) const;
										
		std::vector <std::vector <MbluePoint *> > *getClustersList_4max (
										double x, double y) const;

		int  findNeighbour_clusters (
					double lon, double lat,
					MbluePoint **p1, MbluePoint **p2, 
					MbluePoint **p3, MbluePoint **p4 ) const;
		
		int   findBestNeighboursInCluster (
						double lon, double lat,
						std::vector <MbluePoint *> cluster,
						MbluePoint **p1, MbluePoint **p2, 
						MbluePoint **p3, MbluePoint **p4, 
						double *d1, double *d2, double *d3, double *d4
					) const;

		double  getInterpolatedValueWithoutGrid (
					 DataCode dtc, 
					 double px, double py,
					 bool interpolateValues=true ) const;
					 
		double getRawPressureOnRegularGrid (int i, int j) const;
};


 

#endif
