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

#ifndef MAPDRAWER_H
#define MAPDRAWER_H

#include <QWidget>
#include <QBitmap>

#include "GshhsReader.h"
#include "GisReader.h"
#include "Projection.h"
#include "POI.h"

#include "GribPlot.h"
#include "IacPlot.h"
#include "IrregularGridded.h"


//==============================================================================
class MapDrawer : public QObject
{ Q_OBJECT

friend class Terrain;	// TODO (or not) getters setters

	public:
		MapDrawer(GshhsReader *gshhsReader);
		MapDrawer(const MapDrawer &model);
		~MapDrawer();

		void draw_GSHHS_and_GriddedData (
				QPainter &pntGlobal,
				bool mustRedraw,
				bool isEarthMapValid,
				Projection     *proj,
				GriddedPlotter *plotter,
				bool drawCartouche = true
			);
		
		void draw_GSHHS_and_IAC (
				QPainter &pntGlobal,
				bool mustRedraw,
				bool isEarthMapValid,
				Projection *proj,
				IacPlot    *iacPlot,
				bool drawCartouche = true
			);

		void draw_GSHHS (
				QPainter &pntGlobal,
				bool mustRedraw,
				bool isEarthMapValid,
				Projection *proj
			);
		
		// Cartouche : dates de la prévision courante + infos générales
		void draw_Cartouche_Gridded (
					QPainter &pnt, const Projection *proj, 
					GriddedPlotter *gribPlot );
						
		void draw_Cartouche_IAC  (
					QPainter &pnt, const Projection *proj, IacPlot *iacPlot);

		DataCode getColorMapData () {return colorMapData;}
		void setColorMapData (const DataCode &dtc) {colorMapData = dtc;}
		
		void setGeopotentialData (const DataCode &dtc);
		DataCode getGeopotentialData () {return geopotentialData;}
		
		QPixmap * createPixmap_GriddedData ( 
						time_t date, 
						bool isEarthMapValid, 
						GriddedPlotter *plotter,
						Projection *proj,
						QList<POI*> lspois );
					
	private:
		QPixmap     *imgEarth;   // images précalculées pour accélérer l'affichage
		QPixmap     *imgAll;
		
		GshhsReader *gshhsReader;
		bool         gshhsReaderIsNew;
		
		GisReader	*gisReader;
		bool		 gisReaderIsNew;
		
		int   showCitiesNamesLevel;
		bool  showCountriesNames;
		bool  showCountriesBorders;
		bool  showRivers;
		bool  showLonLatGrid;
		
		std::set<DataCenterModel> setUsedDataCenters;
		DataCode  colorMapData;
		bool      colorMapSmooth;
		
		Altitude  temperatureLabelsAlt;
		bool  showTemperatureLabels;
		
		double isobarsStep;
		bool   showIsobars;
		bool   showIsobarsLabels;
		bool   showPressureMinMax;

		DataCode geopotentialData;
		bool     showGeopotential;
		bool     showGeopotentialLabels;
		int      geopotentialStep;
		int      geopotentialMin;
		int      geopotentialMax;

		double 	isotherms0Step;
		bool   	showIsotherms0;
		bool   	showIsotherms0Labels;

		double 	isotherms_Step;
		bool 	showIsotherms;
		bool 	showIsotherms_Labels;
		Altitude isothermsAltitude;

		double 	linesThetaE_Step;
		bool 	showLinesThetaE;
		bool 	showLinesThetaE_Labels;
		Altitude linesThetaEAltitude;
		
		bool  showWindArrows;
		bool  hasWindForArrows;
		Altitude windArrowsAltitude;
		bool  showGribGrid;
		bool  showBarbules;

		bool  showCurrentArrows;
		bool  hasCurrentForArrows;
		int   showWaveArrowsType;
		
		Altitude currentArrowsAltitude;
		
		QColor  seaColor, landColor, backgroundColor;
		QColor  windArrowsColor;	
		QColor  currentArrowsColor;	
		QPen	isobarsPen;
		QPen	geopotentialsPen;
		QPen	isotherms0Pen;
		QPen	isotherms_Pen;
		QPen	linesThetaE_Pen;
		QPen	seaBordersPen;
		QPen	boundariesPen;
		QPen	riversPen;

		void	updateGraphicsParameters();
		void	initGraphicsParameters  ();
		void    addUsedDataCenterModel (const DataCode &dtc, GriddedPlotter *plotter);
		
		void 	draw_MeteoData_IAC   (QPainter &pnt, Projection *proj, IacPlot *iacPlot);
		void    draw_MeteoData_Gridded 
						( QPainter &pnt, Projection *proj,
						GriddedPlotter   *plotter );

		void	draw_Map_Background  (bool isEarthMapValid, Projection *proj);
		void	draw_Map_Foreground  (QPainter &pnt, Projection *proj);
};


#endif
