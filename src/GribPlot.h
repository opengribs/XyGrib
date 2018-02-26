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

/*************************************
Dessin des données GRIB (avec QT)
*************************************/

#ifndef GRIBPLOT_H
#define GRIBPLOT_H

#include "RegularGridded.h"

#include "GribReader.h"
#include "DataPointInfo.h"
#include "IsoLine.h"
 
//===============================================================
class GribPlot : public RegularGridPlot
{
    public:
        GribPlot ();
        GribPlot (const GribPlot &);
        virtual ~GribPlot ();
        
		virtual void  loadFile (QString fileName,
						LongTaskProgress *taskProgress=NULL, int nbrecs=0);
		
        GribReader *getReader()  const  {return gribReader;}

		virtual void  setCurrentDate (time_t t);

		virtual bool  isReaderOk() const  
						{return gribReader!=NULL && gribReader->isOk();}

		virtual void duplicateFirstCumulativeRecord (bool b);
		virtual void duplicateMissingWaveRecords (bool b);

		//-----------------------------------------------------
		// Draw a complete colored map
		//-----------------------------------------------------
		virtual void draw_ColoredMapPlain ( 
						DataCode dtc,
						bool smooth,
						QPainter &pnt, 
						const Projection *proj);

		virtual void draw_WIND_Arrows (
						Altitude altitude, 
						bool showBarbules, QColor windArrowsColor, 
						QPainter &pnt, const Projection *proj );

		virtual void draw_CURRENT_Arrows (
						Altitude altitude, 
						QColor currentArrowsColor, 
						QPainter &pnt, const Projection *proj );
		
		virtual void draw_WAVES_Arrows (
						const DataCode &dtc,
						QPainter &pnt, const Projection *proj );

		//-----------------------------------------------------        
		// draw a mark a each grid vertex
        virtual void draw_GridPoints (
						const DataCode &dtc,
						QPainter &pnt, const Projection *proj );
    
    protected:
        void       	initNewGribPlot (
						bool interpolateValues=true, 
						bool windArrowsOnGribGrid=true,
						bool currentArrowsOnGribGrid=true );
        
		GribReader 	*gribReader;        
        QString 	fileName;
};

#endif
