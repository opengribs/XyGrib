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

#include <cmath>

#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>

#include "MainWindow.h"
#include "MeteoTable.h"
#include "POI_Editor.h"
#include "Font.h"
#include "Util.h"
#include "Orthodromie.h"
#include "Settings.h"
#include "Version.h"
#include "AngleConverterDialog.h"
#include "DataQString.h"
#include "CurveDrawer.h"


//-----------------------------------------------------------
void ThreadNewInstance::run ()
{
	QStringList args = QCoreApplication::arguments ();
	if (args.size() >= 1) {
		QString appname = args.at(0);
		//DBGQS (appname);
		QProcess::execute (appname+" -sw");
		exit ();
	}
}

//-----------------------------------------------------------
void MainWindow::autoClose () {		// for debuging purpose
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotFile_Quit()));
 	//timer->start (300);
}

//-----------------------------------------------------------
void MainWindow::InitActionsStatus ()
{
	QString strdtc;
	DataCode dtc;
	
    menuBar->acMap_Rivers->setChecked(Util::getSetting("showRivers", false).toBool());
    menuBar->acMap_LonLatGrid->setChecked(Util::getSetting("showLonLatGrid", true).toBool());
    menuBar->acMap_CountriesBorders->setChecked(Util::getSetting("showCountriesBorders", true).toBool());
    menuBar->acMap_Orthodromie->setChecked(Util::getSetting("showOrthodromie", true).toBool());
    menuBar->acMap_AutoZoomOnGribArea->setChecked(Util::getSetting("autoZoomOnGribArea", true).toBool());

	strdtc = DataCodeStr::serialize (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,10));
    strdtc = Util::getSetting("colorMapData", strdtc).toString();
	dtc = DataCodeStr::unserialize (strdtc);
	setMenubarAltitudeData (dtc);
    setMenubarColorMapData (dtc, true);
    terre->setColorMapData (dtc);
	
	if (colorScaleWidget) { 
		if (terre->getGriddedPlotter() && terre->getGriddedPlotter()->isReaderOk())
				colorScaleWidget->setColorScale (terre->getGriddedPlotter(), dtc);
		colorScaleWidget->setVisible (Util::getSetting("showColorScale", true).toBool());
		menuBar->acView_ShowColorScale->setChecked (Util::getSetting("showColorScale", true).toBool());
	}
	if (boardPanel) { 
		boardPanel->setVisible (Util::getSetting("showBoardPanel", true).toBool());
		menuBar->acView_ShowBoardPanel->setChecked (Util::getSetting("showBoardPanel", true).toBool());
	}

	strdtc = DataCodeStr::serialize (DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,925));
    strdtc = Util::getSetting("geopotentialLinesData", strdtc).toString();
	dtc = DataCodeStr::unserialize (strdtc);

	bool geopdraw = Util::getSetting("drawGeopotentialLines", false).toBool();
	bool glabels  = Util::getSetting("drawGeopotentialLinesLabels", false).toBool();
	int  gstep    = Util::getSetting("drawGeopotentialLinesStep", 10).toInt();
	terre->setGeopotentialData (dtc);
	terre->setDrawGeopotential (geopdraw);
	terre->setDrawGeopotentialLabels (glabels);
	terre->setGeopotentialStep (gstep);
	setMenuBarGeopotentialLines (dtc,geopdraw,glabels,gstep);
	
    menuBar->acView_ColorMapSmooth->setChecked(Util::getSetting("colorMapSmooth", true).toBool());
    menuBar->acView_WindArrow->setChecked(Util::getSetting("showWindArrows", true).toBool());
    menuBar->acView_Barbules->setChecked(Util::getSetting("showBarbules", true).toBool());
    menuBar->acView_ThinWindArrows->setChecked(Util::getSetting("thinWindArrows", true).toBool());

    menuBar->acView_CurrentArrow->setChecked(Util::getSetting("showCurrentArrows", true).toBool());
	
    menuBar->setWaveArrowsType (Util::getSetting("waveArrowsType", GRB_TYPE_NOT_DEFINED).toInt());
    terre->setWaveArrowsType (Util::getSetting("waveArrowsType", GRB_TYPE_NOT_DEFINED).toInt());

    menuBar->setIsobarsStep(Util::getSetting("isobarsStep", 2).toInt());
    terre->setIsobarsStep(Util::getSetting("isobarsStep", 2).toInt());
    menuBar->acView_Isobars->setChecked(Util::getSetting("showIsobars", true).toBool());
    menuBar->acView_IsobarsLabels->setChecked(Util::getSetting("showIsobarsLabels", false).toBool());
    menuBar->acView_PressureMinMax->setChecked(Util::getSetting("showPressureMinMax", false).toBool());

    menuBar->setIsotherms0Step (Util::getSetting("isotherms0Step", 100).toInt());
    terre->setIsotherms0Step (Util::getSetting("isotherms0Step", 100).toInt());
    menuBar->acView_Isotherms0->setChecked (Util::getSetting("showIsotherms0", false).toBool());
    menuBar->acView_Isotherms0Labels->setChecked (Util::getSetting("showIsotherms0Labels", false).toBool());
	//--------------------------------------------
    menuBar->setIsotherms_Step (Util::getSetting("isotherms_Step", 2).toDouble());
    terre->setIsotherms_Step (Util::getSetting("isotherms_Step", 2).toDouble());
    menuBar->acView_Isotherms_Labels->setChecked (Util::getSetting("showIsotherms_Labels", false).toBool());
    
	menuBar->acView_Isotherms_2m->setChecked (Util::getSetting("showIsotherms_2m", false).toBool());
    menuBar->acView_Isotherms_925hpa->setChecked (Util::getSetting("showIsotherms_925hpa", false).toBool());
    menuBar->acView_Isotherms_850hpa->setChecked (Util::getSetting("showIsotherms_850hpa", false).toBool());
    menuBar->acView_Isotherms_700hpa->setChecked (Util::getSetting("showIsotherms_700hpa", false).toBool());
    menuBar->acView_Isotherms_500hpa->setChecked (Util::getSetting("showIsotherms_600hpa", false).toBool());
    menuBar->acView_Isotherms_500hpa->setChecked (Util::getSetting("showIsotherms_500hpa", false).toBool());
    menuBar->acView_Isotherms_500hpa->setChecked (Util::getSetting("showIsotherms_400hpa", false).toBool());
    menuBar->acView_Isotherms_300hpa->setChecked (Util::getSetting("showIsotherms_300hpa", false).toBool());
    menuBar->acView_Isotherms_200hpa->setChecked (Util::getSetting("showIsotherms_200hpa", false).toBool());
	//--------------------------------------------
    menuBar->setLinesThetaE_Step (Util::getSetting("linesThetaE_Step", 2).toDouble());
    terre->setLinesThetaE_Step (Util::getSetting("linesThetaE_Step", 2).toDouble());
    menuBar->acView_LinesThetaE_Labels->setChecked (Util::getSetting("showLinesThetaE_Labels", false).toBool());
    
    menuBar->acView_LinesThetaE_925hpa->setChecked (Util::getSetting("showLinesThetaE_925hpa", false).toBool());
    menuBar->acView_LinesThetaE_850hpa->setChecked (Util::getSetting("showLinesThetaE_850hpa", false).toBool());
    menuBar->acView_LinesThetaE_700hpa->setChecked (Util::getSetting("showLinesThetaE_700hpa", false).toBool());
    menuBar->acView_LinesThetaE_600hpa->setChecked (Util::getSetting("showLinesThetaE_500hpa", false).toBool());
    menuBar->acView_LinesThetaE_500hpa->setChecked (Util::getSetting("showLinesThetaE_500hpa", false).toBool());
    menuBar->acView_LinesThetaE_400hpa->setChecked (Util::getSetting("showLinesThetaE_500hpa", false).toBool());
    menuBar->acView_LinesThetaE_300hpa->setChecked (Util::getSetting("showLinesThetaE_300hpa", false).toBool());
    menuBar->acView_LinesThetaE_200hpa->setChecked (Util::getSetting("showLinesThetaE_200hpa", false).toBool());
	//--------------------------------------------
    menuBar->acView_GribGrid->setChecked(Util::getSetting("showGribGrid", false).toBool());
    menuBar->acView_TemperatureLabels->setChecked(Util::getSetting("showTemperatureLabels", false).toBool());

    menuBar->acMap_CountriesNames->setChecked(Util::getSetting("showCountriesNames", false).toBool());
    menuBar->setCitiesNamesLevel(Util::getSetting("showCitiesNamesLevel", 0).toInt());
    terre->setCitiesNamesLevel(Util::getSetting("showCitiesNamesLevel", 0).toInt());

    menuBar->acMap_ShowPOIs->setChecked(Util::getSetting("showPOIs", true).toBool());
    terre->setShowPOIs(Util::getSetting("showPOIs", true).toBool());
	
    menuBar->acMap_ShowMETARs->setChecked(Util::getSetting("showMETARs", true).toBool());
    this->slotMETARSvisibility (Util::getSetting("showMETARs", true).toBool());

    menuBar->acView_DuplicateFirstCumulativeRecord->setChecked(Util::getSetting("duplicateFirstCumulativeRecord", true).toBool());
    menuBar->acView_DuplicateMissingWaveRecords->setChecked(Util::getSetting("duplicateMissingWaveRecords", true).toBool());
    menuBar->acView_InterpolateValues->setChecked(Util::getSetting("interpolateValues", true).toBool());
    menuBar->acView_WindArrowsOnGribGrid->setChecked(Util::getSetting("windArrowsOnGribGrid", false).toBool());
    menuBar->acView_useJetSTreamColorMap->setChecked(Util::getSetting("useJetStreamColorMap", false).toBool());

    menuBar->acView_CurrentArrowsOnGribGrid->setChecked(Util::getSetting("currentArrowsOnGribGrid", false).toBool());

	menuBar->acOptions_DateChooser->setChecked(Util::getSetting("showDateChooser", true).toBool());

    //----------------------------------------------------------------------
	// Set map quality
	int quality = Util::getSetting("gshhsMapQuality", 2).toInt();
    for (int qual=4; qual>=0; qual--) {
        if (! gshhsReader->gshhsFilesExists(qual)) {
            switch (qual) {
                case 0: menuBar->acMap_Quality1->setEnabled(false); break;
                case 1: menuBar->acMap_Quality2->setEnabled(false); break;
                case 2: menuBar->acMap_Quality3->setEnabled(false); break;
                case 3: menuBar->acMap_Quality4->setEnabled(false); break;
                case 4: menuBar->acMap_Quality5->setEnabled(false); break;
            }
            if (quality > qual)
            	quality = qual-1;
        }
    }
    if (quality < 0) {
        QMessageBox::information (this,
            QString(tr("Error")),
            QString(tr("Maps not found.\n\n")
            		+tr("Check program installation."))
        );
    	quality = 0;
	}
	menuBar->setQuality(quality);
	terre->setMapQuality(quality);
	//-------------------------------------
	terre->setSpecialZone ( Util::getSetting ("specialZone_x0", 0).toDouble(),
							Util::getSetting ("specialZone_y0", 0).toDouble(),
							Util::getSetting ("specialZone_x1", 0).toDouble(),
							Util::getSetting ("specialZone_y1", 0).toDouble() );
							
	bool showZone_Swiss = Util::getSetting ("MBshowZone_Swiss", false).toBool();
	menuBar->acMBlueSwiss_ShowArea->setChecked (showZone_Swiss);
	terre->showSpecialZone (showZone_Swiss);
							
	bool fast = Util::getSetting ("MBfastInterpolation", true).toBool();
	menuBar->acMBlue_fastInterpolation->setChecked (fast);
	terre->setMBlueFastInterpolation (fast);
	//-----------------------------------------
	updateGriddedData ();
}

//-----------------------------------------------------------
void MainWindow::updateGraphicsParameters () {
	if (colorScaleWidget && terre->getGriddedPlotter() && terre->getGriddedPlotter()->isReaderOk()) {
 		colorScaleWidget->setColorScale (terre->getGriddedPlotter(), terre->getColorMapData());
	}
}

//-----------------------------------------------------------
// Connexions des signaux
//-----------------------------------------------------------
void MainWindow::connectSignals()
{
    MenuBar  *mb = menuBar;
	autoClose ();

    //-------------------------------------
    // Actions
    //-------------------------------------
    connect(mb->ac_OpenMeteotable, SIGNAL(triggered()), this, SLOT(slotOpenMeteotable()));
    connect(mb->ac_CreatePOI, SIGNAL(triggered()), this, SLOT(slotCreatePOI()));
    connect(mb->ac_CreateAnimation, SIGNAL(triggered()), this, SLOT(slotCreateAnimation()));
    connect(mb->ac_ExportImage, SIGNAL(triggered()), this, SLOT(slotExportImage()));
    connect(mb->ac_showSkewtDiagram, SIGNAL(triggered()), this, SLOT(slotShowSkewtDiagram()));

    connect(mb->acFile_Open, SIGNAL(triggered()), this, SLOT(slotFile_Open()));
    connect(mb->acFile_Close, SIGNAL(triggered()), this, SLOT(slotFile_Close()));
    connect(mb->acFile_NewInstance, SIGNAL(triggered()), this, SLOT(slotGenericAction()));
    connect(mb->acFile_Load_GRIB, SIGNAL(triggered()), this, SLOT(slotFile_Load_GRIB()));
    connect(mb->acFile_Load_IAC, SIGNAL(triggered()), this, SLOT(slotFile_Load_IAC()));

	connect(mb->acMBlueSwiss_Load, SIGNAL(triggered()), this, SLOT(slotFile_MBLUE_Load()));
	connect(mb->acMBlueSwiss_ShowArea, SIGNAL(triggered()), this, SLOT(slotFile_MBLUE_ShowArea()));
    connect(mb->acMBlue_fastInterpolation, SIGNAL(triggered(bool)),
				terre,  SLOT(setMBlueFastInterpolation(bool)));

    connect(mb->acFile_GribServerStatus, SIGNAL(triggered()), this, SLOT(slotFile_GribServerStatus()));
    connect(mb->acFile_Info_GRIB, SIGNAL(triggered()), this, SLOT(slotFile_Info_GRIB()));

	connect(mb->acFile_Quit, SIGNAL(triggered()), this, SLOT(slotFile_Quit()));
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotFile_Quit()));

    //-------------------------------------------------------
    connect(mb->acMap_GroupQuality, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMap_Quality()));

    connect(mb->acMap_GroupProjection, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMap_Projection(QAction *)));

    connect(mb->acMap_Rivers, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawRivers(bool)));
    connect(mb->acMap_LonLatGrid, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawLonLatGrid(bool)));
    connect(mb->acMap_CountriesBorders, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawCountriesBorders(bool)));
    connect(mb->acMap_Orthodromie, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawOrthodromie(bool)));
    connect(mb->acMap_CountriesNames, SIGNAL(triggered(bool)),
            terre,  SLOT(setCountriesNames(bool)));
    connect(mb->acMap_GroupCitiesNames, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMap_CitiesNames()));
    connect(mb->acMap_AutoZoomOnGribArea, SIGNAL(triggered(bool)), this, SLOT(slotGenericAction()));

    connect(mb->acMap_Zoom_In, SIGNAL(triggered()),
            terre,  SLOT(slot_Zoom_In()));
    connect(mb->acMap_Zoom_Out, SIGNAL(triggered()),
            terre,  SLOT(slot_Zoom_Out()));
    connect(mb->acMap_Zoom_Sel, SIGNAL(triggered()),
            terre,  SLOT(slot_Zoom_Sel()));
    connect(mb->acMap_Zoom_All, SIGNAL(triggered()),
            terre,  SLOT(slot_Zoom_All()));

    connect(mb->acMap_Go_Left, SIGNAL(triggered()),
            terre,  SLOT(slot_Go_Left()));
    connect(mb->acMap_Go_Right, SIGNAL(triggered()),
            terre,  SLOT(slot_Go_Right()));
    connect(mb->acMap_Go_Up, SIGNAL(triggered()),
            terre,  SLOT(slot_Go_Up()));
    connect(mb->acMap_Go_Down, SIGNAL(triggered()),
            terre,  SLOT(slot_Go_Down()));

	connect(mb->acMap_ShowPOIs, SIGNAL(triggered(bool)),
            terre,  SLOT(setShowPOIs(bool)));
	connect(mb->acMap_FindCity, SIGNAL(triggered()),
            this,  SLOT(slotMap_FindCity()));
mb->acMap_FindCity->setVisible(false);	// TODO
	//-------------------------------------------------------
	connect(mb->acMap_SelectMETARs, SIGNAL(triggered()),
            this,  SLOT(slotOpenSelectMetar()));
	connect(mb->acMap_ShowMETARs, SIGNAL(triggered(bool)),
			this, SLOT(slotMETARSvisibility(bool)));
mb->acMap_ShowMETARs->setVisible (false);	// TODO
mb->acMap_SelectMETARs->setVisible (false);	// TODO
	//-------------------------------------------------------
	connect(mb->acView_GroupColorMap, SIGNAL(triggered(QAction *)),
			this, SLOT(slot_GroupColorMap(QAction *)));
	connect(mb->acAlt_GroupAltitude, SIGNAL(triggered(QAction *)),
			this, SLOT(slot_GroupAltitude(QAction *)));
	connect(mb->acView_GroupWavesArrows, SIGNAL(triggered(QAction *)),
			this, SLOT(slot_GroupWavesArrows(QAction *)));
    //-------------------------------------------------------

    connect(mb->acView_ColorMapSmooth, SIGNAL(triggered(bool)),
            terre,  SLOT(setColorMapSmooth(bool)));
    connect(mb->acView_DuplicateFirstCumulativeRecord, SIGNAL(triggered(bool)),
            terre,  SLOT(setDuplicateFirstCumulativeRecord(bool)));
    connect(mb->acView_DuplicateMissingWaveRecords, SIGNAL(triggered(bool)),
            terre,  SLOT(setDuplicateMissingWaveRecords(bool)));
    connect(mb->acView_InterpolateValues, SIGNAL(triggered(bool)),
            terre,  SLOT(setInterpolateValues(bool)));
    connect(mb->acView_WindArrowsOnGribGrid, SIGNAL(triggered(bool)),
            terre,  SLOT(setWindArrowsOnGribGrid(bool)));
    connect(mb->acView_useJetSTreamColorMap, SIGNAL(triggered(bool)),
            this,  SLOT(slotUseJetStreamColorMap(bool)));
    connect(mb->acView_ShowColorScale, SIGNAL(triggered(bool)),
            this,  SLOT(slotShowColorScale(bool)));
    connect(mb->acView_ShowBoardPanel, SIGNAL(triggered(bool)),
            this,  SLOT(slotShowBoardPanel(bool)));

    connect(mb->acView_WindArrow, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawWindArrows(bool)));
    connect(mb->acView_WindArrow, SIGNAL(triggered(bool)),
            this,  SLOT(slotWindArrows(bool)));
    connect(mb->acView_Barbules, SIGNAL(triggered(bool)),
            terre,  SLOT(setBarbules(bool)));
    connect(mb->acView_ThinWindArrows, SIGNAL(triggered(bool)),
            terre,  SLOT(setThinArrows(bool)));

    connect(mb->acView_CurrentArrowsOnGribGrid, SIGNAL(triggered(bool)),
            terre,  SLOT(setCurrentArrowsOnGribGrid(bool)));
    connect(mb->acView_CurrentArrow, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawCurrentArrows(bool)));

    connect(mb->acView_TemperatureLabels, SIGNAL(triggered(bool)),
            terre,  SLOT(slotTemperatureLabels(bool)));
			
    connect(mb->acView_PressureMinMax, SIGNAL(triggered(bool)),
            terre,  SLOT(setPressureMinMax(bool)));
	//-----------------------------------------------------------------
    connect(mb->acView_Isobars, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawIsobars(bool)));
    connect(mb->acView_GroupIsobarsStep, SIGNAL(triggered(QAction *)),
            this, SLOT(slotIsobarsStep()));
    connect(mb->acView_IsobarsLabels, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawIsobarsLabels(bool)));

    connect(mb->acView_Isotherms0, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawIsotherms0(bool)));
    connect(mb->acView_GroupIsotherms0Step, SIGNAL(triggered(QAction *)),
            this, SLOT(slotIsotherms0Step()));
    connect(mb->acView_Isotherms0Labels, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawIsotherms0Labels(bool)));

    //-------------------------------------------------------
    connect(mb->groupIsotherms, SIGNAL(triggered(QAction *)),
            this,  SLOT(slotGroupIsotherms(QAction *)));
	connect(mb->groupIsotherms_Step, SIGNAL(triggered(QAction *)),
            this, SLOT(slotIsotherms_Step()));
    connect(mb->acView_Isotherms_Labels, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawIsotherms_Labels(bool)));
	
    //-------------------------------------------------------
    connect(mb->groupLinesThetaE, SIGNAL(triggered(QAction *)),
            this,  SLOT(slotGroupLinesThetaE(QAction *)));
   connect(mb->groupLinesThetaE_Step, SIGNAL(triggered(QAction *)),
            this, SLOT(slotGroupLinesThetaE_Step()));
    connect(mb->acView_LinesThetaE_Labels, SIGNAL(triggered(bool)),
            terre,  SLOT(setDrawLinesThetaE_Labels(bool)));
	
    //-------------------------------------------------------
    connect(mb->acView_GribGrid, SIGNAL(triggered(bool)),
            terre,  SLOT(setGribGrid(bool)));
    connect(mb->acOptions_DateChooser, SIGNAL(triggered(bool)),
            this,  SLOT(slotShowDateChooser(bool)));
    //-------------------------------------------------------
    connect(mb->acOptions_Units, SIGNAL(triggered()), dialogUnits, SLOT(exec()));
    connect(mb->acOptions_Fonts, SIGNAL(triggered()), dialogFonts, SLOT(exec()));
    connect(dialogUnits, SIGNAL(accepted()), terre, SLOT(slotMustRedraw()));
    connect(dialogUnits, SIGNAL(signalTimeZoneChanged()), this, SLOT(slotTimeZoneChanged()));
	connect(dialogFonts, SIGNAL(accepted()), this, SLOT(slotChangeFonts()));

    connect(mb->acOptions_GraphicsParams, SIGNAL(triggered()), dialogGraphicsParams, SLOT(exec()));
    connect(dialogGraphicsParams, SIGNAL(accepted()), terre, SLOT(updateGraphicsParameters()));
    connect(dialogGraphicsParams, SIGNAL(accepted()), this, SLOT(updateGraphicsParameters()));

    connect(mb->acOptions_Proxy, SIGNAL(triggered()), dialogProxy, SLOT(exec()));
    connect(mb->acOptions_AngleConverter, SIGNAL(triggered()), this, SLOT(slotOpenAngleConverter()));

    connect(mb->acOptions_Language, SIGNAL(triggered()),
            this, SLOT(slotOptions_Language()));

    //-------------------------------------------------------
    connect(mb->acHelp_Help, SIGNAL(triggered()), this, SLOT(slotHelp_Help()));
    connect(mb->acHelp_APropos, SIGNAL(triggered()), this, SLOT(slotHelp_APropos()));
    connect(mb->acHelp_AProposQT, SIGNAL(triggered()), this, SLOT(slotHelp_AProposQT()));

    //-------------------------------------
    // Autres objets de l'interface
    //-------------------------------------
    connect(mb->cbDatesGrib, SIGNAL(activated(int)),
            this, SLOT(slotDateGribChanged(int)));
    connect(mb->acDatesGrib_next, SIGNAL(triggered()),
            this, SLOT(slotDateGribChanged_next()));
    connect(mb->acDatesGrib_prev, SIGNAL(triggered()),
            this, SLOT(slotDateGribChanged_prev()));

    connect(dateChooser, SIGNAL(signalDateChanged (time_t,bool)),
            this, SLOT(slotDateChooserChanged(time_t,bool)));
 
    //-------------------------------------
    // Autres signaux
    //-------------------------------------
    connect(this, SIGNAL(signalMapQuality(int)),
            terre,  SLOT(setMapQuality(int)));
    //-----------------------------------------------------------
    connect(terre, SIGNAL(mouseClicked(QMouseEvent *)),
            this,  SLOT(slotMouseClicked(QMouseEvent *)));
    connect(terre, SIGNAL(mouseMoved(QMouseEvent *)),
            this,  SLOT(slotMouseMoved(QMouseEvent *)));
    connect(terre, SIGNAL(mouseLeave(QEvent *)),
            this,  SLOT(slotMouseLeaveTerre(QEvent *)));
    //-----------------------------------------------------------
	connect(mb->acAlt_GroupGeopotLine, SIGNAL(triggered(QAction *)),
			this,  SLOT(slot_GroupGeopotentialLines (QAction *)));
	connect(mb->acAlt_GroupGeopotStep, SIGNAL(triggered(QAction *)),
			this, SLOT(slot_GroupGeopotentialStep (QAction *)));
	connect(mb->acAlt_GeopotLabels, SIGNAL(triggered(bool)),
			terre,  SLOT(setDrawGeopotentialLabels (bool)));
    //-----------------------------------------------------------
	// added by Tim Holtschneider, 05.2010
	// extra context menu for data plot
	if (mb->ac_OpenCurveDrawer)
 		connect(mb->ac_OpenCurveDrawer, SIGNAL(triggered()), this, SLOT(slotOpenCurveDrawer()));
	
}

//===========================================================================
//===========================================================================
//===========================================================================
MainWindow::MainWindow (int w, int h, bool withmblue, QWidget *parent)
    : QMainWindow (parent)
{
	setWindowIcon (QIcon (Util::pathImg("zyGrib_32.xpm")));

    menuBar = new MenuBar(this, withmblue);
    assert(menuBar);
    //---------------------------------------------------
	// Projection
	proj = NULL;
    initProjection();
	
    //--------------------------------------------------
    int mapQuality = 0;
    gshhsReader = new GshhsReader (Util::pathGshhs().toStdString(), mapQuality);
    assert(gshhsReader);
	
    //--------------------------------------------------
    terre = new Terrain (this, proj, gshhsReader);
    assert(terre);
	
	dateChooser = new DateChooser ();
    assert (dateChooser);

    QWidget *mainFrame = new QWidget ();
    assert (mainFrame);
    QGridLayout *lay = new QGridLayout (mainFrame);
    assert (lay);
	
		lay->setContentsMargins (0,0,0,0);
        lay->addWidget (terre, 0, 0);	
    
        lay->addWidget (dateChooser, 1, 0);	
		
    mainFrame->setLayout (lay);
    this->setCentralWidget (mainFrame);

    //--------------------------------------------------
	setWindowTitle(Version::getShortName());
    gribFileName = "";
    gribFilePath = Util::getSetting("gribFilePath", "").toString();
	
	networkManager = new QNetworkAccessManager (this);
	assert (networkManager);
	
	dialogProxy = new DialogProxy (this);
	assert (dialogProxy);
	dialogUnits = new DialogUnits (this);
	assert (dialogUnits);
	dialogFonts = new DialogFonts (this);
	assert (dialogFonts);
	dialogGraphicsParams = new DialogGraphicsParams (this);
	assert (dialogGraphicsParams);

    //--------------------------------------------------
	createToolBar (withmblue);
    statusBar =new QStatusBar(this);
	assert (statusBar);
    statusBar->setFont (Font::getFont(FONT_StatusBar));

    boardPanel = new BoardPanel (this);
	assert (boardPanel);
    this->addDockWidget (Qt::LeftDockWidgetArea, boardPanel);

	colorScaleWidget = new ColorScaleWidget (this);
	assert (colorScaleWidget);
	this->addDockWidget (Qt::RightDockWidgetArea, colorScaleWidget);
	
    //--------------------------------------------------
    setMenuBar   (menuBar);
    setStatusBar (statusBar);
	
    //---------------------------------------------------------
	// Menu popup : bouton droit de la souris
    //---------------------------------------------------------
    menuPopupBtRight = menuBar->createPopupBtRight(this);

    //---------------------------------------------------------
	// Active les actions
    //---------------------------------------------------------
    InitActionsStatus ();
    connectSignals ();
	
    //---------------------------------------------------------
    // METAR's
    //---------------------------------------------------------
	dialogSelectMetar = NULL;
	createAllMETARs ();

    //---------------------------------------------------------
    // Initialisation des POI's
    //---------------------------------------------------------
	createPOIs ();

    //--------------------------------------------------
	resize (Util::getSetting("mainWindowSize", QSize(w,h)).toSize());
	move   (Util::getSetting("mainWindowPos", QPoint()).toPoint());
	restoreState (Util::getSetting("mainWindowState", QByteArray()).toByteArray());
}
//-----------------------------------------------
MainWindow::~MainWindow()
{
    Util::setSetting("mainWindowSize", size());
    Util::setSetting("mainWindowPos", pos());
    Util::setSetting("mainWindowState", this->saveState ());
}
//-----------------------------------------------
void MainWindow::createToolBar (bool withmblue)
{
    toolBar = addToolBar(tr("Tools"));
	assert (toolBar);
	toolBar->setObjectName ("mainToolBar");
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->addAction(menuBar->acFile_Quit);
    toolBar->addSeparator();
    toolBar->addAction(menuBar->acFile_Open);
    toolBar->addSeparator();
    toolBar->addWidget(menuBar->cbDatesGrib);
    toolBar->addAction(menuBar->acDatesGrib_prev);
    toolBar->addAction(menuBar->acDatesGrib_next);
    toolBar->addSeparator();
    toolBar->addAction(menuBar->acMap_Zoom_In);
    toolBar->addAction(menuBar->acMap_Zoom_Out);
    toolBar->addAction(menuBar->acMap_Zoom_Sel);
    toolBar->addAction(menuBar->acMap_Zoom_All);
    toolBar->addSeparator();
    toolBar->addAction(menuBar->acMap_Go_Left);
    toolBar->addAction(menuBar->acMap_Go_Right);
    toolBar->addAction(menuBar->acMap_Go_Up);
    toolBar->addAction(menuBar->acMap_Go_Down);
    toolBar->addSeparator();
    toolBar->addAction(menuBar->acFile_Load_GRIB);
	if (withmblue)
		toolBar->addAction(menuBar->acMBlueSwiss_Load);
    toolBar->addAction(menuBar->acFile_GribServerStatus);
    toolBar->addAction(menuBar->acFile_Info_GRIB);
    toolBar->addSeparator();
    toolBar->addAction(menuBar->ac_CreateAnimation);
    toolBar->addSeparator();
	
}
//-----------------------------------------------
void MainWindow::moveEvent (QMoveEvent *)
{
    Util::setSetting("mainWindowPos", pos());
}
//-----------------------------------------------
void MainWindow::resizeEvent (QResizeEvent *)
{
    Util::setSetting("mainWindowSize", size());
}

//---------------------------------------------------------------
void MainWindow::initProjection()
{
	if (proj != NULL) {
		delete proj;
		proj = NULL;
	}
    int idproj = Util::getSetting("projectionId", Projection::PROJ_ZYGRIB).toInt();
    double cx = (double) Util::getSetting("projectionCX", 0.0).toDouble();
    double cy = (double) Util::getSetting("projectionCY", 0.0).toDouble();
    double scale = (double) Util::getSetting("projectionScale", 0.5).toDouble();

    switch (idproj)
    {
    	case Projection::PROJ_EQU_CYL :
			proj = new Projection_EQU_CYL (width(), height(), cx, cy, scale);
			break;
    	case Projection::PROJ_CENTRAL_CYL :
			proj = new Projection_CENTRAL_CYL (width(), height(), cx, cy, scale);
			break;
    	case Projection::PROJ_MERCATOR :
			proj = new Projection_MERCATOR (width(), height(), cx, cy, scale);
			break;
    	case Projection::PROJ_MILLER :
			proj = new Projection_MILLER (width(), height(), cx, cy, scale);
			break;

    	case Projection::PROJ_ZYGRIB :
    	default :
			proj = new Projection_ZYGRIB (width(), height(), cx, cy, scale);
	}
	assert(proj);
	menuBar->setProjection(idproj);
}
//-------------------------------------------------
void MainWindow::slotMap_Projection(QAction *act)
{
	int idproj = Projection::PROJ_ZYGRIB;
    MenuBar  *mb = menuBar;
    if (act == mb->acMap_PROJ_ZYGRIB)
        idproj = Projection::PROJ_ZYGRIB;
    else if (act == mb->acMap_PROJ_MERCATOR)
        idproj = Projection::PROJ_MERCATOR;
    else if (act == mb->acMap_PROJ_MILLER)
        idproj = Projection::PROJ_MILLER;
    else if (act == mb->acMap_PROJ_CENTRAL_CYL)
        idproj = Projection::PROJ_CENTRAL_CYL;
    else if (act == mb->acMap_PROJ_EQU_CYL)
        idproj = Projection::PROJ_EQU_CYL;

	double x,y;    // current position
	proj->screen2map(proj->getW()/2,proj->getH()/2, &x, &y);

	Util::setSetting("projectionCX", proj->getCX());
	Util::setSetting("projectionCY", proj->getCY());
	Util::setSetting("projectionScale",  proj->getScale());
	Util::setSetting("projectionId", idproj);

	initProjection();
	proj->setMapPointInScreen(x, y, proj->getW()/2, proj->getH()/2);

	terre->setProjection(proj);
}
//-------------------------------------------------
void MainWindow::slotTimeZoneChanged()
{
    if (terre->getGriddedPlotter()->isReaderOk())
    {
        menuBar->updateListeDates(
        			terre->getGriddedPlotter()->getListDates(),
        			terre->getGriddedPlotter()->getCurrentDate());
    }
}
//-------------------------------------------------
void MainWindow::openMeteoDataFile (QString fileName)
{
	QCursor oldcursor = cursor();
	setCursor(Qt::WaitCursor);
	bool ok,ok2,ok3,ok4,ok5,ok6,ok7,ok8,ok9,ok10,ok11;
	FileDataType meteoFileType = DATATYPE_NONE;
	colorScaleWidget->setColorScale (NULL, DataCode());
	dateChooser->reset ();
	if (QFile::exists(fileName))
	{
		// 	DBG ("open file %s", qPrintable(fileName));	
		bool zoom = Util::getSetting("autoZoomOnGribArea", true).toBool();
		meteoFileType = terre->loadMeteoDataFile (fileName, zoom);
		if (meteoFileType != DATATYPE_NONE)
			Util::setSetting("gribFileName",  fileName);
	}
	
	GriddedPlotter *plotter = terre->getGriddedPlotter();
	if (plotter!=NULL && plotter->isReaderOk())
	{
		//------------------------------------------------
		if (meteoFileType == DATATYPE_GRIB)
		//------------------------------------------------
		{
			setWindowTitle (Version::getShortName()+" - "+ QFileInfo(fileName).fileName());
			menuBar->updateListeDates (plotter->getListDates(),
									   plotter->getCurrentDate() );
			gribFileName = fileName;

			menuBar->menuColorMap->setEnabled (true);
			menuBar->menuIsolines->setEnabled (true);
 
			ok = plotter->hasDataType (GRB_TEMP);
			menuBar->acView_TempColors->setEnabled(ok); 
			menuBar->acView_TemperatureLabels->setEnabled(ok);

			ok = plotter->hasDataType (GRB_SNOW_CATEG);
			menuBar->acView_SnowCateg->setEnabled(ok);
			ok = plotter->hasDataType (GRB_SNOW_DEPTH);
			menuBar->acView_SnowDepth->setEnabled(ok);

			ok = plotter->hasDataType (GRB_FRZRAIN_CATEG);
			menuBar->acView_FrzRainCateg->setEnabled(ok);

			menuBar->acView_CAPEsfc->setEnabled (plotter->hasDataType (GRB_CAPE));
			menuBar->acView_CINsfc->setEnabled (plotter->hasDataType (GRB_CIN));
			menuBar->acView_ThetaEColors->setEnabled (plotter->hasDataType (GRB_PRV_THETA_E));

			ok = plotter->hasDataType (GRB_WIND_VX);
			menuBar->acView_WindColors->setEnabled(ok);
			menuBar->acView_WindArrow->setEnabled(ok);
			menuBar->acView_Barbules->setEnabled(ok);
			menuBar->acView_ThinWindArrows->setEnabled(ok);

			ok = plotter->hasDataType (GRB_PRECIP_TOT);
			menuBar->acView_RainColors->setEnabled(ok);
			ok = plotter->hasDataType (GRB_CLOUD_TOT);
			menuBar->acView_CloudColors->setEnabled(ok);
			ok = plotter->hasDataType (GRB_HUMID_REL);
			menuBar->acView_HumidColors->setEnabled(ok);
			ok = plotter->hasDataType (GRB_DEWPOINT);
			menuBar->acView_DeltaDewpointColors->setEnabled(ok);

			ok = plotter->hasData (GRB_PRESSURE_MSL,LV_MSL,0);
			menuBar->acView_Isobars->setEnabled(ok);
			menuBar->acView_IsobarsLabels->setEnabled(ok);
			menuBar->acView_PressureMinMax->setEnabled(ok);
			menuBar->menuIsobarsStep->setEnabled(ok);

			ok = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOTHERM0,0);
			menuBar->acView_Isotherms0->setEnabled(ok);
			menuBar->acView_Isotherms0Labels->setEnabled(ok);
			menuBar->acView_GroupIsotherms0Step->setEnabled(ok);
			menuBar->menuIsotherms0Step->setEnabled(ok);
			//------------------------------------------------
			ok2 = plotter->hasData (GRB_TEMP,LV_ABOV_GND,2);
			menuBar->acView_Isotherms_2m->setEnabled (ok2);
			ok3 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,925);
			menuBar->acView_Isotherms_925hpa->setEnabled (ok3);
			ok4 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,850);
			menuBar->acView_Isotherms_850hpa->setEnabled (ok4);
			ok5 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,700);
			menuBar->acView_Isotherms_700hpa->setEnabled (ok5);
			ok6 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,500);
			menuBar->acView_Isotherms_500hpa->setEnabled (ok6);
			ok7 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,300);
			menuBar->acView_Isotherms_300hpa->setEnabled (ok7);
			ok8 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,200);
			menuBar->acView_Isotherms_200hpa->setEnabled (ok8);
			ok9 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,400);
			menuBar->acView_Isotherms_400hpa->setEnabled (ok9);
			ok10 = plotter->hasData (GRB_TEMP,LV_ISOBARIC,600);
			menuBar->acView_Isotherms_600hpa->setEnabled (ok10);
			ok = ok2 || ok3 || ok4 || ok5 || ok6 || ok7 || ok8 || ok9 || ok10;
			menuBar->menuIsotherms->setEnabled (ok);
			menuBar->acView_Isotherms_Labels->setEnabled (ok);
			menuBar->menuIsotherms_Step->setEnabled (ok);

			// Set altitude menus
			ok9 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,925);
			menuBar->acAlt_GeopotLine_925hpa->setEnabled (ok9);
			ok8 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,850);
			menuBar->acAlt_GeopotLine_850hpa->setEnabled (ok8);
			ok7 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,700);
			menuBar->acAlt_GeopotLine_700hpa->setEnabled (ok7);
			ok5 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,500);
			menuBar->acAlt_GeopotLine_500hpa->setEnabled (ok5);
			ok3 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,300);
			menuBar->acAlt_GeopotLine_300hpa->setEnabled (ok3);
			ok2 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,200);
			menuBar->acAlt_GeopotLine_200hpa->setEnabled (ok2);
			ok10 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,400);
			menuBar->acAlt_GeopotLine_400hpa->setEnabled (ok10);
			ok11 = plotter->hasData (GRB_GEOPOT_HGT,LV_ISOBARIC,600);
			menuBar->acAlt_GeopotLine_600hpa->setEnabled (ok11);
			ok = ok9 || ok8 || ok7 || ok5 || ok3 || ok2 || ok10 || ok11;
			menuBar->menuGeopotStep->setEnabled (ok);
			menuBar->acAlt_GeopotLabels->setEnabled (ok);
			menuBar->acView_DuplicateFirstCumulativeRecord->setEnabled (true);
			menuBar->acView_DuplicateMissingWaveRecords->setEnabled (true);
			
			// Malformed grib file ?
			GriddedReader *reader = plotter->getReader();
			if (reader->hasAmbiguousHeader()) {
				QMessageBox::warning (this,
				tr("Warning"),
				tr("File :") + fileName 
				+ "\n\n"
				+ tr("The header of this GRIB file do not respect standard format.")
				+ "\n\n"
				+ tr("Despite efforts to interpret it, output may be incorrect.")
				+ "\n\n"
				+ tr("Please inform the supplier of this file that the GDS section of "
				   "the file header is ambiguous, particularly about data position.")					
				+ "\n"
				);
			}
		}
		//------------------------------------------------
		else if (meteoFileType == DATATYPE_MBLUE) 
		//------------------------------------------------
		{
			//DBG("DATATYPE_MBLUE");
			gribFileName = fileName;
			setWindowTitle(Version::getShortName()+" - "+ QFileInfo(fileName).fileName());
			menuBar->updateListeDates (plotter->getListDates(),
									   plotter->getCurrentDate() );
			menuBar->menuColorMap->setEnabled (true);
			menuBar->menuIsolines->setEnabled (true);
			ok = true;
			menuBar->acView_TempColors->setEnabled(ok);
			menuBar->acView_TemperatureLabels->setEnabled(ok);
			menuBar->acView_RainColors->setEnabled(ok);
			menuBar->acView_CloudColors->setEnabled(ok);
			menuBar->acView_HumidColors->setEnabled(ok);
			menuBar->acView_DeltaDewpointColors->setEnabled(ok);
			menuBar->acView_Isobars->setEnabled(ok);
			menuBar->acView_IsobarsLabels->setEnabled(ok);
			menuBar->acView_PressureMinMax->setEnabled(ok);
			menuBar->menuIsobarsStep->setEnabled(ok);
			menuBar->acView_WindColors->setEnabled(ok);
			menuBar->acView_WindArrow->setEnabled(ok);
			menuBar->acView_Barbules->setEnabled(ok);
			menuBar->acView_ThinWindArrows->setEnabled(ok);
			menuBar->menuIsotherms->setEnabled(ok);
			menuBar->acView_Isotherms_Labels->setEnabled(ok);
			menuBar->menuIsotherms_Step->setEnabled(ok);
			ok = false;
			menuBar->acView_SnowCateg->setEnabled(ok);
			menuBar->acView_SnowDepth->setEnabled(ok);
			menuBar->acView_FrzRainCateg->setEnabled(ok);
			menuBar->acView_CAPEsfc->setEnabled(ok);
			menuBar->acView_CINsfc->setEnabled(ok);
			menuBar->acView_ThetaEColors->setEnabled(ok);
			menuBar->acView_Isotherms0->setEnabled(ok);
			menuBar->acView_Isotherms0Labels->setEnabled(ok);
			menuBar->acView_GroupIsotherms0Step->setEnabled(ok);
			menuBar->menuIsotherms0Step->setEnabled(ok);
			menuBar->acView_DuplicateFirstCumulativeRecord->setEnabled(ok);
		}
		//------------------------------------------------------
		// Common actions to all gridded data file
		//------------------------------------------------------
		// Sea current
		ok = plotter->hasDataType (GRB_CUR_VX);
		menuBar->acView_CurrentColors->setEnabled(ok);
		menuBar->acView_CurrentArrow->setEnabled(ok);
		menuBar->acView_CurrentArrowsOnGribGrid->setEnabled(ok);
		// Waves
		menuBar->acView_SigWaveHeight->setEnabled (plotter->hasWaveDataType (GRB_WAV_SIG_HT));
		menuBar->acView_MaxWaveHeight->setEnabled (plotter->hasWaveDataType (GRB_WAV_MAX_HT));
		menuBar->acView_WhiteCapProb->setEnabled (plotter->hasWaveDataType (GRB_WAV_WHITCAP_PROB));
		menuBar->menuWavesArrows->setEnabled (plotter->hasWaveDataType ());
		menuBar->acView_DuplicateMissingWaveRecords->setEnabled (plotter->hasWaveDataType ());
		menuBar->acView_WavesArrows_none->setEnabled (plotter->hasWaveDataType ());
		menuBar->acView_WavesArrows_max->setEnabled (plotter->hasWaveDataType (GRB_WAV_MAX_DIR));
		menuBar->acView_WavesArrows_swell->setEnabled (plotter->hasWaveDataType (GRB_WAV_SWL_DIR));
		menuBar->acView_WavesArrows_wind->setEnabled (plotter->hasWaveDataType (GRB_WAV_WND_DIR));
		menuBar->acView_WavesArrows_prim->setEnabled (plotter->hasWaveDataType (GRB_WAV_PRIM_DIR));
		menuBar->acView_WavesArrows_scdy->setEnabled (plotter->hasWaveDataType (GRB_WAV_SCDY_DIR));
		//------------------------------------------------
		menuBar->acDatesGrib_prev->setEnabled (true);
		menuBar->acDatesGrib_next->setEnabled (true);
		menuBar->cbDatesGrib->setEnabled (true);
		dateChooser->setGriddedPlotter (plotter);
		dateChooser->setVisible (Util::getSetting("showDateChooser", true).toBool());
		//-----------------------------------------------
		updateGriddedData ();
	}
	//------------------------------------------------
	else if (meteoFileType == DATATYPE_IAC) 
	//------------------------------------------------
	{
		//DBG("DATATYPE_IAC");
		std::set<time_t> setDatesEmpty;
		setWindowTitle(Version::getShortName()+" - "+ QFileInfo(fileName).fileName());
		gribFileName = fileName;

		menuBar->updateListeDates(&setDatesEmpty, 0);
		menuBar->menuColorMap->setEnabled (false);
		menuBar->menuIsolines->setEnabled (false);
		menuBar->acDatesGrib_prev->setEnabled (false);
		menuBar->acDatesGrib_next->setEnabled (false);
		menuBar->cbDatesGrib->setEnabled (false);
		dateChooser->setGriddedPlotter (NULL);
		dateChooser->setVisible (false);
	}
	//------------------------------------------------
	else  
	{
		if (meteoFileType != DATATYPE_CANCELLED) {
			QMessageBox::critical (this,
				tr("Error"),
				tr("File :") + fileName + "\n\n"
					+ tr("Can't open file.") + "\n\n"
					+ tr("It's not a GRIB file,") + "\n"
					+ tr("or it contains unrecognized data,") + "\n"
					+ tr("or...")
			);
		}
		dateChooser->setGriddedPlotter (NULL);
		dateChooser->setVisible (false);
		menuBar->acDatesGrib_prev->setEnabled (false);
		menuBar->acDatesGrib_next->setEnabled (false);
		menuBar->cbDatesGrib->setEnabled (false);
	}
	setCursor(oldcursor);
}
//-------------------------------------------------------
void MainWindow::slotUseJetStreamColorMap (bool b) 
{
	Util::setSetting ("useJetStreamColorMap", b);
	updateGriddedData ();
}
//--------------------------------------------------------------
// Ajuste les paramètres des cartes colorées.
// Essaye de réutiliser les paramètres d'affichage précédents
// si compatible avec le fichier.
//--------------------------------------------------------------
void MainWindow::updateGriddedData ()
{
	GriddedPlotter *plotter = terre->getGriddedPlotter();
	if (plotter!=NULL && plotter->isReaderOk())
	{
		GriddedReader *reader = plotter->getReader();
		QString strdtc;
		DataCode dtc;
		strdtc = DataCodeStr::serialize (DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,10));
		strdtc = Util::getSetting ("colorMapData", strdtc).toString();
		dtc = DataCodeStr::unserialize (strdtc);
		
		bool useJetStreamColorMap = Util::getSetting("useJetStreamColorMap", false).toBool();
		if (dtc.dataType==GRB_PRV_WIND_JET || dtc.dataType==GRB_PRV_WIND_XY2D) {
			if (useJetStreamColorMap)
				dtc.dataType = GRB_PRV_WIND_JET;
			else
				dtc.dataType = GRB_PRV_WIND_XY2D;
		}
		// Data existe ?
		DataCode dtc2 = dtc;
		if (dtc2.dataType == GRB_PRV_WIND_JET)
			dtc2.dataType = GRB_PRV_WIND_XY2D;
		if (! reader->hasData(dtc2)) {
			dtc = DataCode (GRB_TYPE_NOT_DEFINED, LV_TYPE_NOT_DEFINED, 0);
		}
		//-----------------------------------------
		setMenubarAltitudeData (dtc);
		setMenubarColorMapData (dtc, true);
		if (colorScaleWidget && terre->getGriddedPlotter() && terre->getGriddedPlotter()->isReaderOk()) {
			colorScaleWidget->setColorScale (terre->getGriddedPlotter(), dtc);
		}
		menuBar->acView_useJetSTreamColorMap->setChecked (useJetStreamColorMap);
		terre->setColorMapData (dtc);
	}
}
//-------------------------------------------------
void MainWindow::slotCreateAnimation()
{
	GriddedPlotter *plotter = terre->getGriddedPlotter();
	
	if (plotter==NULL  || ! plotter->isReaderOk())
	{
        QMessageBox::critical (this,
            tr("Error"),
			tr("Can't create animation :") + "\n"
                + tr("no GRIB file loaded.")
        );
	}
	else if (
			   plotter->getReader()->getReaderFileDataType() != DATATYPE_GRIB
			&& plotter->getReader()->getReaderFileDataType() != DATATYPE_MBLUE
		  )
	{
        QMessageBox::critical (this,
            tr("Error"),
			tr("Can't create animation.")
        );
	}
	else
	{
		new GribAnimator (terre);
	}
}

//-------------------------------------------------
void MainWindow::slotCreatePOI ()
{
	double lon, lat;
	proj->screen2map(mouseClicX,mouseClicY, &lon, &lat);
	new POI_Editor (Settings::getNewCodePOI(), lon, lat, proj, this, terre);
}
//-------------------------------------------------
void MainWindow::createPOIs ()
{
	POI *poi;
    QList<uint> lscodes = Settings::getSettingAllCodesPOIs();
	for (int i=0; i < lscodes.size(); ++i)
	{
		uint code = lscodes.at(i);
 		poi = new POI (code, proj, this, terre);
		connectPOI (poi);
 	}
}
//-------------------------------------------------
void MainWindow::connectPOI (POI *poi)
{
	bool visible = Util::getSetting("showPOIs", true).toBool();
	if (poi!=NULL) {
		if (poi->isValid()) {
			poi->setVisible (visible);
		}
		else {
			Settings::deleteSettingsPOI (poi->getCode() );
			delete poi;
			poi = NULL;
		}
	}
}
//-------------------------------------------------
void MainWindow::slotOpenSelectMetar ()
{
	DBG(" ");
	if (dialogSelectMetar == NULL) {
		dialogSelectMetar = new DialogSelectMetar ();
		assert (dialogSelectMetar);
		connect (dialogSelectMetar, SIGNAL(metarListChanged()), this, SLOT(slotMETARsListChanged()));
	}
	dialogSelectMetar->exec ();
}
//-------------------------------------------------
void MainWindow::slotMETARsListChanged ()
{
	createAllMETARs ();
}
//-------------------------------------------------
void MainWindow::createAllMETARs ()
{
	MetarWidget  *mw;
	MetarWidgetFactory *factory;
	QStringList allMetarsSelected = 
		(Util::getSetting("metar_selected", QStringList()).toStringList() );
	qDeleteAll (listAllMetars);
	listAllMetars.clear ();
	bool isVisible = Util::getSetting("showMETARs", true).toBool();
	if (allMetarsSelected.size() > 0) {
		factory = new MetarWidgetFactory ();
		assert (factory);
		for (int i=0; i < allMetarsSelected.size(); i++) {
			QString icao = allMetarsSelected.at(i);
			mw = factory->createMetarWidget (icao, isVisible, proj, terre);
			listAllMetars.append (mw);
		}
		delete factory;
	}
}
//-------------------------------------------------
void MainWindow::slotMETARSvisibility (bool vis)
{
	Util::setSetting ("showMETARs", vis);
	for (int i=0; i < listAllMetars.size(); i++) {
		listAllMetars.at(i)->setVisible (vis);
	}
}
//-------------------------------------------------
void MainWindow::slotPOImoved(POI* poi)
{
    QString message =
    		poi->getName()+" : "
            + Util::formatPosition(poi->getLongitude(),poi->getLatitude());
    statusBar->showMessage(message);
}
//-------------------------------------------------
void MainWindow::slotFile_Close()
{
    gribFileName = "";
	Util::setSetting ("gribFileName",  gribFileName);
	colorScaleWidget->setColorScale (NULL, DataCode());
    terre->closeMeteoDataFile ();
	dateChooser->reset ();
	// close opened windows related to the file
	QObjectList allobjs = children();
	foreach (QObject *obj, allobjs) {
		QVariant prop = obj->property ("objectType");
		if (prop.isValid()) {
			if (prop.toString() == "MeteoTableDialog")
				delete obj;
		}
	}
}

//-------------------------------------------------
void MainWindow::slotOpenMeteotablePOI (POI* poi)
{
	GriddedPlotter *griddedPlot = terre->getGriddedPlotter();
	if (griddedPlot && griddedPlot->isReaderOk())
	{
		new MeteoTableDialog (
			griddedPlot,
			poi->getLongitude(), poi->getLatitude(), poi->getName());
	}
}
//-------------------------------------------------
void MainWindow::slotOpenMeteotable ()
{
	double lon, lat;
	proj->screen2map(mouseClicX,mouseClicY, &lon, &lat);
	GriddedPlotter *griddedPlot = terre->getGriddedPlotter();
	if (griddedPlot && griddedPlot->isReaderOk())
	{
		new MeteoTableDialog (
			griddedPlot,
			lon, lat, "");
	}
}
//-------------------------------------------------
// added by Tim Holtschneider, 05.2010
// slot for extra context menu entry to plot data in a graph
void MainWindow::slotOpenCurveDrawer()
{
	double lon, lat;
	if( GLOB_listSelectedPOI.empty() ) {
		proj->screen2map(mouseClicX, mouseClicY, &lon, &lat);
		new CurveDrawer( terre->getGriddedPlotter(), lon, lat);
	} else if( GLOB_listSelectedPOI.size() == 1 ) {
		new CurveDrawer( terre->getGriddedPlotter(), GLOB_listSelectedPOI.first()->getLongitude(),
													 GLOB_listSelectedPOI.first()->getLatitude());
	} else  if( GLOB_listSelectedPOI.size() >= 2 ) {
		// OpenCPN
//		new CurveDrawer( terre->getGriddedPlotter(), this );		
	} else {
		QMessageBox::warning( this, tr("Currently it is only possible to select 1 POI for data plot.\nUnselected by left click in map holding shift at the same time"), 
									tr("Currently it is only possible to select 1 POI for data plot.\nUnselected by left click in map holding shift at the same time") );
	}
}
//-------------------------------------------------
void MainWindow::slotOpenAngleConverter()
{
	new AngleConverterDialog(this);
}
//-------------------------------------------------
void MainWindow::slotOptions_Language()
{
    QString oldlang = Util::getSetting("appLanguage", "").toString();
	DialogChooseLang langChooser (this, oldlang);
	langChooser.exec ();
	QString lang = langChooser.getLanguage();
	if (lang != oldlang) {
		Util::setSetting("appLanguage", lang);
		if (lang == "fr")
		{ QMessageBox::information (this,
				QString("Changement de langue"),
				QString("Langue : Français\n\n")
				+ QString("Les modifications seront prises en compte\n")
				+ QString("au prochain lancement du programme.")
				);
		}
		else {
			QMessageBox::information (this,
				QString("Application Language"),
				QString("Language : "+lang+"\n\n")
				+ QString("Please reload application to activate language.\n")
				);
		}
	}
}
//-------------------------------------------------
void MainWindow::slotMap_Quality()
{
	int quality = 0;
    MenuBar  *mb = menuBar;
    QAction *act = mb->acMap_GroupQuality->checkedAction();
    if (act == mb->acMap_Quality1)
        quality = 0;
    else if (act == mb->acMap_Quality2)
        quality = 1;
    else if (act == mb->acMap_Quality3)
        quality = 2;
    else if (act == mb->acMap_Quality4)
        quality = 3;
    else if (act == mb->acMap_Quality5)
        quality = 4;

	Util::setSetting("gshhsMapQuality", quality);
	emit signalMapQuality(quality);
}

//-------------------------------------------------
void MainWindow::slotMap_FindCity ()
{
	
}
//-------------------------------------------------
void MainWindow::slotMap_CitiesNames()
{
    MenuBar  *mb = menuBar;
    QAction *act = mb->acMap_GroupCitiesNames->checkedAction();
    if (act == mb->acMap_CitiesNames0)
        terre->setCitiesNamesLevel(0);
    else if (act == mb->acMap_CitiesNames1)
        terre->setCitiesNamesLevel(1);
    else if (act == mb->acMap_CitiesNames2)
        terre->setCitiesNamesLevel(2);
    else if (act == mb->acMap_CitiesNames3)
        terre->setCitiesNamesLevel(3);
    else if (act == mb->acMap_CitiesNames4)
        terre->setCitiesNamesLevel(4);
    else if (act == mb->acMap_CitiesNames5)
        terre->setCitiesNamesLevel(5);
}
//-------------------------------------------------
void MainWindow::slotIsobarsStep()
{
    int s = 4;
    MenuBar  *mb = menuBar;
    QAction *act = mb->acView_GroupIsobarsStep->checkedAction();
    if (act == mb->acView_IsobarsStep1)
        s = 1;
    else if (act == mb->acView_IsobarsStep2)
        s = 2;
    else if (act == mb->acView_IsobarsStep3)
        s = 3;
    else if (act == mb->acView_IsobarsStep4)
        s = 4;
    else if (act == mb->acView_IsobarsStep5)
        s = 5;
    else if (act == mb->acView_IsobarsStep6)
        s = 6;
    else if (act == mb->acView_IsobarsStep8)
        s = 8;
    else if (act == mb->acView_IsobarsStep10)
        s = 10;
    terre->setIsobarsStep(s);
}
//-------------------------------------------------
void MainWindow::slotIsotherms0Step ()
{
    int s = 100;
    MenuBar  *mb = menuBar;
    QAction *act = mb->acView_GroupIsotherms0Step->checkedAction();
    if (act == mb->acView_Isotherms0Step10)
        s = 10;
    else if (act == mb->acView_Isotherms0Step20)
        s = 20;
    else if (act == mb->acView_Isotherms0Step50)
        s = 50;
    else if (act == mb->acView_Isotherms0Step100)
        s = 100;
    else if (act == mb->acView_Isotherms0Step200)
        s = 200;
    else if (act == mb->acView_Isotherms0Step500)
        s = 500;
    else if (act == mb->acView_Isotherms0Step1000)
        s = 1000;
    terre->setIsotherms0Step(s);
}
//-------------------------------------------------
void MainWindow::slotIsotherms_Step ()
{
    int s = 2;
    MenuBar  *mb = menuBar;
    QAction *act = mb->groupIsotherms_Step->checkedAction();
    if (act == mb->acView_Isotherms_Step1)
        s = 1;
    else if (act == mb->acView_Isotherms_Step2)
        s = 2;
    else if (act == mb->acView_Isotherms_Step5)
        s = 5;
    else if (act == mb->acView_Isotherms_Step10)
        s = 10;
    terre->setIsotherms_Step(s);
}
//-------------------------------------------------
void MainWindow::slotGroupLinesThetaE_Step ()
{
    int s = 2;
    MenuBar  *mb = menuBar;
    QAction *act = mb->groupLinesThetaE_Step->checkedAction();
    if (act == mb->acView_LinesThetaE_Step1)
        s = 1;
    else if (act == mb->acView_LinesThetaE_Step2)
        s = 2;
    else if (act == mb->acView_LinesThetaE_Step5)
        s = 5;
    else if (act == mb->acView_LinesThetaE_Step10)
        s = 10;
    terre->setLinesThetaE_Step (s);
}
//-------------------------------------------------
void MainWindow::slotGroupIsotherms (QAction *ac)
{
	Util::setSetting ("showIsotherms_2m", false);
	Util::setSetting ("showIsotherms_925hpa", false);
	Util::setSetting ("showIsotherms_850hpa", false);
	Util::setSetting ("showIsotherms_700hpa", false);
	Util::setSetting ("showIsotherms_600hpa", false);
	Util::setSetting ("showIsotherms_500hpa", false);
	Util::setSetting ("showIsotherms_400hpa", false);
	Util::setSetting ("showIsotherms_300hpa", false);
	if (ac && ac->isChecked()) {
		Altitude alt;
		Util::setSetting ("showIsotherms_200hpa", false);
		if (ac == menuBar->acView_Isotherms_2m) {
			alt = Altitude (LV_ABOV_GND,2);
			Util::setSetting ("showIsotherms_2m", true);
		}
		else if (ac == menuBar->acView_Isotherms_925hpa) {
			alt = Altitude (LV_ISOBARIC,925);
			Util::setSetting ("showIsotherms_925hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_850hpa) {
			alt = Altitude (LV_ISOBARIC,850);
			Util::setSetting ("showIsotherms_850hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_700hpa) {
			alt = Altitude (LV_ISOBARIC,700);
			Util::setSetting ("showIsotherms_700hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_600hpa) {
			alt = Altitude (LV_ISOBARIC,600);
			Util::setSetting ("showIsotherms_600hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_500hpa) {
			alt = Altitude (LV_ISOBARIC,500);
			Util::setSetting ("showIsotherms_500hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_400hpa) {
			alt = Altitude (LV_ISOBARIC,400);
			Util::setSetting ("showIsotherms_400hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_300hpa) {
			alt = Altitude (LV_ISOBARIC,300);
			Util::setSetting ("showIsotherms_300hpa", true);
		}
		else if (ac == menuBar->acView_Isotherms_200hpa) {
			alt = Altitude (LV_ISOBARIC,200);
			Util::setSetting ("showIsotherms_200hpa", true);
		}
		terre->setIsotherms_Altitude (alt);
		terre->setDrawIsotherms (true);
		//DBGQS ("MainWindow Alt = "+AltitudeStr::toString (alt));
	}
	else {
		terre->setDrawIsotherms (false);
	}
}
//-------------------------------------------------
void MainWindow::slotGroupLinesThetaE (QAction *ac)
{
	Util::setSetting ("showLinesThetaE_925hpa", false);
	Util::setSetting ("showLinesThetaE_850hpa", false);
	Util::setSetting ("showLinesThetaE_700hpa", false);
	Util::setSetting ("showLinesThetaE_600hpa", false);
	Util::setSetting ("showLinesThetaE_500hpa", false);
	Util::setSetting ("showLinesThetaE_400hpa", false);
	Util::setSetting ("showLinesThetaE_300hpa", false);
	Util::setSetting ("showLinesThetaE_200hpa", false);
	if (ac) {
		Altitude alt;
		if (ac == menuBar->acView_LinesThetaE_925hpa) {
			alt = Altitude (LV_ISOBARIC,925);
			Util::setSetting ("showLinesThetaE_925hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_850hpa) {
			alt = Altitude (LV_ISOBARIC,850);
			Util::setSetting ("showLinesThetaE_850hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_700hpa) {
			alt = Altitude (LV_ISOBARIC,700);
			Util::setSetting ("showLinesThetaE_700hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_600hpa) {
			alt = Altitude (LV_ISOBARIC,600);
			Util::setSetting ("showLinesThetaE_600hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_500hpa) {
			alt = Altitude (LV_ISOBARIC,500);
			Util::setSetting ("showLinesThetaE_500hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_400hpa) {
			alt = Altitude (LV_ISOBARIC,400);
			Util::setSetting ("showLinesThetaE_400hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_300hpa) {
			alt = Altitude (LV_ISOBARIC,300);
			Util::setSetting ("showLinesThetaE_300hpa", true);
		}
		else if (ac == menuBar->acView_LinesThetaE_200hpa) {
			alt = Altitude (LV_ISOBARIC,200);
			Util::setSetting ("showLinesThetaE_200hpa", true);
		}
		terre->setLinesThetaE_Altitude (alt);
		terre->setDrawLinesThetaE (true);
		//DBGQS ("MainWindow Alt = "+AltitudeStr::toString (alt));
	}
	else {
		terre->setDrawLinesThetaE (false);
	}
}

//-------------------------------------------------
void MainWindow::slotHelp_Help() {
    QMessageBox::question (this,
            tr("Help"),
            tr("Any question ?"));

    QString coderand = QDateTime::currentDateTime().toString("yyyymmsszzz");

    QMessageBox::information (this,
            tr("Help"),
            tr("Congratulation, your request has been registred.\n")
            +tr("Reference :") + coderand
            +"\n\n"
            +tr("We will contact you as soon as possible... perhaps.")
            +"\n\n"
            +tr("In the meantime, try to press randomly keys of the keyboard, ")
            +tr("or move the mouse and from time to time ")
            +tr("press one button, ")
            +tr("either anything of the other one, ")
            +tr("and you will see what happens...")
            );
}
//-------------------------------------------------
void MainWindow::slotHelp_APropos()
{
    QMessageBox::information (this,
            tr("About"),
            tr("zyGrib : GRIB files visualization")
            +"\n"+
            tr("Version : ")+Version::getVersion()
                    +"      "+Version::getDate()
            +"\n"+ tr("Licence : GNU GPL v3")
            +"\n"+ tr("http://www.zygrib.org")
        );
}
//-------------------------------------------------
void MainWindow::slotHelp_AProposQT() {
	QMessageBox::aboutQt (this);
}

//-------------------------------------------------
void MainWindow::slotFile_Quit() {
    QApplication::quit();
}
//-------------------------------------------------
void MainWindow::slotFile_Open()
{
    QString filter;
/*    filter =  tr("Fichiers GRIB (*.grb *.grib *.grb.bz2 *.grib.bz2 *.grb.gz *.grib.gz)")
            + tr(";;Autres fichiers (*)");*/
	filter="";
    QString fileName = Util::getOpenFileName(this,
                         tr("Choose a GRIB file"),
                         gribFilePath,
                         filter);
    if (fileName != "")
    {
        QFileInfo finfo(fileName);
        gribFilePath = finfo.absolutePath();
    	Util::setSetting("gribFilePath",  gribFilePath);
        openMeteoDataFile (fileName);
    }
}

//========================================================================
void MainWindow::slotFile_Load_IAC()
{
	QString fname = DialogLoadIAC::getFile (networkManager, this);
	if (fname != "") {
		openMeteoDataFile (fname);
	}
}
//---------------------------------------------
void MainWindow::slotFile_MBLUE_Load ()
{
    double x0, y0, x1, y1;
    if ( terre->getSelectedRectangle (&x0,&y0, &x1,&y1)
		 || terre->getGribFileRectangle (&x0,&y0, &x1,&y1) )
    {
		QString fname = DialogLoadMBLUE::getFile (MBLUE_SWISS, 
												  networkManager, this, x0,y0,x1,y1);
		if (fname != "") {
			openMeteoDataFile (fname);
		}
    }
    else {
        QMessageBox::warning (this,
            tr("Download a Meteoblue file"),
            tr("Please select an area on the map."));
    }
}
//---------------------------------------------
void MainWindow::slotFile_MBLUE_ShowArea ()
{
	QAction *bt = (QAction *) sender();

	if (bt == menuBar->acMBlueSwiss_ShowArea) {
		if (bt->isChecked()) {
			double x0,y0, x1,y1;
			if (MblueReader::getMeteoblueTotalArea 
									(MBLUE_SWISS, &x0,&y0,&x1,&y1) ) 
			{
				terre->setSpecialZone  (x0,y0, x1,y1);
				terre->showSpecialZone (true);
				terre->zoomOnZone (x0,y0, x1,y1);
			}
			Util::setSetting ("MBshowZone_Swiss", true);
			Util::setSetting ("specialZone_x0", x0);
			Util::setSetting ("specialZone_x1", x1);
			Util::setSetting ("specialZone_y0", y0);
			Util::setSetting ("specialZone_y1", y1);
		}
		else {
			Util::setSetting ("MBshowZone_Swiss", false);
			terre->showSpecialZone (false);
			terre->slotMustRedraw ();
		}
	}
}

//---------------------------------------------
void MainWindow::slotFile_Load_GRIB ()
{
    double x0, y0, x1, y1;
    if ( terre->getSelectedRectangle (&x0,&y0, &x1,&y1)
		 || terre->getGribFileRectangle (&x0,&y0, &x1,&y1) )
    {
		QString fname = DialogLoadGRIB::getFile (networkManager, this, x0,y0,x1,y1);
		if (fname != "") {
			openMeteoDataFile (fname);
		}
    }
    else {
        QMessageBox::warning (this,
            tr("Download a GRIB file"),
            tr("Please select an area on the map."));
    }
}

//-----------------------------------------------
void MainWindow::slotFile_GribServerStatus()
{
    DialogServerStatus dial (networkManager);
    dial.exec();
}

//-----------------------------------------------
QString MainWindow::dataPresentInGrib (GribReader* grib,
				int dataType,int levelType,int levelValue,
				bool *ok)
{
	if (dataType == GRB_DEWPOINT) {
		switch (grib->getDewpointDataStatus (levelType,levelValue)) {
			case GribReader::DATA_IN_FILE :
				if (ok != NULL) *ok = true;
				return tr("yes");
				break;
			case GribReader::NO_DATA_IN_FILE :
				if (ok != NULL) *ok = false;
				return tr("no");
				break;
			case GribReader::COMPUTED_DATA :
			default :
				if (ok != NULL) *ok = true;
				return tr("no (computed with Magnus-Tetens formula)");
				break;
		}
	}
	else {
		if (grib->getNumberOfGribRecords 
						(DataCode(dataType,levelType,levelValue)) > 0) {
			if (ok != NULL) *ok = true;
			return tr("yes");
		}
		else {
			if (ok != NULL) *ok = false;
			return tr("no");
		}
	}
}

//-----------------------------------------------
void MainWindow::slotFile_Info_GRIB ()
{
	GriddedPlotter *plotter = terre->getGriddedPlotter();
    if (!plotter || !plotter->isReaderOk())    {
        QMessageBox::information (this,
            tr("File information"),
            tr("File not loaded."));
		return;
    }
	GriddedReader *reader = plotter->getReader();
	GriddedRecord *record = reader->getFirstRecord();
    if (!record)    {
        QMessageBox::information (this,
            tr("File information"),
            tr("Data error."));
		return;
    }
    QString msg;
	msg += tr("File : %1\n") .arg( reader->getFileName().c_str());
	msg += tr("Size : %1 bytes\n") .arg(reader->getFileSize());
	msg += "\n";
	msg += tr("Weather center %1") .arg(record->getIdCenter());
	msg += +" - "+ tr("Model %1") .arg(record->getIdModel());
	msg += +" - "+ tr("Grid %1") .arg(record->getIdGrid());
	msg += "\n";

	//msg += tr("%1 enregistrements, ").arg(grib->getTotalNumberOfGribRecords());
	msg += tr("%1 dates:\n").arg(reader->getNumberOfDates());

	std::set<time_t> sdates = reader->getListDates();
	msg += tr("    from %1\n").arg( Util::formatDateTimeLong(*(sdates.begin())) );
	msg += tr("    to %1\n").arg( Util::formatDateTimeLong(*(sdates.rbegin())) );

	msg += "\n";
	msg += tr("Avalaible data :");
	std::set<DataCode> setdata = plotter->getAllDataCode ();
	std::set<DataCode>::iterator it;
	int  currentype = -1;
	bool firstalt = true;
	for (it=setdata.begin(); it!=setdata.end(); it++) {
		DataCode dtc = *it;
		if (   dtc.dataType != GRIB_NOTDEF
			&& dtc.dataType != GRB_WIND_VY
			&& dtc.dataType != GRB_CUR_VY
		) {
			DataCode code = dtc;
			if (dtc.dataType == GRB_WIND_VX)
					code.dataType = GRB_PRV_WIND_XY2D;
			if (dtc.dataType == GRB_CUR_VX)
					code.dataType = GRB_PRV_CUR_XY2D;
			if (currentype != dtc.dataType) {
				msg += "\n* "+DataCodeStr::toString_name (code)+": ";
				currentype = dtc.dataType;
				firstalt = true;
			}
			if (!firstalt)
				msg += ", ";
			else
				firstalt = false;
			//msg += DataCodeStr::toString_level (code);
			msg += AltitudeStr::toStringShort (code.getAltitude());
		}
	}

	msg += "\n";
	msg += "\n";
	if (record->isRegularGrid()) {
		msg += tr("Grid : %1x%2=%3 points") 
					.arg(record->getNi())
					.arg(record->getNj())
					.arg(record->getNi()*record->getNj());
		msg += QString(" : %1°x%2°")
					.arg(record->getDeltaX()).arg(record->getDeltaY());
		msg += "\n";
	}
	else {
		msg += tr("Grid : %1 points")
					.arg(record->getTotalNumberOfPoints());
		double resapprox = 1.0/sqrt(record->getAveragePointsDensity());
		msg += QString("  ≈ %1°x%2°")
					.arg( resapprox, 0,'g',2 )
					.arg( resapprox, 0,'g',2 );
		msg += "\n";
	}

    double x0,y0, x1,y1;
    if (reader->getZoneExtension(&x0,&y0, &x1,&y1))
	{
		msg += tr("Area :");
		QString pos1 = Util::formatPosition (x0, y0);
		QString pos2 = Util::formatPosition (x1, y1);
		msg += QString("%1  ->  %2\n").arg( pos1, pos2);
	}
	msg += "\n";
	msg += tr("Reference date: %1")
					.arg(Util::formatDateTimeLong(reader->getFirstDate()));
	//DBGQS(msg);
	//--------------------------------------------------------------
	QMessageBox::information (this,
		tr("GRIB file information"),
		msg );
}

//========================================================================
void MainWindow::slotDateGribChanged(int id)
{
    time_t date = menuBar->getDateGribById (id);
    //printf("id= %d : %d %s\n",id, (int)date, qPrintable(Util::formatDateTimeLong(date)));
    terre->setCurrentDate (date);
    // Ajuste les actions disponibles
	menuBar->updateCurrentDate (date);
	dateChooser->setDate (date); 
	updateBoardPanel ();
}
//-------------------------------------------------
void MainWindow::slotDateGribChanged_next()
{
    int id = menuBar->cbDatesGrib->currentIndex();
    if (id < menuBar->cbDatesGrib->count()-1) {
        menuBar->cbDatesGrib->setCurrentIndex(id+1);
        slotDateGribChanged(id+1);
    }
}
//-------------------------------------------------
void MainWindow::slotDateGribChanged_prev()
{
    int id = menuBar->cbDatesGrib->currentIndex();
    if (id > 0) {
        menuBar->cbDatesGrib->setCurrentIndex(id-1);
        slotDateGribChanged(id-1);
    }
}
//-------------------------------------------------
void MainWindow::slotDateChooserChanged (time_t date, bool isMoving)
{
	if (! isMoving) {
		menuBar->updateCurrentDate (date);
		terre->setCurrentDate (date);
	}
}
//-------------------------------------------------
void MainWindow::slotShowDateChooser (bool b)
{
	dateChooser->setVisible (b);
	Util::setSetting ("showDateChooser", b);
}
//-------------------------------------------------
void MainWindow::slotShowColorScale (bool b)
{
	if (colorScaleWidget) {
		colorScaleWidget->setVisible (b);
		Util::setSetting ("showColorScale", b);
	}
}
//-------------------------------------------------
void MainWindow::slotShowBoardPanel (bool b)
{
	if (boardPanel) {
		boardPanel->setVisible (b);
		Util::setSetting ("showBoardPanel", b);
	}
}
//-------------------------------------------------
void MainWindow::slotWindArrows(bool b)
{
    // pas de barbules sans flèches
    menuBar->acView_Barbules->setEnabled(b);
}
//-------------------------------------------------
void MainWindow::statusBar_showSelectedZone()
{
    double x0,y0,  x1,y1;
    terre->getSelectedLine(&x0,&y0, &x1,&y1);

    QString message =
            tr("Selected area: ")
            + Util::formatPosition(x0,y0)
            + " -> "
            + Util::formatPosition(x1,y1);

    Orthodromie orth(x0,y0, x1,y1);

    message = message+ "   "
                + tr("(great circle dist:")
                + Util::formatDistance(orth.getDistance())
                + tr("  init.dir: %1°").arg(qRound(orth.getAzimutDeg()))
                + ")";

    statusBar->showMessage(message);
}

//--------------------------------------------------------------
void MainWindow::slotMouseClicked(QMouseEvent * e)
{
statusBar_showSelectedZone();

	mouseClicX = e->x();
 	mouseClicY = e->y();
    switch (e->button()) {
        case Qt::LeftButton : {
			// added by Tim Holtschneider, 05.2010
			// Unmark selected POIs
			if( e->modifiers() == Qt::ShiftModifier ) {
				POI::restoreBgOfSelectedPOIs();
				GLOB_listSelectedPOI.clear();
			}
            break;
        }
        case Qt::MidButton :   // Centre la carte sur le point
            proj->setCentralPixel(e->x(), e->y());
			terre->setProjection(proj);
            break;

        case Qt::RightButton :
            // Affiche un menu popup
            menuPopupBtRight->exec(QCursor::pos());
            break;

        default :
            break;
    }
}
//----------------------------------------------------
void MainWindow::slotMouseMoved (QMouseEvent *)
{
    if (terre->isSelectingZone()) {
        statusBar_showSelectedZone();
    }
    else {
		updateBoardPanel ();
    }
}
//----------------------------------------------------
void MainWindow::updateBoardPanel ()
{
	if (terre->underMouse()) {
		GriddedPlotter *plotter = terre->getGriddedPlotter ();
		double xx, yy;
		int x, y;
		terre->getLastMousePosition (&x, &y);
		proj->screen2map(x, y, &xx, &yy);
		if (plotter != NULL) {
			DataPointInfo  pf (plotter->getReader(),
								xx, yy, plotter->getCurrentDate() );
 			boardPanel->showDataPointInfo (pf, plotter->getWindAltitude());
		}
		else {
 			boardPanel->showPosition (xx, yy);
		}
	}
}
//--------------------------------------------------------------
void MainWindow::slotMouseLeaveTerre (QEvent *) {
	DataPointInfo  pf (NULL, 0,0,0);
	boardPanel->showDataPointInfo (pf, 0);
}
//--------------------------------------------------------------
void MainWindow::slotChangeFonts ()
{
	foreach (FontSelector* fontsel, dialogFonts->hashFontSelectors)
	{
		Font::changeGlobalFont (fontsel->getFontCode(), fontsel->getFont());
	}
	// Default font
	QFont ft;
	ft = dialogFonts->getFontItem (FONT_Default);
	qApp->setFont (ft);
	// Update other fonts
 	boardPanel->updateLabelsSizes();
	menuBar->updateFonts();
	terre->slotMustRedraw();
    statusBar->setFont (Font::getFont(FONT_StatusBar));
}

//-------------------------------------------------
void MainWindow::setMenubarColorMapData (const DataCode &dtc, bool trigAction)
{
    MenuBar  *mb = menuBar;
    QAction  *act = NULL;
	switch (dtc.dataType)
	{
		case GRB_PRV_WIND_XY2D :
		case GRB_PRV_WIND_JET :
			act = mb->acView_WindColors;
			break;
		case GRB_PRV_CUR_XY2D :
			act = mb->acView_CurrentColors;
			break;
		case GRB_PRECIP_TOT :
			act = mb->acView_RainColors;
			break;
		case GRB_CLOUD_TOT :
			act = mb->acView_CloudColors;
			break;
		case GRB_HUMID_REL :
			act = mb->acView_HumidColors;
			break;
		case GRB_TEMP :
			act = mb->acView_TempColors;
			break;
		case GRB_PRV_DIFF_TEMPDEW :
			act = mb->acView_DeltaDewpointColors;
			break;
		case GRB_SNOW_CATEG :
			act = mb->acView_SnowCateg;
			break;
		case GRB_FRZRAIN_CATEG :
			act = mb->acView_FrzRainCateg;
			break;
		case GRB_SNOW_DEPTH :
			act = mb->acView_SnowDepth;
			break;
		case GRB_CAPE :
			act = mb->acView_CAPEsfc;
			break;
		case GRB_CIN :
			act = mb->acView_CINsfc;
			break;
		case GRB_PRV_THETA_E :
			act = mb->acView_ThetaEColors;
			break;
		case GRB_WAV_SIG_HT :
			act = mb->acView_SigWaveHeight;
			break;
		case GRB_WAV_MAX_HT :
			act = mb->acView_MaxWaveHeight;
			break;
		case GRB_WAV_WHITCAP_PROB :
			act = mb->acView_WhiteCapProb;
			break;
	}
    mb->acView_GroupColorMap->setCheckedAction (act, trigAction, false);
}

//=======================================================================
void MainWindow::slot_GroupColorMap (QAction *act)
{
    if (! terre->getGriddedPlotter() || ! terre->getGriddedPlotter()->isReaderOk())    {
		return;
    }
    MenuBar  *mb = menuBar;
	GriddedReader *reader = terre->getGriddedPlotter()->getReader();
	DataCode dtc, dtctmp;
    if (act == mb->acView_WindColors) 
	{	// search "prefered" altitude for wind 
		dtc.set (GRB_TYPE_NOT_DEFINED,LV_TYPE_NOT_DEFINED,0); 
    	dtctmp.set (GRB_PRV_WIND_XY2D,LV_ABOV_GND,10);
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_MSL,0); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_GND_SURF,0); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ABOV_GND,1); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ABOV_GND,2); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ABOV_GND,3); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ABOV_GND,10); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,925); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,850); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,700); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,600); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,500); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,400); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,300); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_WIND_XY2D,LV_ISOBARIC,200); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
	}
    else if (act == mb->acView_CurrentColors)
    	dtc.set (GRB_PRV_CUR_XY2D,LV_GND_SURF,0);
    else if (act == mb->acView_RainColors)
    	dtc.set (GRB_PRECIP_TOT,LV_GND_SURF,0);
    else if (act == mb->acView_CloudColors)
    	dtc.set (GRB_CLOUD_TOT,LV_ATMOS_ALL,0);
    else if (act == mb->acView_HumidColors)
    	dtc.set (GRB_HUMID_REL,LV_ABOV_GND,2);
    else if (act == mb->acView_TempColors)
    	dtc.set (GRB_TEMP,LV_ABOV_GND,2);
    else if (act == mb->acView_DeltaDewpointColors)
    	dtc.set (GRB_PRV_DIFF_TEMPDEW,LV_ABOV_GND,2);
    else if (act == mb->acView_SnowCateg)
    	dtc.set (GRB_SNOW_CATEG,LV_GND_SURF,0);
    else if (act == mb->acView_FrzRainCateg)
    	dtc.set (GRB_FRZRAIN_CATEG,LV_GND_SURF,0);
    else if (act == mb->acView_SnowDepth)
    	dtc.set (GRB_SNOW_DEPTH,LV_GND_SURF,0);
    else if (act == mb->acView_CAPEsfc)
    	dtc.set (GRB_CAPE,LV_GND_SURF,0);
    else if (act == mb->acView_CINsfc)
    	dtc.set (GRB_CIN,LV_GND_SURF,0);
	//-----------------------------------
    else if (act == mb->acView_ThetaEColors)
	{	// search "prefered" altitude for theta-e
		dtc.set (GRB_TYPE_NOT_DEFINED,LV_TYPE_NOT_DEFINED,0); 
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,850); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,925); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,700); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,600); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,500); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,400); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,300); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
		dtctmp.set (GRB_PRV_THETA_E,LV_ISOBARIC,200); 
		if (dtc.dataType==GRB_TYPE_NOT_DEFINED && reader->hasData(dtctmp))
			dtc = dtctmp;
	}
	//-----------------------------------
    else if (act == mb->acView_SigWaveHeight)
    	dtc.set (GRB_WAV_SIG_HT,LV_GND_SURF,0);
    else if (act == mb->acView_MaxWaveHeight)
    	dtc.set (GRB_WAV_MAX_HT,LV_GND_SURF,0);
    else if (act == mb->acView_WhiteCapProb)
    	dtc.set (GRB_WAV_WHITCAP_PROB,LV_GND_SURF,0);
	//-----------------------------------
    else
    	dtc.set (GRB_TYPE_NOT_DEFINED);

	setMenubarAltitudeData (dtc);
    terre->setColorMapData (dtc);
	if (colorScaleWidget && terre->getGriddedPlotter() && terre->getGriddedPlotter()->isReaderOk()) {
 		colorScaleWidget->setColorScale (terre->getGriddedPlotter(), dtc);
	}
}
//--------------------------------------------------------
void MainWindow::slot_GroupAltitude (QAction *act)
{
    if (! terre->getGriddedPlotter() || ! terre->getGriddedPlotter()->isReaderOk())    {
		return;
    }
    MenuBar  *mb = menuBar;
	DataCode dtcaltitude;
    if (act == mb->acAlt_MSL)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_MSL, 0);
    else if (act == mb->acAlt_sigma995)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_SIGMA, 9950);
    else if (act == mb->acAlt_GND)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_GND_SURF, 0);
    else if (act == mb->acAlt_GND_1m)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ABOV_GND, 1);
    else if (act == mb->acAlt_GND_2m)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ABOV_GND, 2);
    else if (act == mb->acAlt_GND_3m)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ABOV_GND, 3);
    else if (act == mb->acAlt_GND_10m)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ABOV_GND, 10);
    else if (act == mb->acAlt_925hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 925);
    else if (act == mb->acAlt_850hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 850);
    else if (act == mb->acAlt_700hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 700);
    else if (act == mb->acAlt_600hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 600);
    else if (act == mb->acAlt_500hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 500);
    else if (act == mb->acAlt_400hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 400);
    else if (act == mb->acAlt_300hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 300);
    else if (act == mb->acAlt_200hpa)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ISOBARIC, 200);
    else if (act == mb->acAlt_Atmosphere)
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_ATMOS_ALL, 0);
	else
		dtcaltitude.set (GRB_TYPE_NOT_DEFINED, LV_TYPE_NOT_DEFINED, 0);

	DataCode dctactual = terre->getColorMapData ();
	
	DataCode dtc (dctactual.dataType, 
				  dtcaltitude.levelType, dtcaltitude.levelValue);
	
	setMenubarAltitudeData (dtc);
    terre->setColorMapData (dtc);
	if (colorScaleWidget && terre->getGriddedPlotter() && terre->getGriddedPlotter()->isReaderOk()) {
		colorScaleWidget->setColorScale (terre->getGriddedPlotter(), dtc);
	}
}
//-------------------------------------------------
void MainWindow::setMenubarAltitudeData (DataCode dtc)
{
    if (! terre->getGriddedPlotter() || ! terre->getGriddedPlotter()->isReaderOk())    {
		return;
    }
    MenuBar  *mb = menuBar;
	GriddedReader *reader = terre->getGriddedPlotter()->getReader();
	
	mb->acAlt_MSL->setEnabled (false);
	mb->acAlt_sigma995 ->setEnabled (false);
	mb->acAlt_GND ->setEnabled (false);
	mb->acAlt_GND_1m ->setEnabled (false);
	mb->acAlt_GND_2m ->setEnabled (false);
	mb->acAlt_GND_3m ->setEnabled (false);
	mb->acAlt_GND_10m ->setEnabled (false);
	mb->acAlt_925hpa ->setEnabled (false);
	mb->acAlt_850hpa ->setEnabled (false);
	mb->acAlt_700hpa ->setEnabled (false);
	mb->acAlt_600hpa ->setEnabled (false);
	mb->acAlt_500hpa ->setEnabled (false);
	mb->acAlt_400hpa ->setEnabled (false);
	mb->acAlt_300hpa ->setEnabled (false);
	mb->acAlt_200hpa ->setEnabled (false);
	mb->acAlt_Atmosphere ->setEnabled (false);

	if (dtc.dataType == GRB_PRV_WIND_JET) {
		dtc.dataType = GRB_PRV_WIND_XY2D;
	}
	std::set<Altitude> setalt = reader->getAllAltitudes (dtc.dataType);
	std::set<Altitude>::iterator it;
	
	for (it=setalt.begin(); it!=setalt.end(); it++) {
		Altitude alt = *it;
		checkAltitude (LV_GND_SURF,0, mb->acAlt_GND, alt, dtc);
		checkAltitude (LV_ABOV_GND,1, mb->acAlt_GND_1m, alt, dtc);
		checkAltitude (LV_ABOV_GND,2, mb->acAlt_GND_2m, alt, dtc);
		checkAltitude (LV_ABOV_GND,3, mb->acAlt_GND_3m, alt, dtc);
		checkAltitude (LV_ABOV_GND,10, mb->acAlt_GND_10m, alt, dtc);
		checkAltitude (LV_MSL,0, mb->acAlt_MSL, alt, dtc);
		checkAltitude (LV_SIGMA,9950, mb->acAlt_sigma995, alt, dtc);
		checkAltitude (LV_ISOBARIC,925, mb->acAlt_925hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,850, mb->acAlt_850hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,700, mb->acAlt_700hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,600, mb->acAlt_600hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,500, mb->acAlt_500hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,400, mb->acAlt_400hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,300, mb->acAlt_300hpa, alt, dtc);
		checkAltitude (LV_ISOBARIC,200, mb->acAlt_200hpa, alt, dtc);
		checkAltitude (LV_ATMOS_ALL,0, mb->acAlt_Atmosphere, alt, dtc);
	}
}
//----------------------------------------------------------------------
void MainWindow::checkAltitude (int levelType,int levelValue, 
								QAction *action,
							   const Altitude &alt, 
							   const DataCode &dtc )
{
	if (alt.equals (levelType,levelValue))
	{
		action->setEnabled (true);
		if (alt == dtc.getAltitude()) {
			action->setChecked (true);
		}
	}
}
//----------------------------------------------------
void MainWindow::setMenuBarGeopotentialLines (
				const DataCode &dtc,
				bool drawGeopot,
				bool drawLabels,
				int  step )
{
    MenuBar  *mb = menuBar; 
	if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,925) && drawGeopot)
		mb->acAlt_GeopotLine_925hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,850) && drawGeopot)
		mb->acAlt_GeopotLine_850hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,700) && drawGeopot)
		mb->acAlt_GeopotLine_700hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,600) && drawGeopot)
		mb->acAlt_GeopotLine_600hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,500) && drawGeopot)
		mb->acAlt_GeopotLine_500hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,400) && drawGeopot)
		mb->acAlt_GeopotLine_400hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,300) && drawGeopot)
		mb->acAlt_GeopotLine_300hpa->setChecked (true);
	else if (dtc.equals (GRB_GEOPOT_HGT,LV_ISOBARIC,200) && drawGeopot)
		mb->acAlt_GeopotLine_200hpa->setChecked (true);
	else {
		mb->acAlt_GeopotLine_925hpa->setChecked (false);
		mb->acAlt_GeopotLine_850hpa->setChecked (false);
		mb->acAlt_GeopotLine_700hpa->setChecked (false);
		mb->acAlt_GeopotLine_600hpa->setChecked (false);
		mb->acAlt_GeopotLine_500hpa->setChecked (false);
		mb->acAlt_GeopotLine_400hpa->setChecked (false);
		mb->acAlt_GeopotLine_300hpa->setChecked (false);
		mb->acAlt_GeopotLine_200hpa->setChecked (false);
	}
	switch (step) {
		case 1 :
			mb->acAlt_GeopotStep_1->setChecked (true); break;
		case 2 :
			mb->acAlt_GeopotStep_2->setChecked (true); break;
		case 5 :
			mb->acAlt_GeopotStep_5->setChecked (true); break;
		case 10 :
			mb->acAlt_GeopotStep_10->setChecked (true); break;
		case 20 :
			mb->acAlt_GeopotStep_20->setChecked (true); break;
		case 50 :
			mb->acAlt_GeopotStep_50->setChecked (true); break;
		case 100 :
			mb->acAlt_GeopotStep_100->setChecked (true); break;
	}
	mb->acAlt_GeopotLabels->setChecked (drawLabels);
}
//----------------------------------------------------
void MainWindow::slot_GroupGeopotentialLines (QAction *act)
{
    MenuBar  *mb = menuBar;
	DataCode dtc;
	if (act == mb->acAlt_GeopotLine_925hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,925);
	else if (act == mb->acAlt_GeopotLine_850hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,850);
	else if (act == mb->acAlt_GeopotLine_700hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,700);
	else if (act == mb->acAlt_GeopotLine_600hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,600);
	else if (act == mb->acAlt_GeopotLine_500hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,500);
	else if (act == mb->acAlt_GeopotLine_400hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,400);
	else if (act == mb->acAlt_GeopotLine_300hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,300);
	else if (act == mb->acAlt_GeopotLine_200hpa)
		dtc.set (GRB_GEOPOT_HGT,LV_ISOBARIC,200);
	else
		dtc.set (GRB_TYPE_NOT_DEFINED,LV_TYPE_NOT_DEFINED,0);
	
	if (dtc.dataType != GRB_TYPE_NOT_DEFINED) {
		terre->setGeopotentialData (dtc);
		terre->setDrawGeopotential (act->isChecked());
	}
	else {
		terre->setDrawGeopotential (false);
	}
}
//----------------------------------------------------
void MainWindow::slot_GroupGeopotentialStep (QAction *act)
{
    MenuBar  *mb = menuBar;
	DataCode dtc;
	if (act == mb->acAlt_GeopotStep_1)
		terre->setGeopotentialStep (1);
	else if (act == mb->acAlt_GeopotStep_2)
		terre->setGeopotentialStep (2);
	else if (act == mb->acAlt_GeopotStep_5)
		terre->setGeopotentialStep (5);
	else if (act == mb->acAlt_GeopotStep_10)
		terre->setGeopotentialStep (10);
	else if (act == mb->acAlt_GeopotStep_20)
		terre->setGeopotentialStep (20);
	else if (act == mb->acAlt_GeopotStep_50)
		terre->setGeopotentialStep (50);
	else if (act == mb->acAlt_GeopotStep_100)
		terre->setGeopotentialStep (100);
}
//-------------------------------------------------
void MainWindow::slotExportImage()
{
	ImageWriter writer (this, terre);
	writer.saveCurrentImage ();
}
//--------------------------------------------------------
void MainWindow::slot_GroupWavesArrows (QAction *act)
{
    if (! terre->getGriddedPlotter() || ! terre->getGriddedPlotter()->isReaderOk())    {
		return;
    }
    MenuBar  *mb = menuBar;
    if (act == mb->acView_WavesArrows_none)
		terre->setWaveArrowsType (GRB_TYPE_NOT_DEFINED);
    else if (act == mb->acView_WavesArrows_max)
		terre->setWaveArrowsType (GRB_PRV_WAV_MAX);
    else if (act == mb->acView_WavesArrows_swell)
		terre->setWaveArrowsType (GRB_PRV_WAV_SWL);
    else if (act == mb->acView_WavesArrows_wind)
		terre->setWaveArrowsType (GRB_PRV_WAV_WND);
    else if (act == mb->acView_WavesArrows_prim)
		terre->setWaveArrowsType (GRB_PRV_WAV_PRIM);
    else if (act == mb->acView_WavesArrows_scdy)
		terre->setWaveArrowsType (GRB_PRV_WAV_SCDY);
}
//-------------------------------------------------
void MainWindow::slotShowSkewtDiagram ()
{
	double lon, lat;
	proj->screen2map (mouseClicX,mouseClicY, &lon, &lat);
	openSkewtDiagramWindow (lon, lat);
}
//-------------------------------------------------
void MainWindow::openSkewtDiagramWindow (double lon, double lat, 
										 GriddedReader *reader, time_t date
										)
{
	//DBG("%g, %g",lon,lat);
	if (reader == NULL) {
		if (! terre->getGriddedPlotter() || ! terre->getGriddedPlotter()->isReaderOk())    {
			return;
		}
		reader = terre->getGriddedPlotter()->getReader();
		date = terre->getGriddedPlotter()->getCurrentDate ();
	}
	SkewT *skewt = new SkewT ();
	skewt->initFromGriddedReader (reader, lon, lat, date);
		
	SkewTWindow *sdial = new SkewTWindow (skewt);
	sdial->show ();
}
//-------------------------------------------------
void MainWindow::slotGenericAction ()
{
	QAction *ac = (QAction *) sender ();
	if (ac == menuBar->acMap_AutoZoomOnGribArea) {
		bool b = menuBar->acMap_AutoZoomOnGribArea->isChecked ();
		Util::setSetting ("autoZoomOnGribArea", b);
	}
	else if (ac == menuBar->acFile_NewInstance) {
		ThreadNewInstance *t = new ThreadNewInstance ();
		if (t)
			t->start();
	}
}







