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
#include <QFile>

#include "MenuBar.h"
#include "Font.h"
#include "Util.h"
#include "Projection.h"

//===================================================================================
void ZeroOneActionGroup::addAction (QAction *act)
{
	lsactions.append(act);
	connect(act, SIGNAL(triggered(bool)),
            	this,  SLOT(slot_actionTrigerred(bool)));
}
//------------------------------------------------------------------
void ZeroOneActionGroup::slot_actionTrigerred(bool b)
{
	QAction *act = (QAction *) sender();
	setCheckedAction (act, b, true);
}
//------------------------------------------------------------------
void ZeroOneActionGroup::setCheckedAction (QAction *act, bool val, bool emitSignal)
{
	for (int i=0; i<lsactions.size(); i++) {
		if (lsactions.at(i)== act) {
			lsactions.at(i)->setChecked (val);
		}
		else {
			lsactions.at(i)->setChecked (false);
		}
	}
	if (emitSignal) {
		if (val)
			emit triggered (act);
		else
			emit triggered (NULL);
	}
}

//===================================================================================
MenuBar::MenuBar (QWidget *parent, bool withmblue)
    : QMenuBar (parent)
{
#if defined (Q_OS_UNIX)
	bool native = Util::getSetting("systemNativeMenuBar", false).toBool();
	setNativeMenuBar (native);    // bug with some versions of ubuntu 
#endif

	//======================================================================
    menuFile = new QMenu(tr("File"));
    //======================================================================
        acFile_Open = addAction (menuFile,
        			tr("Open"), tr("Ctrl+O"),
                    tr("Open a GRIB file"), Util::pathImg("fileopen.png"));
        acFile_Close = addAction (menuFile,
        			tr("Close"), tr("Ctrl+W"),
                    tr("Close"), Util::pathImg("fileclose.png"));
        acFile_NewInstance = addAction (menuFile,
        			tr("New instance"), tr("Ctrl+Shift+N"),
                    tr("Open a new zyGrib instance"), "");
        menuFile->addSeparator();
        acFile_Load_GRIB = addAction (menuFile,
        			tr("Download GRIB"), tr("Ctrl+D"),
                    tr("Download"), Util::pathImg("network.png"));
        acFile_GribServerStatus = addAction (menuFile,
        			tr("GRIB server status"), tr("Ctrl+R"),
                    tr("GRIB file server status"), Util::pathImg("connect_no.png"));

        acFile_Load_IAC = addAction (menuFile,
        	tr("IAC fleetcode NOAA"), tr("Ctrl+L"),
			tr("Download current IAC file (fleetcode) from NOAA - Analyse or Forecast +24h - Europe"), "");

		QMenu *menutmp = new QMenu (tr("Meteoblue"));
			acMBlue_fastInterpolation = addActionCheck (menutmp,
					tr("Fast interpolation"), tr(""),
					tr("Use a faster but a little less accurate interpolation"), "");
			acMBlueSwiss_Load = addAction (menutmp,
					tr("Load Meteoblue file : Swiss"), tr("Ctrl+B"),
					tr("Download Meteoblue file (Swiss)"), Util::pathImg("meteoblue.png"));
			acMBlueSwiss_ShowArea = addActionCheck (menutmp, 
					tr("Show area : Swiss"), "",
					tr("Show Meteoblue area (Swiss)"));
		if (withmblue)
			menuFile->addMenu (menutmp);
			
        menuFile->addSeparator();
        acFile_Info_GRIB = addAction (menuFile,
        			tr("File information"), tr("Ctrl+I"),
                    tr("GRIB file information"), Util::pathImg("info.png"));
        menuFile->addSeparator();
        ac_CreateAnimation = addAction (menuFile,
        			tr("Create animation"), tr("Ctrl+A"),
                    tr("Create animation with GRIB data"), Util::pathImg("anim.png"));
        ac_ExportImage = addAction (menuFile,
        			tr("Save current image"), tr("Ctrl+S"),"",Util::pathImg("media-floppy.png"));
        menuFile->addSeparator();
        acFile_Quit = addAction (menuFile,
                    tr("Quit"), tr("Ctrl+Q"),
                    tr("Bye"), Util::pathImg("exit.png"));
				
    //======================================================================
    menuColorMap = new QMenu(tr("Weather map"));
    //======================================================================
		acView_GroupColorMap = new ZeroOneActionGroup(menuColorMap);
			acView_WindColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Wind"), "", "");
			acView_RainColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Precipitation"),"","");
			acView_CloudColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Cloud cover"), "","");
			acView_HumidColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Relative humidity"),"","");
			acView_TempColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Temperature"),"","");
			acView_DeltaDewpointColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Gap temperature-dew point"), "", "");
			acView_SnowCateg = addGroup (acView_GroupColorMap, menuColorMap, tr("Snow (snowfall possible)"), "", "");
			acView_SnowDepth = addGroup (acView_GroupColorMap, menuColorMap, tr("Snow (depth)"), "", "");
			acView_FrzRainCateg = addGroup (acView_GroupColorMap, menuColorMap, tr("Frozen rain (rainfall possible)"), "", "");
			acView_CAPEsfc = addGroup (acView_GroupColorMap, menuColorMap, tr("CAPE"), "", "");
			acView_CINsfc = addGroup (acView_GroupColorMap, menuColorMap, tr("CIN"), "", "");
			acView_ThetaEColors = addGroup (acView_GroupColorMap, menuColorMap, tr("Theta-e"), "", tr("Equivalent potential temperature"));
        //--------------------------------
        menuColorMap->addSeparator();
        acView_WindArrow = addActionCheck (menuColorMap, tr("Wind arrows"), tr("Ctrl+J"),
                    tr("Show wind arrows"));
        acView_TemperatureLabels = addActionCheck (menuColorMap,
        			tr("Temperature")+" (2m)", tr("Ctrl+T"),
                    "");
        //--------------------------------
        menuColorMap->addSeparator();
		menuViewOptions = new QMenu (tr("Options"));
        menuColorMap->addMenu (menuViewOptions);
        //--------------------------------
        menuViewOptions->addSeparator();
        acView_DuplicateFirstCumulativeRecord = addActionCheck (menuViewOptions,
        			tr("NOAA-GFS")+": "+tr("Duplicate first cumulative record"), "", "");
        acView_InterpolateValues = addActionCheck (menuViewOptions,
        			tr("Numerical data interpolation"), "", "");
        acView_ColorMapSmooth = addActionCheck (menuViewOptions,
        			tr("Smooth colors"), tr("Ctrl+F"), "");
        //--------------------------------
        menuViewOptions->addSeparator();
        acView_Barbules = addActionCheck (menuViewOptions, tr("Wind barbs"), "",
                    tr("Show barbs on wind arrows"));
        acView_ThinWindArrows = addActionCheck (menuViewOptions, tr("Thin wind arrows"), "","");
        acView_WindArrowsOnGribGrid = addActionCheck (menuViewOptions,
        			tr("Wind arrows on Grib grid"), "", "");
        acView_useJetSTreamColorMap = addActionCheck (menuViewOptions,
        			tr("Jet stream colors"), tr("Ctrl+Shift+J"), "");
		
        menuViewOptions->addSeparator();
        acView_GribGrid = addActionCheck (menuViewOptions, tr("Show Grib grid"), 
					tr("Ctrl+X"), tr("Show GRIB grid"));
        acView_ShowColorScale = addActionCheck (menuViewOptions, tr("Show color scale"),
					tr("Ctrl+Y"), tr("Show color scale"));
    //======================================================================
	menuAltitude = new QMenu (tr("Altitude"));
    //======================================================================
		acAlt_GroupAltitude = new QActionGroup (menuAltitude);
			acAlt_MSL = addGroup (acAlt_GroupAltitude, menuAltitude, tr("Sea level"), "", "");
			acAlt_GND = addGroup (acAlt_GroupAltitude, menuAltitude, tr("Surface"), "", "");
			acAlt_sigma995 = addGroup (acAlt_GroupAltitude, menuAltitude, tr("Sigma 995"), "", "");
			acAlt_GND_1m = addGroup (acAlt_GroupAltitude, menuAltitude, tr("1 m above ground"), "", "");
			acAlt_GND_2m = addGroup (acAlt_GroupAltitude, menuAltitude, tr("2 m above ground"), "", "");
			acAlt_GND_3m = addGroup (acAlt_GroupAltitude, menuAltitude, tr("3 m above ground"), "", "");
			acAlt_GND_10m = addGroup (acAlt_GroupAltitude, menuAltitude, tr("10 m above ground"), "", "");
			acAlt_925hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("925 hPa (≈ 760 m)"), "", "");
			acAlt_850hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("850 hPa (≈ 1460 m)"), "", "");
			acAlt_700hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("700 hPa (≈ 3000 m)"), "", "");
			acAlt_600hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("600 hPa (≈ 4200 m)"), "", "");
			acAlt_500hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("500 hPa (≈ 5600 m)"), "", "");
			acAlt_400hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("400 hPa (≈ 7200 m)"), "", "");
			acAlt_300hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("300 hPa (≈ 9200 m)"), "", "");
			acAlt_200hpa = addGroup (acAlt_GroupAltitude, menuAltitude, tr("200 hPa (≈ 11800 m)"), "", "");
			acAlt_Atmosphere = addGroup (acAlt_GroupAltitude, menuAltitude, tr("Atmosphere"), "", "");
    //======================================================================
    menuIsolines = new QMenu (tr("Isolines"));
    //======================================================================
		acView_Isobars = addActionCheck (menuIsolines, tr("Isobars (MSL)"), "",
						"");
		menuIsobarsStep = new QMenu(tr("Isobars spacing (hPa)"));
		menuIsolines->addMenu(menuIsobarsStep);
		acView_GroupIsobarsStep = new QActionGroup(menuIsobarsStep);
			acView_IsobarsStep1 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("1"), "", "");
			acView_IsobarsStep2 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("2"), "", "");
			acView_IsobarsStep3 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("3"), "", "");
			acView_IsobarsStep4 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("4"), "", "");
			acView_IsobarsStep5 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("5"), "", "");
			acView_IsobarsStep6 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("6"), "", "");
			acView_IsobarsStep8 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("8"), "", "");
			acView_IsobarsStep10 = addGroup (acView_GroupIsobarsStep, menuIsobarsStep, tr("10"), "", "");
        acView_IsobarsLabels = addActionCheck (menuIsolines, tr("Isobars labels"), "", "");
        acView_PressureMinMax = addActionCheck (menuIsolines, tr("Pressure Low High"), "",
                            tr("Show Low (L) and Hight (H) pressure points"));
        //--------------------------------
        menuIsolines->addSeparator();
		acView_Isotherms0 = addActionCheck (menuIsolines, tr("Isotherms 0°C"), "","");
		menuIsotherms0Step = new QMenu(tr("Isotherms 0°C spacing (m)"));
		acView_GroupIsotherms0Step    = new QActionGroup(menuIsotherms0Step);
		menuIsolines->addMenu(menuIsotherms0Step);
			acView_Isotherms0Step10   = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("10"), "", "");
			acView_Isotherms0Step20   = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("20"), "", "");
			acView_Isotherms0Step50   = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("50"), "", "");
			acView_Isotherms0Step100  = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("100"), "", "");
			acView_Isotherms0Step200  = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("200"), "", "");
			acView_Isotherms0Step500  = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("500"), "", "");
			acView_Isotherms0Step1000 = addGroup (acView_GroupIsotherms0Step, menuIsotherms0Step, tr("1000"), "","");
        acView_Isotherms0Labels = addActionCheck (menuIsolines, tr("Isotherms 0°C labels"), "", "");
        //--------------------------------
        menuIsolines->addSeparator();
		menuIsotherms = new QMenu (tr("Isotherms (altitude)"));
		menuIsolines->addMenu (menuIsotherms);
		groupIsotherms = new ZeroOneActionGroup (menuIsotherms);
			acView_Isotherms_2m = addGroup (groupIsotherms, menuIsotherms, tr("2 m"), "", "");
			acView_Isotherms_925hpa = addGroup (groupIsotherms, menuIsotherms, tr("925 hpa"), "", "");
			acView_Isotherms_850hpa = addGroup (groupIsotherms, menuIsotherms, tr("850 hpa"), "", "");
			acView_Isotherms_700hpa = addGroup (groupIsotherms, menuIsotherms, tr("700 hpa"), "", "");
			acView_Isotherms_600hpa = addGroup (groupIsotherms, menuIsotherms, tr("600 hpa"), "", "");
			acView_Isotherms_500hpa = addGroup (groupIsotherms, menuIsotherms, tr("500 hpa"), "", "");
			acView_Isotherms_400hpa = addGroup (groupIsotherms, menuIsotherms, tr("400 hpa"), "", "");
			acView_Isotherms_300hpa = addGroup (groupIsotherms, menuIsotherms, tr("300 hpa"), "", "");
			acView_Isotherms_200hpa = addGroup (groupIsotherms, menuIsotherms, tr("200 hpa"), "", "");
		menuIsotherms_Step = new QMenu(tr("Isotherms spacing (°C)"));
		groupIsotherms_Step = new QActionGroup(menuIsotherms_Step);
			acView_Isotherms_Step1 = addGroup (groupIsotherms_Step, menuIsotherms_Step, tr("1"), "", "");
			acView_Isotherms_Step2 = addGroup (groupIsotherms_Step, menuIsotherms_Step, tr("2"), "", "");
			acView_Isotherms_Step5 = addGroup (groupIsotherms_Step, menuIsotherms_Step, tr("5"), "", "");
			acView_Isotherms_Step10 = addGroup (groupIsotherms_Step, menuIsotherms_Step, tr("10"), "", "");
		menuIsolines->addMenu (menuIsotherms_Step);
        acView_Isotherms_Labels = addActionCheck (menuIsolines, tr("Isotherms labels"), "",  "");
        //--------------------------------
        menuIsolines->addSeparator();
		menutmp = new QMenu (tr("Geopotential altitude"));
		menuIsolines->addMenu (menutmp);
		acAlt_GroupGeopotLine = new ZeroOneActionGroup (menutmp);
			acAlt_GeopotLine_925hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("925 hpa"), "", "");
			acAlt_GeopotLine_850hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("850 hpa"), "", "");
			acAlt_GeopotLine_700hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("700 hpa"), "", "");
			acAlt_GeopotLine_600hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("600 hpa"), "", "");
			acAlt_GeopotLine_500hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("500 hpa"), "", "");
			acAlt_GeopotLine_400hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("400 hpa"), "", "");
			acAlt_GeopotLine_300hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("300 hpa"), "", "");
			acAlt_GeopotLine_200hpa = addGroup (acAlt_GroupGeopotLine, menutmp, tr("200 hpa"), "", "");
			menuGeopotStep = new QMenu(tr("Geopotentials spacing (m)"));
            acAlt_GroupGeopotStep = new QActionGroup (menuGeopotStep);
                acAlt_GeopotStep_1  = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("1"), "", "");
                acAlt_GeopotStep_2  = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("2"), "", "");
                acAlt_GeopotStep_5  = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("5"), "", "");
                acAlt_GeopotStep_10 = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("10"), "", "");
                acAlt_GeopotStep_20 = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("20"), "", "");
                acAlt_GeopotStep_50 = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("50"), "", "");
                acAlt_GeopotStep_100 = addGroup (acAlt_GroupGeopotStep, menuGeopotStep, tr("100"), "", "");
            menuIsolines->addMenu(menuGeopotStep);
			acAlt_GeopotLabels = addActionCheck (menuIsolines, tr("Geopotentials labels"), "","");
        //--------------------------------
        menuIsolines->addSeparator();
		menuLinesThetaE = new QMenu (tr("Theta-e (altitude)"));
		menuIsolines->addMenu (menuLinesThetaE);
		groupLinesThetaE = new ZeroOneActionGroup (menuLinesThetaE);
			acView_LinesThetaE_925hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("925 hpa"), "", "");
			acView_LinesThetaE_850hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("850 hpa"), "", "");
			acView_LinesThetaE_700hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("700 hpa"), "", "");
			acView_LinesThetaE_600hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("600 hpa"), "", "");
			acView_LinesThetaE_500hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("500 hpa"), "", "");
			acView_LinesThetaE_400hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("400 hpa"), "", "");
			acView_LinesThetaE_300hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("300 hpa"), "", "");
			acView_LinesThetaE_200hpa = addGroup (groupLinesThetaE, menuLinesThetaE, tr("200 hpa"), "", "");
		menuLinesThetaE_Step = new QMenu(tr("Theta-e spacing (°C)"));
		groupLinesThetaE_Step = new QActionGroup(menuLinesThetaE_Step);
			acView_LinesThetaE_Step1 = addGroup (groupLinesThetaE_Step, menuLinesThetaE_Step, tr("1"), "", "");
			acView_LinesThetaE_Step2 = addGroup (groupLinesThetaE_Step, menuLinesThetaE_Step, tr("2"), "", "");
			acView_LinesThetaE_Step5 = addGroup (groupLinesThetaE_Step, menuLinesThetaE_Step, tr("5"), "", "");
			acView_LinesThetaE_Step10 = addGroup (groupLinesThetaE_Step, menuLinesThetaE_Step, tr("10"), "", "");
		menuIsolines->addMenu (menuLinesThetaE_Step);
        acView_LinesThetaE_Labels = addActionCheck (menuIsolines, tr("Theta-e labels"), "",  "");
		
    //======================================================================
    menuMap = new QMenu(tr("Earth"));
    //======================================================================
		QMenu *menuQuality = new QMenu(tr("Map quality"));
        acMap_GroupQuality = new QActionGroup(menuQuality);
            acMap_Quality1 = addGroup (acMap_GroupQuality, menuQuality, tr("Resolution 1 (25 km)"), "", tr("Niveau de détail de la carte"));
            acMap_Quality2 = addGroup (acMap_GroupQuality, menuQuality, tr("Resolution 2 (5 km)"), "", tr("Niveau de détail de la carte"));
            acMap_Quality3 = addGroup (acMap_GroupQuality, menuQuality, tr("Resolution 3 (1 km)"), "", tr("Niveau de détail de la carte"));
            acMap_Quality4 = addGroup (acMap_GroupQuality, menuQuality, tr("Resolution 4 (200 m)"), "", tr("Niveau de détail de la carte"));
            acMap_Quality5 = addGroup (acMap_GroupQuality, menuQuality, tr("Resolution 5 (100 m)"), "", tr("Niveau de détail de la carte"));
		menuMap->addMenu(menuQuality);

		QMenu *menuProjection = new QMenu(tr("Projection"));
        acMap_GroupProjection = new QActionGroup(menuProjection);
            acMap_PROJ_ZYGRIB = addGroup (acMap_GroupProjection, menuProjection, tr("ZyGrib"), "", "");
            acMap_PROJ_MERCATOR = addGroup (acMap_GroupProjection, menuProjection, tr("Mercator"), "", "");
            acMap_PROJ_MILLER = addGroup (acMap_GroupProjection, menuProjection, tr("Miller"), "", "");
            acMap_PROJ_CENTRAL_CYL = addGroup (acMap_GroupProjection, menuProjection, tr("Central Cylindric"), "", "");
            acMap_PROJ_EQU_CYL = addGroup (acMap_GroupProjection, menuProjection, tr("Equal cylindric"), "", "");
		menuMap->addMenu(menuProjection);

	menuMap->addSeparator();
        acMap_Orthodromie = addActionCheck (menuMap, tr("Great circle distance"), "", "");
        acMap_LonLatGrid = addActionCheck (menuMap, tr("Longitudes-latitudes grid"), "", "");
        acMap_AutoZoomOnGribArea = addActionCheck (menuMap, tr("Auto zoom on grib area"), "", 
										tr("Automatic zoom on grib area after file loading"));

     menuMap->addSeparator();
        acMap_CountriesBorders = addActionCheck (menuMap, tr("Boundaries"), "", tr("Show boundaries"));
        acMap_Rivers = addActionCheck (menuMap, tr("Rivers"), "", tr("Show rivers"));
        acMap_CountriesNames = addActionCheck (menuMap, tr("Countries names"), "", tr("Display countries names"));

        QMenu *menuCitiesNames = new QMenu(tr("Cities names"));
        acMap_GroupCitiesNames = new QActionGroup(menuMap);
            acMap_CitiesNames0 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("None"), "", "");
            acMap_CitiesNames1 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("Level 1")+" (>= 1000000)", "", "");
            acMap_CitiesNames2 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("Level 2")+" (>= 100000)", "", "");
            acMap_CitiesNames3 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("Level 3")+" (>= 10000)", "", "");
            acMap_CitiesNames4 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("Level 4")+" (>= 1000)", "", "");
            acMap_CitiesNames5 = addGroup (acMap_GroupCitiesNames, menuCitiesNames, tr("Level 5")+" (>= 0)", "", "");
            menuMap->addMenu(menuCitiesNames);
        acMap_FindCity = addAction (menuMap, tr("Find a city..."), "", "");
        acMap_ShowPOIs = addActionCheck (menuMap, tr("Point of interest"), tr("Ctrl+N"), tr("Display Points of interest"));

        acMap_ShowMETARs = addActionCheck (menuMap, tr("METAR: show stations"), tr("Ctrl+K"), tr("Display METAR stations"));
        acMap_SelectMETARs = addAction (menuMap, tr("METAR: select stations"), tr("Ctrl+Shift+K"), tr("Select METAR stations"));

        menuMap->addSeparator();
        acMap_Zoom_In = addAction (menuMap,
        				tr("Increase map scale"), tr("+"),
                        tr("Increase map scale"), Util::pathImg("viewmag+.png"));
        acMap_Zoom_Out = addAction (menuMap,
        				tr("Reduce map scale"), tr("-"),
                        tr("Reduce map scale"), Util::pathImg("viewmag-.png"));
        acMap_Zoom_Sel = addAction (menuMap,
                        tr("Zoom (selected zone or Grib file)"), tr("Ctrl+Z"),
                        tr("Zoom on the selected zone or on the Grib file area"),
                        Util::pathImg("viewmagfit.png"));
        acMap_Zoom_All = addAction (menuMap,
        				tr("Show whole map"), tr("Ctrl+M"),
                        tr("Show whole map"), Util::pathImg("viewmag1.png"));
        menuMap->addSeparator();
        acMap_Go_Left = addAction (menuMap,
        				tr("Left"), tr("LEFT"),
                        tr("Move"), Util::pathImg("back.png"));
        acMap_Go_Right = addAction (menuMap,
        				tr("Right"), tr("RIGHT"),
                        tr("Move"), Util::pathImg("forward.png"));
        acMap_Go_Up   = addAction (menuMap,
        				tr("Top"), tr("UP"),
                        tr("Move"), Util::pathImg("up.png"));
        acMap_Go_Down = addAction (menuMap,
        				tr("Down"), tr("DOWN"),
                        tr("Move"), Util::pathImg("down.png"));
	
    //======================================================================
    menuSeaState = new QMenu(tr("Sea"));
    //======================================================================
		// Waves
		acView_SigWaveHeight = addActionCheck (menuSeaState, tr("Significant wave height"), "", "");
		acView_MaxWaveHeight = addActionCheck (menuSeaState, tr("Maximum wave height"), "", "");
		acView_WhiteCapProb = addActionCheck (menuSeaState, tr("Whitecap probability"), "", "");
		menuWavesArrows = new QMenu (tr("Waves arrows"));
			acView_WavesArrows_none = addActionCheck (menuWavesArrows, tr("None"), "", "");
			acView_WavesArrows_max = addActionCheck (menuWavesArrows, tr("Maximum wave"), "", "");
			acView_WavesArrows_swell = addActionCheck (menuWavesArrows, tr("Swell"), "", "");
			acView_WavesArrows_wind = addActionCheck (menuWavesArrows, tr("Wind wave"), "", "");
			acView_WavesArrows_prim = addActionCheck (menuWavesArrows, tr("Primary wave"), "", "");
			acView_WavesArrows_scdy = addActionCheck (menuWavesArrows, tr("Secondary wave"), "", "");
		menuSeaState->addMenu (menuWavesArrows);
        menuSeaState->addSeparator();
        acView_DuplicateMissingWaveRecords = addActionCheck (menuSeaState,
        			tr("FNMOC-WW3")+": "+tr("Duplicate missing wave records"), "", "");
		// Currents
        menuSeaState->addSeparator();
		acView_CurrentColors = addActionCheck (menuSeaState, tr("Current map"), "", "");
        acView_CurrentArrow = addActionCheck (menuSeaState, tr("Current arrows"), "",
									tr("Show current arrows"));
        acView_CurrentArrowsOnGribGrid = addActionCheck (menuSeaState,
									tr("Current arrows on Grib grid"), "", "");
		// Waves arrows type
		acView_GroupWavesArrows = new QActionGroup (menuWavesArrows);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_none);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_max);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_swell);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_wind);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_prim);
			acView_GroupWavesArrows->addAction (acView_WavesArrows_scdy);
		// Actions inserted in the color maps group
		acView_GroupColorMap->addAction (acView_CurrentColors);
		acView_GroupColorMap->addAction (acView_SigWaveHeight);
		acView_GroupColorMap->addAction (acView_MaxWaveHeight);
		acView_GroupColorMap->addAction (acView_WhiteCapProb);
    //======================================================================
    menuOptions = new QMenu(tr("Options"));
    //======================================================================
        acOptions_Proxy = addAction (menuOptions, tr("Internet parameters"),"","","");
        acView_ShowBoardPanel = addActionCheck (menuOptions, tr("Show values panel"),
					tr("Ctrl+V"), tr("Show values panel"));
        acOptions_AngleConverter = addAction (menuOptions,
        				tr("Angle converter"), "", "","");
        acOptions_DateChooser = addActionCheck (menuOptions, 
						tr("Date selector"), "", "");
        acOptions_Units = addAction (menuOptions,
        				tr("Units"), tr("Ctrl+U"), "","");
        acOptions_Fonts = addAction (menuOptions,
        				tr("Fonts"), tr("Ctrl+E"), "","");
        acOptions_GraphicsParams = addAction (menuOptions,
						tr("Graphical parameters"), tr("Ctrl+G"), "","");
		//----------------------------------------------------
		QString lang = Util::getSetting("appLanguage", "").toString();
		QString flagIconName = (lang == "") ? "" : Util::pathImg("drapeau_")+lang+".png";
        acOptions_Language = addAction (menuOptions,
						tr("Language"), tr("Language"), "",flagIconName);

    //======================================================================
    menuHelp = new QMenu(tr("Help"));
    //======================================================================
        acHelp_Help = addAction (menuHelp,
        				tr("Help"), tr("Ctrl+H"),
        				"",Util::pathImg("help.png"));
        acHelp_APropos = addAction (menuHelp, tr("About zyGrib"),"","","");
        acHelp_AProposQT = addAction (menuHelp, tr("About QT"),"","","");

    //======================================================================
    addMenu (menuFile);
    addMenu (menuColorMap);
    addMenu (menuAltitude);
    addMenu (menuIsolines);
    addMenu (menuSeaState);
    addMenu (menuMap);
    addMenu (menuOptions);
    addMenu (menuHelp);
    //-------------------------------------
    // Autres objets de l'interface
    //-------------------------------------
    acDatesGrib_prev = addAction ( NULL,
            tr("Last forecast [page up]"),tr("PgUp"),"",Util::pathImg("1leftarrow.png"));
    acDatesGrib_next = addAction ( NULL,
            tr("Next forecast [page down]"),tr("PgDown"),"",Util::pathImg("1rightarrow.png"));

    cbDatesGrib = new QComboBox ();
    cbDatesGrib->setSizeAdjustPolicy (QComboBox::AdjustToContents);
    cbDatesGrib->addItem("-------------------------");
    
	cbDatesGrib->setEnabled (false);
	acDatesGrib_prev->setEnabled (true);
	acDatesGrib_next->setEnabled (true);

	updateFonts();
}

//---------------------------------------------------------
void MenuBar::updateFonts ()
{
	cbDatesGrib->setFont (Font::getFont(FONT_ComboBox));
	
	QFont ft = Font::getFont(FONT_MenuBar);
	this->setFont(ft);
	QList<QAction *> lsactions = this->findChildren<QAction *>();
	for (int i = 0; i < lsactions.size(); ++i) {
		lsactions.at(i)->setFont(ft);
	}
	
}

//---------------------------------------------------------
// Menu popup : bouton droit de la souris
//---------------------------------------------------------
QMenu * MenuBar::createPopupBtRight(QWidget *parent)
{
	QMenu *popup = new QMenu(parent);

	ac_OpenMeteotable = addAction (popup, tr("Meteotable"),"","","");
	ac_CreatePOI = addAction (popup, tr("Mark Point Of Interest"),"","","");
	ac_showSkewtDiagram = addAction (popup, tr("SkewT-LogP diagram"),"","","");

	
	// added by Tim Holtschneider, 05.2010
 	ac_OpenCurveDrawer = addAction (popup, tr("Plot Data"),"","","");

	return popup;
}

//===================================================================================
QAction* MenuBar::addGroup (ZeroOneActionGroup *group, QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName)
{
    QAction *action = addActionCheck (menu, title, shortcut, statustip, iconFileName);
	group->addAction (action);
	return action;
}
//-------------------------------------------------
QAction* MenuBar::addGroup (QActionGroup *group, QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName)
{
    QAction *action = addActionCheck (menu, title, shortcut, statustip, iconFileName);
	group->addAction (action);
	return action;
}
//-------------------------------------------------
QAction* MenuBar::addAction (QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName)
{
    QAction *action;
    action = new QAction(title, menu);
    action->setShortcut  (shortcut);
    action->setShortcutContext (Qt::ApplicationShortcut);
    action->setStatusTip (statustip);
    if (iconFileName != "") {
        action->setIcon(QIcon(iconFileName));
		action->setIconVisibleInMenu(true);
	}
    if (menu != NULL)
        menu->addAction (action);
    return action;
}
//-------------------------------------------------
QAction* MenuBar::addActionCheck (QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName)
{
    QAction *action;
    action = addAction (menu, title, shortcut, statustip, iconFileName);
    action->setCheckable  (true);
    return action;
}
//-------------------------------------------------
void MenuBar::setProjection(int idproj)
{
	switch (idproj)
	{
        case Projection::PROJ_MERCATOR :
    		acMap_PROJ_MERCATOR->setChecked(true);  break;
        case Projection::PROJ_MILLER :
    		acMap_PROJ_MILLER->setChecked(true);    break;
        case Projection::PROJ_CENTRAL_CYL :
    		acMap_PROJ_CENTRAL_CYL->setChecked(true); break;
        case Projection::PROJ_EQU_CYL :
    		acMap_PROJ_EQU_CYL->setChecked(true);   break;
        case Projection::PROJ_ZYGRIB :
		    acMap_PROJ_ZYGRIB->setChecked(true);    break;
	}
}
//-------------------------------------------------
void MenuBar::setQuality (int q) {
    switch (q) {
        case 0: acMap_Quality1->setChecked(true); break;
        case 1: acMap_Quality2->setChecked(true); break;
        case 2: acMap_Quality3->setChecked(true); break;
        case 3: acMap_Quality4->setChecked(true); break;
        case 4: acMap_Quality5->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setIsobarsStep (int step) {
    switch (step) {
        case 1: acView_IsobarsStep1->setChecked(true); break;
        case 2: acView_IsobarsStep2->setChecked(true); break;
        case 3: acView_IsobarsStep3->setChecked(true); break;
        case 4: acView_IsobarsStep4->setChecked(true); break;
        case 5: acView_IsobarsStep5->setChecked(true); break;
        case 6: acView_IsobarsStep6->setChecked(true); break;
        case 8: acView_IsobarsStep8->setChecked(true); break;
        case 10: acView_IsobarsStep10->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setIsotherms0Step (int step) {
    switch (step) {
        case 10: acView_Isotherms0Step10->setChecked(true); break;
        case 20: acView_Isotherms0Step20->setChecked(true); break;
        case 50: acView_Isotherms0Step50->setChecked(true); break;
        case 100: acView_Isotherms0Step100->setChecked(true); break;
        case 200: acView_Isotherms0Step200->setChecked(true); break;
        case 500: acView_Isotherms0Step500->setChecked(true); break;
        case 1000: acView_Isotherms0Step1000->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setIsotherms_Step (int step) {
    switch (step) {
        case 1: acView_Isotherms_Step1->setChecked(true); break;
        case 2: acView_Isotherms_Step2->setChecked(true); break;
        case 5: acView_Isotherms_Step5->setChecked(true); break;
        case 10: acView_Isotherms_Step10->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setLinesThetaE_Step (int step) {
    switch (step) {
        case 1: acView_LinesThetaE_Step1->setChecked(true); break;
        case 2: acView_LinesThetaE_Step2->setChecked(true); break;
        case 5: acView_LinesThetaE_Step5->setChecked(true); break;
        case 10: acView_LinesThetaE_Step10->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setCitiesNamesLevel (int level) {
    switch (level) {
        case 0: acMap_CitiesNames0->setChecked(true); break;
        case 1: acMap_CitiesNames1->setChecked(true); break;
        case 2: acMap_CitiesNames2->setChecked(true); break;
        case 3: acMap_CitiesNames3->setChecked(true); break;
        case 4: acMap_CitiesNames4->setChecked(true); break;
        case 5: acMap_CitiesNames5->setChecked(true); break;
    }
}
//-------------------------------------------------
void MenuBar::setWaveArrowsType (int type) {
    switch (type) {
        case GRB_TYPE_NOT_DEFINED: acView_WavesArrows_none->setChecked(true); break;
        case GRB_PRV_WAV_MAX:  acView_WavesArrows_max->setChecked(true); break;
        case GRB_PRV_WAV_SWL:  acView_WavesArrows_swell->setChecked(true); break;
        case GRB_PRV_WAV_WND:  acView_WavesArrows_wind->setChecked(true); break;
        case GRB_PRV_WAV_PRIM: acView_WavesArrows_prim->setChecked(true); break;
        case GRB_PRV_WAV_SCDY: acView_WavesArrows_scdy->setChecked(true); break;
    }
}

//------------------------------------------------------------
// Génère la liste des dates des Records du fichier GRIB
void MenuBar::updateListeDates(std::set<time_t> *setDates, time_t currentDate)
{
    listGribDates.clear();
    // Construit un vector à partir du set (plus pratique)
    std::set<time_t>::iterator its;
    for (its=setDates->begin(); its!=setDates->end(); its++) {
        listGribDates.push_back(*its);
    }

    // Met à jour les item du QComboBox
    while (cbDatesGrib->count() > 0) {
        cbDatesGrib->removeItem(0);
    }
    std::vector<time_t>::iterator it;
    for (it=listGribDates.begin(); it!=listGribDates.end(); it++) {
        time_t tps = *it;
        QString str = Util::formatDateTimeLong(tps);
        //printf("%s\n", qPrintable(str));
        cbDatesGrib->addItem(str);
    }
	
	updateCurrentDate (currentDate);
}
//------------------------------------------------------------
time_t  MenuBar::getDateGribById(int id)
{
    if (listGribDates.size() > (uint)id)
        return listGribDates[id];
    else
        return (time_t)0;
}
//------------------------------------------------------------
void MenuBar::updateCurrentDate (time_t currentDate)
{
	acDatesGrib_prev->setEnabled (true);
	acDatesGrib_next->setEnabled (true);
	QString strCurrentDate = Util::formatDateTimeLong (currentDate);
	int id = cbDatesGrib->findText (strCurrentDate);
	if (id >= 0) {
		cbDatesGrib->setCurrentIndex (id);
		if (id == 0) {
			acDatesGrib_prev->setEnabled (false);
		}
		if (id == cbDatesGrib->count()-1) {
			acDatesGrib_next->setEnabled (false);
		}
	}
}









