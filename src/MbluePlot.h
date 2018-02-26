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

#ifndef MBLUEPLOT_H
#define MBLUEPLOT_H

#include <iostream>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QPainter>
#include <QString>
#include "zuFile.h"


#include "IrregularGridded.h"
#include "MblueReader.h"

//===============================================================
class MbluePlot : public IrregularGridPlot
{
    public:
        MbluePlot ();        
        virtual ~MbluePlot();

		void  loadFile (QString fileName,
						LongTaskProgress *taskProgress, int nbrecs);
		
		virtual bool  isReaderOk() const
					{return reader!=NULL && reader->isOk();}						

		virtual MblueReader *getReader () const {return reader;};

		virtual void setFastInterpolation (bool b)
						{if (reader) reader->setFastInterpolation(b);}
		
		//---------------------------------------------------
		/** Draw the complete colored map.
		*/
		virtual void draw_ColoredMapPlain ( 
						DataCode dtc,
						bool smooth,
						QPainter &pnt, 
						const Projection *proj);
		
		/** Draw wind arrows on the colored map.
		*/
		virtual void draw_WIND_Arrows (
						Altitude altitude, 
						bool showBarbules, QColor windArrowsColor, 
						QPainter &pnt, const Projection *proj );
		
		/** Draw current arrows on the colored map.
		*/
		virtual void draw_CURRENT_Arrows (
						Altitude altitude, 
						QColor currentArrowsColor, 
						QPainter &pnt, const Projection *proj );
		
		/** Draw waves arrows on the colored map.
		*/
		virtual void draw_WAVES_Arrows (
						const DataCode &,
						QPainter &, const Projection *) 
		{}
		
		/** Draw a mark a each grid vertex.
		*/
        virtual void draw_GridPoints (
						const DataCode &dtc,
						QPainter &pnt, const Projection *proj );

		/** Pressure: write H and L at hight and low points (pressure).
		*/
        virtual void draw_DATA_MinMax (
						DataCode dtc, 
						double   meanValue,
						QString  minSymbol,
						QString  maxSymbol,
						QFont 	 labelsFont,
						QColor   labelsColor,
						QPainter &pnt, const Projection *proj);
						
	private :
		MblueReader *reader;
		
};


#endif
