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

#include <iostream>
#include <cassert>

#include <QApplication>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

#include "MapDrawer.h"
#include "LonLatGrid.h"
#include "Font.h"
#include "DataQString.h"

//===========================================================
MapDrawer::MapDrawer(std::shared_ptr<GshhsReader> gshhsReader)
{
    imgEarth = nullptr;
    imgAll   = nullptr;

    gisReader = std::make_shared<GisReader>();

	this->gshhsReader = gshhsReader;
	
	initGraphicsParameters();
	updateGraphicsParameters();
}
//---------------------------------------------------------------------
MapDrawer::MapDrawer(const MapDrawer &model)
{
    imgEarth = nullptr;
    imgAll   = nullptr;
	gisReader = model.gisReader;
    
	this->gshhsReader = model.gshhsReader;
	
	colorMapData = model.colorMapData;
	colorMapSmooth = model.colorMapSmooth;
	temperatureLabelsAlt = model.temperatureLabelsAlt;
	windArrowsAltitude   = model.windArrowsAltitude;
	currentArrowsAltitude   = model.currentArrowsAltitude;
	//geopotentialData = model.geopotentialData;
	//showGeopotential = model.showGeopotential;
	//showGeopotentialLabels = model.showGeopotentialLabels;
	//geopotentialStep = model.geopotentialStep;
	//geopotentialMin = model.geopotentialMin;
	//geopotentialMax = model.geopotentialMax;
	showWaveArrowsType = model.showWaveArrowsType;
	
	initGraphicsParameters();
	updateGraphicsParameters();
}
//---------------------------------------------------------------------
MapDrawer::~MapDrawer()
{
    delete imgAll;
    delete imgEarth;
}

//===========================================================
void MapDrawer::initGraphicsParameters()
{
    showCitiesNamesLevel = Util::getSetting("showCitiesNamesLevel", 0).toInt();
    showCountriesNames = Util::getSetting("showCountriesNames", false).toBool();
    showCountriesBorders  = Util::getSetting("showCountriesBorders", true).toBool();
    showRivers   = Util::getSetting("showRivers", false).toBool();
    showLonLatGrid   = Util::getSetting("showLonLatGrid", true).toBool();

    showIsobars  = Util::getSetting("showIsobars", true).toBool();
    showIsobarsLabels = Util::getSetting("showIsobarsLabels", false).toBool();
    isobarsStep = Util::getSetting("isobarsStep", 2).toDouble();
    showPressureMinMax = Util::getSetting("showPressureMinMax", false).toBool();

    showIsotherms0  = Util::getSetting("showIsotherms0", false).toBool();
    showIsotherms0Labels  = Util::getSetting("showIsotherms0Labels", false).toBool();
    isotherms0Step = Util::getSetting("isotherms0Step", 50).toDouble();

    showIsotherms = Util::getSetting("showIsotherms", false).toBool();
    showIsotherms_Labels  = Util::getSetting("showIsotherms_Labels", false).toBool();
    isotherms_Step = Util::getSetting("isotherms_Step", 2).toDouble();
	QString stralt = AltitudeStr::serialize (Altitude(LV_ABOV_GND,2));
	stralt = Util::getSetting("isothermsAltitude", stralt).toString();
	isothermsAltitude = AltitudeStr::unserialize (stralt);

    showLinesThetaE = Util::getSetting("showLinesThetaE", false).toBool();
    showLinesThetaE_Labels  = Util::getSetting("showLinesThetaE_Labels", false).toBool();
    linesThetaE_Step = Util::getSetting("linesThetaE_Step", 2).toDouble();
	stralt = AltitudeStr::serialize (Altitude(LV_ISOBARIC,850));
	stralt = Util::getSetting("linesThetaEAltitude", stralt).toString();
	linesThetaEAltitude = AltitudeStr::unserialize (stralt);
	
	QString  strdtc = DataCodeStr::serialize (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,10));
    strdtc = Util::getSetting("colorMapData", strdtc).toString();
	colorMapData = DataCodeStr::unserialize (strdtc);

    showGeopotential = Util::getSetting("drawGeopotentialLines", false).toBool();
    showGeopotentialLabels = Util::getSetting("drawGeopotentialLinesLabels", false).toBool();
	strdtc = DataCodeStr::serialize (DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,925));
    strdtc = Util::getSetting("geopotentialLinesData", strdtc).toString();
	setGeopotentialData (DataCodeStr::unserialize (strdtc));

	colorMapSmooth = Util::getSetting("colorMapSmooth", true).toBool();
    showWindArrows = Util::getSetting("showWindArrows", true).toBool();
    showBarbules  = Util::getSetting("showBarbules", true).toBool();

    showCurrentArrows  = Util::getSetting("showCurrentArrows", true).toBool();

    showTemperatureLabels = Util::getSetting("showTemperatureLabels", false).toBool();
    showGribGrid = Util::getSetting("showGribGrid", false).toBool();
}

//-------------------------------------------
void MapDrawer::updateGraphicsParameters()
{
    backgroundColor  = Util::getSetting("backgroundColor", QColor(0,0,45)).value<QColor>();
    seaColor  = Util::getSetting("seaColor", QColor(67,193,183)).value<QColor>();
    landColor = Util::getSetting("landColor", QColor(200,200,120)).value<QColor>();

    seaBordersPen.setColor(Util::getSetting("seaBordersLineColor", QColor(40,45,30)).value<QColor>());
    seaBordersPen.setWidthF(Util::getSetting("seaBordersLineWidth", 1.8).toDouble());

    boundariesPen.setColor(Util::getSetting("boundariesLineColor", QColor(40,40,40)).value<QColor>());
    boundariesPen.setWidthF(Util::getSetting("boundariesLineWidth", 1.4).toDouble());

    riversPen.setColor(Util::getSetting("riversLineColor", QColor(50,50,150)).value<QColor>());
    riversPen.setWidthF(Util::getSetting("riversLineWidth", 1.0).toDouble());

    isobarsPen.setColor(Util::getSetting("isobarsLineColor", QColor(80,80,80)).value<QColor>());
    isobarsPen.setWidthF(Util::getSetting("isobarsLineWidth", 2.0).toDouble());

    isotherms0Pen.setColor(Util::getSetting("isotherms0LineColor", QColor(200,120,100)).value<QColor>());
    isotherms0Pen.setWidthF(Util::getSetting("isotherms0LineWidth", 1.6).toDouble());

    isotherms_Pen.setColor(Util::getSetting("isotherms_LineColor", QColor(40,40,150)).value<QColor>());
    isotherms_Pen.setWidthF(Util::getSetting("isotherms_LineWidth", 1.6).toDouble());

    linesThetaE_Pen.setColor(Util::getSetting("linesThetaE_LineColor", QColor(40,140,40)).value<QColor>());
    linesThetaE_Pen.setWidthF(Util::getSetting("linesThetaE_LineWidth", 1.6).toDouble());

    geopotentialsPen.setColor(Util::getSetting("geopotentials_LineColor", QColor(0, 140, 0)).value<QColor>());
    geopotentialsPen.setWidthF(Util::getSetting("geopotentials_LineWidth", 0.3).toDouble());
}

//---------------------------------------------------------------------
void MapDrawer::setGeopotentialData (const DataCode &dtc)
{

	if (dtc.getAltitude().levelType == LV_ISOBARIC) {
		geopotentialData = dtc;
		switch (dtc.getAltitude().levelValue) {
			case 925:
				geopotentialMin = 250;
				geopotentialMax = 2000;
				break;
			case 850:
				geopotentialMin = 1000;
				geopotentialMax = 2000;
				break;
			case 700:
				geopotentialMin = 2000;
				geopotentialMax = 4000;
				break;
			case 600:
				geopotentialMin = 2000;
				geopotentialMax = 6000;
				break;
			case 500:
				geopotentialMin = 4000;
				geopotentialMax = 7000;
				break;
			case 400:
				geopotentialMin = 5000;
				geopotentialMax = 10000;
				break;
			case 300:
				geopotentialMin = 7000;
				geopotentialMax = 11000;
				break;
			case 200:
				geopotentialMin = 8000;
				geopotentialMax = 16000;
				break;
		}
		geopotentialStep = Util::getSetting("drawGeopotentialLinesStep", 10).toInt();
	}
	else {
		geopotentialMin = 0;
		geopotentialMax = 0;
	}
}

//---------------------------------------------------------------------
void MapDrawer::draw_Map_Background(bool isEarthMapValid, Projection *proj)
{

    delete imgAll;
	imgAll = new QPixmap(proj->getW(), proj->getH());
	assert(imgAll);

	QPainter pnt(imgAll);
	pnt.setRenderHint(QPainter::Antialiasing, true);

	if (!isEarthMapValid)
	{

        delete imgEarth;
		imgEarth = new QPixmap(proj->getW(), proj->getH());
		assert(imgEarth);

        if (gshhsReader.get() != nullptr)
		{
			QPainter pnt1(imgEarth);
			pnt1.setRenderHint(QPainter::Antialiasing, false);
			gshhsReader.get()->drawBackground(pnt1, proj, seaColor, backgroundColor);
			gshhsReader.get()->drawContinents(pnt1, proj, seaColor, landColor);
		}
	}
	pnt.drawPixmap(0,0, *imgEarth);
}
//----------------------------------------------------------------------
void MapDrawer::draw_Map_Foreground(QPainter &pnt, Projection *proj)
{
    if (gshhsReader.get() != nullptr)
	{
		pnt.setPen(seaBordersPen);
		gshhsReader.get()->drawSeaBorders(pnt, proj);

		if (showCountriesBorders) {
			pnt.setPen(boundariesPen);
			gshhsReader.get()->drawBoundaries(pnt, proj);
		}
		if (showRivers) {
			pnt.setPen(riversPen);
			gshhsReader.get()->drawRivers(pnt, proj);
		}
	}
	if (showLonLatGrid) {
		LonLatGrid gr;
		gr.drawLonLatGrid(pnt, proj);
	}
	if (showCountriesNames) {
		gisReader.get()->drawCountriesNames(pnt, proj);
	}
	if (showCitiesNamesLevel > 0) {
		gisReader.get()->drawCitiesNames(pnt, proj, showCitiesNamesLevel);
	}
}

//=======================================================================
// GSHHS Map
//=======================================================================
void MapDrawer::draw_GSHHS (
			QPainter &pntGlobal,
			bool mustRedraw, bool isEarthMapValid,
			Projection *proj
	)
{
    if (mustRedraw  ||  !isEarthMapValid)
    {
		//===================================================
		// Dessin du fond de carte
		//===================================================
		draw_Map_Background (isEarthMapValid, proj);
		//===================================================
		// Dessin des bordures et frontières
		//===================================================
		QPainter pnt (imgAll);
		pnt.setRenderHint (QPainter::Antialiasing, true);
		draw_Map_Foreground (pnt, proj);
    }
    // Recopie l'image complète
    pntGlobal.drawPixmap (0,0, *imgAll);
}

//=======================================================================
// Gridded data
//=======================================================================
void MapDrawer::draw_GSHHS_and_GriddedData (
			QPainter &pntGlobal,
			bool mustRedraw, bool isEarthMapValid,
			Projection *proj,
			GriddedPlotter   *plotter,
			bool drawCartouche
	)
{
    if (mustRedraw  ||  !isEarthMapValid)
    {
		//===================================================
		// Dessin du fond de carte
		//===================================================
		draw_Map_Background (isEarthMapValid, proj);
		//===================================================
		// Dessin des données Meteo
		//===================================================
		QPainter pnt (imgAll);
		pnt.setRenderHint (QPainter::Antialiasing, true);
		draw_MeteoData_Gridded (pnt, proj, plotter);

		//===================================================
		// Dessin des bordures et frontières
		//===================================================
		draw_Map_Foreground(pnt, proj);

		//===================================================
		// Cartouche
		//===================================================
		if (drawCartouche)
			draw_Cartouche_Gridded (pnt, proj, plotter);
    }
    // Recopie l'image complète
    pntGlobal.drawPixmap(0,0, *imgAll);
}
//===================================================================
void MapDrawer::addUsedDataCenterModel (const DataCode &dtc, GriddedPlotter *plotter)
{
	auto reader = plotter->getReader();
	assert(reader != nullptr);

	int type = reader->getDataTypeAlias(dtc.dataType);

	GriddedRecord *rec = reader->getRecord
			(DataCode(type,dtc.levelType,dtc.levelValue),  plotter->getCurrentDate());
	if (rec != nullptr) {
		setUsedDataCenters.insert (rec->getDataCenterModel());
	}
}
//===================================================================
// Draw gridded data
//===================================================================
void MapDrawer::draw_MeteoData_Gridded 
			( QPainter &pnt, Projection *proj,
			GriddedPlotter   *plotter )
{
	setUsedDataCenters.clear ();
	plotter->draw_CoveredZone (pnt, proj);
	
	Altitude mapAltitude =  colorMapData.getAltitude ();
	
	if (showWindArrows) {
		hasWindForArrows = true;
		if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D, mapAltitude))) {
			windArrowsAltitude = mapAltitude;
		}
		else if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,10))) {
			windArrowsAltitude = Altitude (LV_ABOV_GND,10);
		}
		else if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,3))) {
			windArrowsAltitude = Altitude (LV_ABOV_GND,3);
		}
		else if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,2))) {
			windArrowsAltitude = Altitude (LV_ABOV_GND,2);
		}
		else if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,1))) {
			windArrowsAltitude = Altitude (LV_ABOV_GND,1);
		}
		else if (plotter->hasData (DataCode(GRB_PRV_WIND_XY2D,LV_GND_SURF,0))) {
			windArrowsAltitude = Altitude (LV_GND_SURF,0);
		}
		else if (plotter->hasData (DataCode(GRB_WIND_SPEED,LV_ABOV_GND,10))
		 && plotter->hasData (DataCode(GRB_WIND_DIR,LV_ABOV_GND,10)) )
		{
			windArrowsAltitude = Altitude (LV_ABOV_GND,10);
		}
		else {
			hasWindForArrows = false;
		}
	}
	if (showWaveArrowsType != GRB_TYPE_NOT_DEFINED) {
		hasWaveForArrows = false;
		switch (showWaveArrowsType) {
		case GRB_PRV_WAV_SIG:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_DIR,LV_GND_SURF,0));
			break;
		case GRB_PRV_WAV_PRIM:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_DIR,LV_GND_SURF,0));
			break;
		case GRB_PRV_WAV_SCDY:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_SCDY_DIR,LV_GND_SURF,0));
			break;
		case GRB_PRV_WAV_MAX:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_MAX_DIR,LV_GND_SURF,0));
			break;
		case GRB_PRV_WAV_SWL:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_SWL_DIR,LV_GND_SURF,0));
			break;
		case GRB_PRV_WAV_WND:
			hasWaveForArrows = plotter->hasData(DataCode(GRB_WAV_WND_DIR,LV_GND_SURF,0));
			break;
		}
	}

	if (showCurrentArrows) {
		hasCurrentForArrows = true;
        if (plotter->hasData (DataCode(GRB_PRV_CUR_XY2D, LV_GND_SURF,0))) {
            currentArrowsAltitude = Altitude (LV_GND_SURF,0);
        } else if (plotter->hasData (DataCode(GRB_PRV_CUR_XY2D, LV_BLW_SURF,1))) {
            currentArrowsAltitude = Altitude (LV_BLW_SURF,1);
            colorMapData.levelType = LV_BLW_SURF;
            colorMapData.levelValue = 1;
        } else if (plotter->hasData (DataCode(GRB_PRV_CUR_XY2D, LV_BLW_SURF,2))) {
            currentArrowsAltitude = Altitude (LV_BLW_SURF,2);
            colorMapData.levelType = LV_BLW_SURF;
            colorMapData.levelValue = 2;
        } else if (plotter->hasData (DataCode(GRB_PRV_CUR_XY2D, LV_BLW_SURF,3))) {
            currentArrowsAltitude = Altitude (LV_BLW_SURF,3);
            colorMapData.levelType = LV_BLW_SURF;
            colorMapData.levelValue = 3;
        }
		else if (plotter->hasData (DataCode(GRB_CUR_SPEED, LV_GND_SURF,0))
		 && plotter->hasData (DataCode(GRB_CUR_DIR, LV_GND_SURF,0)) )
		{
			currentArrowsAltitude = Altitude (LV_GND_SURF,0);
		}
        else {
			hasCurrentForArrows = false;
		}
	}
	
	temperatureLabelsAlt = Altitude (LV_ABOV_GND,2);
	switch (colorMapData.dataType) 
	{
		case GRB_PRV_WIND_XY2D :
		case GRB_PRV_CUR_XY2D :
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
		case GRB_WAV_WHITCAP_PROB :
			windArrowsColor.setRgb(255, 255, 255);
			currentArrowsColor.setRgb(255, 255, 255);
			break;
		case GRB_TEMP :
		case GRB_TEMP_POT :
		case GRB_DEWPOINT :
			temperatureLabelsAlt = mapAltitude;	
			windArrowsColor.setRgb(255, 255, 255);
			currentArrowsColor.setRgb(255, 255, 255);
			break;
		case GRB_PRV_DIFF_TEMPDEW :
		case GRB_CLOUD_TOT :
		case GRB_HUMID_REL :
			windArrowsColor.setRgb(180, 180, 80);
			currentArrowsColor.setRgb(180, 180, 80);
			break;
		case GRB_CAPE :
			windArrowsColor.setRgb(100, 80, 80);
			currentArrowsColor.setRgb(100, 80, 80);
			break;
		case GRB_PRECIP_TOT :
        case GRB_PRECIP_RATE :
        case GRB_SNOW_DEPTH :
		case GRB_SNOW_CATEG :
		case GRB_FRZRAIN_CATEG :
			windArrowsColor.setRgb(140, 120, 100);
			currentArrowsColor.setRgb(140, 120, 100);
			break;
		default:
			windArrowsColor.setRgb(25, 25, 25);
			currentArrowsColor.setRgb(25, 25, 25);
			break;
	}
	
	//-------------------------------------------------------
	// draw complete colored map
	//-------------------------------------------------------
	plotter->draw_ColoredMapPlain (colorMapData, colorMapSmooth,pnt,proj);
	addUsedDataCenterModel (colorMapData, plotter);
	//-------------------------------------------------------

	std::vector <IsoLine *> listIsobars;
	std::vector <IsoLine *> listIsotherms0;
	std::vector <IsoLine *> listGeopotential;
	std::vector <IsoLine *> listIsotherms;
	std::vector <IsoLine *> listLinesThetaE;

	if (! plotter->hasData (GRB_PRESSURE_MSL,LV_MSL,0))
		showIsobars = false;
	if (! plotter->hasData (GRB_GEOPOT_HGT,LV_ISOTHERM0,0))
		showIsotherms0 = false;
	if (! plotter->hasData (geopotentialData))
		showGeopotential = false;
	if (! plotter->hasData (GRB_TEMP,isothermsAltitude))
		showIsotherms = false;
	if (! plotter->hasData (GRB_PRV_THETA_E,linesThetaEAltitude))
		showLinesThetaE = false;

	if (showIsobars) {
		pnt.setPen (isobarsPen);
		DataCode dtc (GRB_PRESSURE_MSL,LV_MSL,0);
		addUsedDataCenterModel (dtc, plotter);
		plotter->complete_listIsolines (&listIsobars, dtc,
						   84000, 112000, isobarsStep*100, proj);
        plotter->draw_listIsolines (listIsobars, pnt,proj);
	}

	if (showIsotherms0) {
		pnt.setPen (isotherms0Pen);
		DataCode dtc (GRB_GEOPOT_HGT,LV_ISOTHERM0,0);
		addUsedDataCenterModel (dtc, plotter);
		plotter->complete_listIsolines (&listIsotherms0, dtc,
						   0, 15000, isotherms0Step, proj);
        plotter->draw_listIsolines (listIsotherms0, pnt,proj);
	}

	if (showGeopotential) {
        pnt.setPen (geopotentialsPen);
        addUsedDataCenterModel(geopotentialData, plotter);
		plotter->complete_listIsolines (&listGeopotential,
						   geopotentialData,
						   geopotentialMin, geopotentialMax, geopotentialStep, proj);
        plotter->draw_listIsolines (listGeopotential, pnt,proj);
	}
	
	if (showIsotherms) {
		pnt.setPen (isotherms_Pen);
		DataCode dtc (GRB_TEMP,isothermsAltitude);
		addUsedDataCenterModel (dtc, plotter);
		plotter->complete_listIsolines (&listIsotherms, dtc,
						   -140+273.15, 80+273.15, isotherms_Step, proj);
        plotter->draw_listIsolines (listIsotherms, pnt,proj);
	}
	
	if (showLinesThetaE) {
		pnt.setPen (linesThetaE_Pen);
		DataCode dtc (GRB_PRV_THETA_E,linesThetaEAltitude);
		addUsedDataCenterModel (dtc, plotter);
		plotter->complete_listIsolines (&listLinesThetaE, dtc,
						   -80+273.15, 140+273.15, linesThetaE_Step, proj);
        plotter->draw_listIsolines (listLinesThetaE, pnt,proj);
	}

	if (showWaveArrowsType != GRB_TYPE_NOT_DEFINED && hasWaveForArrows) {
		plotter->draw_WAVES_Arrows (showWaveArrowsType, pnt, proj);
	}
	
	if (showWindArrows && hasWindForArrows) {
		plotter->draw_WIND_Arrows (windArrowsAltitude, showBarbules, windArrowsColor, pnt, proj);
	}
	if (showCurrentArrows && hasCurrentForArrows) {
		plotter->draw_CURRENT_Arrows (currentArrowsAltitude, currentArrowsColor, pnt, proj);
	}

	if (showIsobarsLabels && showIsobars) {
		QColor color (40,40,40);
        plotter->draw_listIsolines_labels (listIsobars, 0.01,0, color, pnt,proj);
	}
	if (showIsotherms0Labels && showIsotherms0) {
		QColor color(200,80,80);
		DataCode dtc (GRB_GEOPOT_HGT,LV_ISOTHERM0,0);
		addUsedDataCenterModel (dtc, plotter);
		double coef = Util::getDataCoef (dtc);
        plotter->draw_listIsolines_labels (listIsotherms0, coef,0, color, pnt,proj);
	}
	if (showGeopotentialLabels && showGeopotential) {
        QColor color(200,80,80);
		addUsedDataCenterModel (geopotentialData, plotter);
		double coef = Util::getDataCoef (geopotentialData);
        plotter->draw_listIsolines_labels (listGeopotential, coef,0, color, pnt,proj);
	}
	if (showIsotherms_Labels && showIsotherms) {
		QColor color(40,40,150); 
        plotter->draw_listIsolines_labels (listIsotherms,
										1.,-273.15,
										color, pnt,proj, 
										16	// TODO: labels density
										);
	} 
	if (showLinesThetaE_Labels && showLinesThetaE) {
		QColor color(40,40,150); 
        plotter->draw_listIsolines_labels (listLinesThetaE,
										1.,-273.15,
										color, pnt,proj, 
										16	// TODO: labels density
										);
	} 

	if (showPressureMinMax) {
		DataCode dtc (GRB_PRESSURE_MSL,LV_MSL,0);
		addUsedDataCenterModel (dtc, plotter);
		plotter->draw_DATA_MinMax ( 
						dtc, 101200, "L", "H",
						Font::getFont(FONT_GRIB_PressHL),
						QColor(0,0,0), pnt, proj);
	}
	if (showTemperatureLabels ) {
		DataCode dtc (GRB_TEMP,temperatureLabelsAlt);
		if (plotter->hasData (dtc)) {
			addUsedDataCenterModel (dtc, plotter);
			plotter->draw_DATA_Labels (
					dtc, Font::getFont(FONT_GRIB_Temp),
					QColor(0,0,0),
					Util::formatTemperature_short, pnt, proj);
		}
	}

	//===================================================
	// Grille
	//===================================================
	if (showGribGrid) {
		pnt.setPen(QColor (40,40,40));
		plotter->draw_GridPoints (colorMapData, pnt, proj);
	}

	// cleanup
	for (auto const &it :listIsobars) { delete it;}
	for (auto const &it :listIsotherms0) { delete it;}
	for (auto const &it :listGeopotential) { delete it;}
	for (auto const &it :listIsotherms) { delete it;}
	for (auto const &it :listLinesThetaE) { delete it;}
}
//-------------------------------------------------------------
// Cartouche : dates de la prévision courante + infos générales
//-------------------------------------------------------------
void MapDrawer::draw_Cartouche_Gridded 
		(QPainter &pnt, const Projection *proj, GriddedPlotter *plotter)
{
	GriddedReader *reader = plotter->getReader();
    if (reader == nullptr) {
        return;
    }
	//------------------------------------------
	QString origine, duplicated;
	DataCode dtmp = colorMapData;
	dtmp.dataType = reader->getDataTypeAlias(dtmp.dataType);

	GriddedRecord *rec = reader->getRecord (dtmp, plotter->getCurrentDate());
	if (rec != nullptr) {
		origine = DataCodeStr::toString (rec->getDataCenterModel());
		duplicated = rec->isDuplicated() ? " (dup)" : "";
	}
	//------------------------------------------
    if (reader->getNumberOfDates() > 0)
    {
        QString  tval = "Val: "+Util::formatDateTimeLong (plotter->getCurrentDate());
		QFont fontbig   = Font::getFont (FONT_MapInfo_Big);
		QFont fontsmall  = Font::getFont (FONT_MapInfo_Medium);
		QFont fontdate = Font::getFont (FONT_MapInfo_DateRef);
		QFont fontsign = Font::getFont (FONT_MapInfo_Small);
		QFontMetrics fmbig   (fontbig);
		QFontMetrics fmsmall (fontsmall);
		QFontMetrics fmsign  (fontsign);
		QFontMetrics fmdate  (fontdate);
        int n = 1;
				
		//---------------------------------------------------------
		// Top left : Data
		//---------------------------------------------------------
        QStringList datalist;
		if (origine != "")
			datalist.append (tr("Data: ") + origine + duplicated);
		
		if (colorMapData.dataType != GRB_TYPE_NOT_DEFINED)
			datalist.append (DataCodeStr::toString_levelShort (colorMapData));
		
		if (showIsobars)
			datalist.append (tr("Isobars MSL (hPa)"));
		if (showIsotherms0)
			datalist.append (tr("Isotherms 0°C") 
				+" (" 
				+Util::getDataUnit(DataCode(GRB_GEOPOT_HGT,LV_ISOTHERM0,0)) +")");
		if (showIsotherms)
			datalist.append (tr("Isotherms")+" "+AltitudeStr::toStringShort(isothermsAltitude)+" "+tr("(°C)"));
		if (showLinesThetaE)
			datalist.append (tr("Theta-e")+" "+AltitudeStr::toStringShort(linesThetaEAltitude)+" "+tr("(°C)"));
		if (showTemperatureLabels) {
			DataCode dtc (GRB_TEMP,temperatureLabelsAlt);
			if (plotter->hasData (dtc)) {
				datalist.append (tr("Temperature label")
					+" ("+AltitudeStr::toStringShort(temperatureLabelsAlt)+")");
			}
		}
		if (showGeopotential)
			datalist.append (tr("Geopotential")
					+" "+AltitudeStr::toStringShort(geopotentialData.getAltitude())
					+" (" 
					+Util::getDataUnit(DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,0)) 
					+")");
		if (showWindArrows && hasWindForArrows)
			datalist.append (tr("Wind arrows")
					+" ("+AltitudeStr::toStringShort(windArrowsAltitude)+")");
		if (showCurrentArrows && hasCurrentForArrows)
			datalist.append (tr("Current arrows"));
		if (showWaveArrowsType != GRB_TYPE_NOT_DEFINED && hasWaveForArrows) {
			switch (showWaveArrowsType) {
			case GRB_PRV_WAV_SIG:
				datalist.append (tr("Combined wind and swell wave arrows"));
				break;
			case GRB_PRV_WAV_PRIM:
				datalist.append (tr("Primary wave arrows"));
				break;
			case GRB_PRV_WAV_SCDY:
				datalist.append (tr("Secondary wave arrows"));
				break;
			case GRB_PRV_WAV_MAX:
				datalist.append (tr("Maximum wave arrows"));
				break;
			case GRB_PRV_WAV_SWL:
				datalist.append (tr("Swell arrows"));
				break;
			case GRB_PRV_WAV_WND:
				datalist.append (tr("Wind wave arrows"));
				break;
			}
		}
        int dy, w, h, x, y, w1, w2;
		QColor   transpcolor(255,255,255,180);
		QColor   textcolor(20,20,20,255);

		// use a gradient, because it's a bug sometimes with solid pattern (black background)
		QLinearGradient gradient;
		gradient.setColorAt(0, transpcolor);
		gradient.setColorAt(1, transpcolor);
		pnt.setBrush(gradient);
		//---------------------------------------------------------
		// forecast validity date (top left corner) + data type
		//---------------------------------------------------------
        QString  tval2 = Util::formatDateTimeLong(plotter->getCurrentDate());
		n +=  datalist.size();
		w1 = fmbig.width(tval2)+5;
		w2 = 0;
		// get largest string data
		for (int i = 0; i < datalist.size(); i++) {
        	int t = fmsmall.width (datalist.at(i))+5;
			if (t>w2)
				w2 = t;
		}
        w  = (w1>w2) ? w1 : w2;
		dy = fontbig.pointSize()+1;
        h = n*dy+8;
		// A. Degwerth [Cassidian] added support of Arabic language
        x = QApplication::isRightToLeft() ? proj->getW() - w : 0;
        y = 0;
        pnt.setPen(transpcolor);
        pnt.drawRect(x,y,w,h);
        pnt.setPen(textcolor);
        pnt.setFont(fontbig);
        pnt.drawText(x+3, y+dy+2, tval2);// forecast validity date
		// show other data drawed in map
        pnt.setFont(fontsmall);
		for (int i = 0; i < datalist.size(); i++) {
        	pnt.drawText(x+3, y+(i+2)*dy+4, datalist.at(i));
		}
		//--------------------------------------------------
        // Reference dates (bottom left)
		//--------------------------------------------------
        datalist.clear ();
		time_t  tref;
		for (auto dcm : setUsedDataCenters) {
				tref = reader->getRefDateForDataCenter (dcm);
			// DBGN(tref);
			if (tref != 0) {
				QString stref = "Ref ";
				if (dcm != OTHER_DATA_CENTER)
					stref += DataCodeStr::toString(dcm);
				stref += ": "+Util::formatDateTimeLong (tref);
				datalist.append (stref);
			}
		}
		if (!datalist.empty()) {
			pnt.setFont (fontdate);
			w = 0;  // get largest string data
			for (int i = 0; i < datalist.size(); i++) {
				int t = fmdate.width (datalist.at(i))+5;
				if (t>w)
					w = t;
			}
			n = datalist.size ();
			dy = fontdate.pointSize()+2;
			h = n*dy+2;
			// A. Degwerth [Cassidian] added support of Arabic language
			x = QApplication::isRightToLeft() ? proj->getW() - w : 0;
			y = proj->getH() - h;
			pnt.setPen(transpcolor);
			pnt.drawRect(x,y,w,h);
			pnt.setPen(textcolor);
			for (int i = 0; i < datalist.size(); i++) {
				pnt.drawText(x+3, y+(i+1)*dy+2, datalist.at(i));
			}
		}
		//--------------------------------------------------
        // Signature XyGrib (coin bas droite)
		//--------------------------------------------------
        QString sign = tr("XyGrib");
        pnt.setFont(fontsign);
		w = fmsign.width(sign)+5;
		dy = fontsign.pointSize()+1;
        h = dy+2;
        x = QApplication::isRightToLeft() ? 0 : proj->getW() - w;
        y = proj->getH() - h;
        pnt.setPen(transpcolor);
        pnt.drawRect(x,y,w,h);
        pnt.setPen(textcolor);
        pnt.drawText(x+3, y+dy, sign);
	}
}


//===========================================================
QPixmap * MapDrawer::createPixmap_GriddedData ( 
						time_t date, 
						bool isEarthMapValid, 
						GriddedPlotter *plotter,
						Projection *proj,
						const QList<POI*>& lspois )
{
	QPixmap *pixmap = new QPixmap(proj->getW(), proj->getH());

	QPainter pnt;
	pnt.begin (pixmap);
	
		if (plotter) {
			plotter->setCurrentDate (date);
			this->draw_GSHHS_and_GriddedData (pnt, true, isEarthMapValid, proj, plotter);
		}
		else {
			this->draw_GSHHS (pnt, true, isEarthMapValid, proj);
		}
		// Ajoute les pOIs visibles
		for (auto poi : lspois) {
            if (poi->isVisible()) {
				poi->drawContent (pnt, proj, true);
			}
		}
	pnt.end();
	return pixmap;
}

