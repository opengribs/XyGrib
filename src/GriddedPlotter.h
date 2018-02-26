
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

#ifndef GRIDDEDPLOTTER_H
#define GRIDDEDPLOTTER_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <set>
#include <map>

#include <QApplication>
#include <QPainter>

#include "DataMeteoAbstract.h"
#include "DataColors.h"
#include "GriddedReader.h"
#include "Projection.h"
#include "IsoLine.h"
#include "Util.h"
#include "LongTaskProgress.h"


//===============================================================
class GriddedPlotter : 
		public DataPlotterAbstract, 
		public DataColors
{
    public:
		GriddedPlotter ();
		virtual ~GriddedPlotter ();
		
		virtual bool  isReaderOk () const = 0;
		virtual GriddedReader *getReader () const = 0;
		virtual void  loadFile (QString fileName, 
								LongTaskProgress *taskProgress,
								int nbrecs=0) = 0;
		
		virtual void  updateGraphicsParameters ();
		
		//----------------------------------------------------------------
		// Data manipulation
		//----------------------------------------------------------------
        virtual void     setCurrentDate (time_t t) = 0;
        virtual void     setCurrentDateClosestFromNow ();
        virtual time_t   getCurrentDate () const    {return currentDate;}
        virtual time_t   getClosestDateFromNow  () const
							{ return isReaderOk() ? 
								getReader()->getClosestDateFromNow():0;}
		
        virtual std::set<time_t> * getListDates ()  {return &listDates;}
        virtual int getNumberOfDates ()  {return listDates.size();}
						
		virtual std::set<Altitude> getAllAltitudes (int dataType) const; 
		virtual std::set<DataCode> getAllDataCode ()  const;
		
		virtual bool hasData (const DataCode &dtc) const
						{ return isReaderOk() && getReader()->hasData(dtc); }
		virtual bool hasData (int dataType, int levelType, int levelValue)  const
						{ return hasData (DataCode(dataType,levelType,levelValue)); }
		virtual bool hasData (int dataType, const Altitude &alt)  const
						{ return hasData (DataCode(dataType,alt.levelType,alt.levelValue)); }
						
		virtual bool hasDataType (int dataType)  const
						{ return isReaderOk() && getReader()->hasDataType(dataType); }
						
		virtual bool hasWaveDataType (int dataType=GRB_TYPE_NOT_DEFINED)  const;
		
		//----------------------------------------------------------------
		// Drawing parameters
		//----------------------------------------------------------------
		virtual void setFastInterpolation (bool b)
							{fastInterpolation = b;}
							
		virtual void setInterpolateValues (bool b)
							{mustInterpolateValues = b;}
		virtual void setWindArrowsOnGrid  (bool b)
							{drawWindArrowsOnGrid = b;}
		virtual void setCurrentArrowsOnGrid  (bool b)
							{drawCurrentArrowsOnGrid = b;}
		virtual void duplicateFirstCumulativeRecord (bool b)
							{mustDuplicateFirstCumulativeRecord = b;}
		virtual void duplicateMissingWaveRecords (bool b)
							{mustDuplicateMissingWaveRecords = b;}
		virtual void setUseJetStreamColorMap (bool b)
							{useJetStreamColorMap = b;}
        virtual Altitude getWindAltitude () 
							{return windAltitude;}
		
        virtual Altitude getCurrentAltitude () 
							{return currentAltitude;}
		
		//----------------------------------------------------------------
		// Drawing functions (virtual not pure)
		//----------------------------------------------------------------
		/** Data: write numerical values on the map (temperature).
		*/
        virtual void draw_DATA_Labels (
						DataCode dtc, 
						QFont 	 labelsFont,
						QColor   labelsColor,
						QString  (formatLabelFunction) (float v, bool withUnit),
						QPainter &pnt, const Projection *proj);

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
		
		//----------------------------------------------------------------
		// Drawing functions (pure virtual)
		//----------------------------------------------------------------
		/** Draw the complete colored map.
			Update windAltitude.
		*/
		virtual void draw_ColoredMapPlain ( 
						DataCode dtc,
						bool smooth,
						QPainter &pnt, 
						const Projection *proj) = 0;
		
		/** Draw wind arrows on the colored map.
			Update windAltitude.
		*/
		virtual void draw_WIND_Arrows (
						Altitude altitude, 
						bool showBarbules, QColor windArrowsColor, 
						QPainter &pnt, const Projection *proj ) = 0;
		
		/** Draw current arrows on the colored map.
			Update currentAltitude.
		*/
		virtual void draw_CURRENT_Arrows (
						Altitude altitude, 
						QColor currentArrowsColor, 
						QPainter &pnt, const Projection *proj ) = 0;
		
		/** Draw waves arrows on the colored map.
		*/
		virtual void draw_WAVES_Arrows (
						const DataCode &dtc,
						QPainter &pnt, const Projection *proj ) = 0;
		
		/** Draw a mark a each grid vertex.
		*/
        virtual void draw_GridPoints (
						const DataCode &dtc,
						QPainter &pnt, const Projection *proj ) = 0;
        
		//----------------------------------------------------------------
		// Isolines functions
		//----------------------------------------------------------------
		/** Draw all the Isolines contained in a list.
		*/
        virtual void complete_listIsolines (
						std::vector <IsoLine *> *listIsolines,
						DataCode dtc,
						double dataMin, double dataMax, double dataStep,
						const Projection *proj);
						
        void draw_listIsolines (
						std::vector <IsoLine *> & listIsolines,
						QPainter &pnt, const Projection *proj);

        void draw_listIsolines_labels (
						std::vector <IsoLine *> & listIsolines,
						double coef, 
						double offset,
						QColor &color,
						QPainter &pnt, 
						const Projection *proj,
						int density = -1
  					);

						
		//----------------------------------------------------------------
		// Drawing auxiliary functions
		//----------------------------------------------------------------
		void draw_CoveredZone (QPainter &pnt, const Projection *proj);

		void drawWindArrowWithBarbs (
        			QPainter &pnt, int i, int j,
        			double vx, double vy,
        			bool south,
        			QColor arrowColor=Qt::white);
		
		static void drawWindArrowWithBarbs_static (
        			QPainter &pnt, int i, int j,
        			double vx, double vy,
        			bool south,
        			QColor arrowColor,
					int  windBarbuleSize,
					bool thinWindArrows
  				);
		
		// Called from MeteoTable
		void drawCurrentArrow (
        			QPainter &pnt, int i, int j,
        			double cx, double cy,
        			bool south,
        			QColor arrowColor=Qt::white);
		

	protected:
        time_t  	currentDate;
        std::set<time_t>    listDates;     // liste des dates des GribRecord

		bool    mustInterpolateValues;
		bool    fastInterpolation;
		bool    drawWindArrowsOnGrid;
		bool    drawCurrentArrowsOnGrid;
		bool	mustDuplicateFirstCumulativeRecord;
		bool    mustDuplicateMissingWaveRecords;
		bool    thinWindArrows;
		bool 	useJetStreamColorMap;

		Altitude windAltitude;		  // current wind altitude
		Altitude currentAltitude;	  // current altitude
		
		QColor windArrowColor;        // couleur des flèches du vent
		int    windArrowSpace;        // distance mini entre flèches (pixels)
        int    windArrowSpaceOnGrid;  // distance mini entre flèches si affichage sur grille
        int    windBarbuleSpace;      // distance mini entre flèches (pixels)
        int    windBarbuleSpaceOnGrid;  // distance mini entre flèches
		QColor currentArrowColor;        // couleur des flèches du courant
		int    currentArrowSpace;        // distance mini entre flèches (pixels)
        int    currentArrowSpaceOnGrid;  // distance mini entre flèches si affichage sur grille

        void    drawWindArrow (QPainter &pnt, int i, int j, double vx, double vy);
        void    drawWaveArrow (QPainter &pnt, int i, int j, double dir, double period);
        void    drawCurrentArrow (QPainter &pnt, int i, int j, double vx, double vy);

		//-----------------------------------------------------------------
		void  drawColorMapGeneric_1D (
				QPainter &pnt, const Projection *proj, bool smooth,
				DataCode dtc,
				QRgb (DataColors::*function_getColor) (double v, bool smooth)
			);
		void  drawColorMapGeneric_2D (
				QPainter &pnt, const Projection *proj, bool smooth,
				DataCode dtcX, DataCode dtcY, 
				QRgb (DataColors::*function_getColor) (double v, bool smooth)
			);
		void  drawColorMapGeneric_Abs_Delta_Data (
				QPainter &pnt, const Projection *proj, bool smooth,
				DataCode dtc1, DataCode dtc2, 
				QRgb (DataColors::*function_getColor) (double v, bool smooth)
			);
		
		void analyseVisibleGridDensity (const Projection *proj, GriddedRecord *rec, 
										double coef, int *deltaI, int *deltaJ);

		
	private:
        int    windArrowSize;         // longueur des flèches
        int    windBarbuleSize;       // longueur des flèches

        static void drawTransformedLine( QPainter &pnt,
                double si, double co,int di, int dj, int i,int j, int k,int l);
        
        static void drawPetiteBarbule(QPainter &pnt, bool south,
                    double si, double co, int di, int dj, int b);
        static void drawGrandeBarbule(QPainter &pnt,  bool south,
                    double si, double co, int di, int dj, int b);
        static void drawTriangle(QPainter &pnt, bool south,
                    double si, double co, int di, int dj, int b);
};






#endif
