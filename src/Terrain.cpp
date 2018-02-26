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

#include <iostream>
#include <cassert>

#include <QApplication>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressDialog>
#include <QMessageBox>

#include "Terrain.h"
#include "Orthodromie.h"
#include "Font.h"
#include "DataQString.h"

//---------------------------------------------------------
// Constructeur
//---------------------------------------------------------
Terrain::Terrain (QWidget *parent, Projection *proj, GshhsReader *gshhsReader)
    : QWidget(parent)
{
	setSizePolicy (QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    this->proj = proj;
	
    quality = 0;
    selX0 = selY0 = 0;
    selX1 = selY1 = 0;
	lastMouseX = -1;
	lastMouseY = -1;
    setAutoFillBackground (false);
	drawCartouche = true;
	
    timerResize = new QTimer(this);
    assert(timerResize);
    timerResize->setSingleShot(true);
    connect(timerResize, SIGNAL(timeout()), this, SLOT(slotTimerResize()));
    
    timerZoomWheel = new QTimer(this);
    assert(timerZoomWheel);
    timerZoomWheel->setSingleShot(true);
    connect(timerZoomWheel, SIGNAL(timeout()), this, SLOT(slotTimerZoomWheel()));
    deltaZoomWheel = 1.0;
    
	//---------------------------------------------------
	drawer = new MapDrawer(gshhsReader);
	assert(drawer);
	currentFileType = DATATYPE_NONE;
    
    //----------------------------------------------------------------------------
    showOrthodromie   = Util::getSetting("showOrthodromie", false).toBool();
    duplicateFirstCumulativeRecord = Util::getSetting("duplicateFirstCumulativeRecord", true).toBool();
    duplicateMissingWaveRecords = Util::getSetting("duplicateMissingWaveRecords", true).toBool();
    interpolateValues = Util::getSetting("interpolateValues", true).toBool();
    windArrowsOnGribGrid = Util::getSetting("windArrowsOnGribGrid", false).toBool();
    currentArrowsOnGribGrid = Util::getSetting("currentArrowsOnGribGrid", false).toBool();
	fastInterpolation_MBlue = Util::getSetting ("MBfastInterpolation", true).toBool();
    
    isEarthMapValid = false;
    mustRedraw = true;
 	mustShowSpecialZone = false;
	firstDrawingIsDone = false;
    isResizing = false;
	pleaseWait = false;
	
    //---------------------------------------------------------------
	griddedPlot = NULL;
	iacPlot = NULL;
	taskProgress = NULL;

    //---------------------------------------------------------------
	updateGraphicsParameters();
    setIsobarsStep(drawer->isobarsStep);
    
    isSelectionZoneEnCours = false;
	isDraggingMapEnCours = false;

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
	
	createCrossCursor ();
}
//-------------------------------------------
void Terrain::updateGraphicsParameters()
{            
    drawer->updateGraphicsParameters();
	if (griddedPlot)
		griddedPlot->updateGraphicsParameters();
	isEarthMapValid = false;
	mustRedraw = true;
    update();
}
//-------------------------------------------------------
void Terrain::createCrossCursor ()
{            
	//---------------------------------------------------
	// Define new cross cursor, more visible
	//---------------------------------------------------
/*		* B=1 and M=1 gives black.
		* B=0 and M=1 gives white.
		* B=0 and M=0 gives transparent.
		* B=1 and M=0 gives an XOR'd result.*/
	QBitmap *crossBits = new QBitmap(32,32);
	QBitmap *crossMask = new QBitmap(32,32);
	crossBits->clear();
 	crossMask->clear();
	QPainter pb (crossBits);
	QPainter pm (crossMask);
	QPen pen1(Qt::color1);
	pen1.setWidth(1);
	pb.setPen(pen1);
	pb.drawLine(6,16, 26,16);
	pb.drawLine(16,6, 16,26);
	pm.setPen(pen1);
	 // black
	pm.drawLine(6,16, 26,16);
	pm.drawLine(16,6, 16,26);
	// white
	pm.drawLine(6,15, 26,15);
	pm.drawLine(6,17, 26,17);
	
	pm.drawLine(15,6, 15,26);
	pm.drawLine(17,6, 17,26);
	
	pb.end();
	pm.end();
	myCrossCursor = QCursor(*crossBits, *crossMask);
    
    int v = 180;
    selectColor     = QColor(v,v,v);
    setCursor(myCrossCursor);
}

//-------------------------------------------------------
// Tracé récursif de la route orthodromique
void Terrain::draw_OrthodromieSegment(QPainter &pnt,
                            double x0,double y0, double x1,double y1,
                            int recurs
                            )
{
    if (recurs > 100)
        return;
    Orthodromie *ortho;
    int i0,j0, i1,j1, im,jm;
    double eps = 0.5;
    if (y0 > 90-eps) y0 = 90-eps;
    if (y0 <-90+eps) y0 =-90+eps;
    if (y1 > 90-eps) y1 = 90-eps;
    if (y1 <-90+eps) y1 =-90+eps;
    
    if (fabs(x0-x1)>180)  // il faut faire le tour du monde par derrière
    {
        if (x0 < x1) {
            draw_OrthodromieSegment(pnt, x1-360,y1, x0,y0, recurs+1);
            draw_OrthodromieSegment(pnt, x0+360,y0, x1,y1, recurs+1);
        }
        else {
            draw_OrthodromieSegment(pnt, x0-360,y0, x1,y1, recurs+1);
            draw_OrthodromieSegment(pnt, x1+360,y1, x0,y0, recurs+1);
        }
    }
    else
    {
        proj->map2screen(x0, y0, &i0, &j0);
        proj->map2screen(x1, y1, &i1, &j1);
        if (abs(i0-i1) > 10)
        {
            double xm, ym;
            
            ortho = new Orthodromie(x0, y0, x1, y1);
            ortho->getMidPoint(&xm, &ym);
            delete ortho;
            ortho = NULL;
            
            xm *= 180.0/M_PI;
            ym *= 180.0/M_PI;
            while (ym > 90)
                ym -= 180;
            while (ym < -90)
                ym += 180;
            proj->map2screen(xm, ym, &im, &jm);
            //printf("%5d: (%5d %5d) (%5d %5d) (%5d %5d)      %f %f   %f %f\n",recurs,i0,j0, im,jm, i1,j1,x0,y0,x1,y1);
            draw_OrthodromieSegment(pnt, x0,y0, xm,ym, recurs+1);
            draw_OrthodromieSegment(pnt, xm,ym, x1,y1, recurs+1);
        }
        else {
                pnt.drawLine(i0,j0, i1,j1);
        }
    }
}

//-------------------------------------------------------
void Terrain::draw_Orthodromie(QPainter &pnt)
{
    draw_OrthodromieSegment(pnt, selX0, selY0, selX1, selY1);
}

//---------------------------------------------------------
void Terrain::indicateWaitingMap()
{
    pleaseWait = true;   // Affiche un message d'attente
}

//-------------------------------------------------------
void Terrain::setProjection(Projection *proj)
{ 
    indicateWaitingMap();
    this->proj = proj;
    proj->setScreenSize( width(), height());
	
    Util::setSetting("projectionCX", proj->getCX());
    Util::setSetting("projectionCY", proj->getCY());
    Util::setSetting("projectionScale",  proj->getScale());	
	
	QList<POI*> lpois = getListPOIs();
	for (int i=0; i<lpois.size(); i++) {
		lpois.at(i)->setProjection(proj);
	}
    isEarthMapValid = false;
	mustRedraw = true;
    update();
}

//=========================================================
void Terrain::setDrawRivers(bool b) {
    if (drawer->showRivers != b) {
        drawer->showRivers = b;
        Util::setSetting("showRivers", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawLonLatGrid(bool b) {
    if (drawer->showLonLatGrid != b) {
        drawer->showLonLatGrid = b;
        Util::setSetting("showLonLatGrid", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::slotTemperatureLabels(bool b) {
    if (drawer->showTemperatureLabels != b) {
        drawer->showTemperatureLabels = b;
        Util::setSetting("showTemperatureLabels", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawOrthodromie(bool b) {
    if (showOrthodromie != b) {
        showOrthodromie = b;
        Util::setSetting("showOrthodromie", b);
        mustRedraw = false;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawCountriesBorders(bool b) {
    if (drawer->showCountriesBorders != b) {
        drawer->showCountriesBorders = b;
        Util::setSetting("showCountriesBorders", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setCountriesNames(bool b) {
    if (drawer->showCountriesNames != b) {
        drawer->showCountriesNames = b;
        Util::setSetting("showCountriesNames", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setCitiesNamesLevel  (int level) {
    if (drawer->showCitiesNamesLevel != level) {
        drawer->showCitiesNamesLevel = level;
        Util::setSetting("showCitiesNamesLevel", level);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setWaveArrowsType  (int type) {
    if (drawer->showWaveArrowsType != type) {
        drawer->showWaveArrowsType = type;
        Util::setSetting("waveArrowsType", type);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setMapQuality (int q) {
    indicateWaitingMap();
    if (quality != q) {
        if (drawer->gshhsReader == NULL)
            return;
        quality = q;
        pleaseWait = true;
        //update();
        QCursor oldcursor = cursor();
        setCursor(Qt::WaitCursor);
            drawer->gshhsReader->setUserPreferredQuality(q);
            isEarthMapValid = false;
            update();
        setCursor(oldcursor);
        pleaseWait = false;
    }
}
//-------------------------------------------------------
void Terrain::setDuplicateMissingWaveRecords (bool b) {
    if (duplicateMissingWaveRecords != b) {
        duplicateMissingWaveRecords = b;
        Util::setSetting("duplicateMissingWaveRecords", b);
	    griddedPlot->duplicateMissingWaveRecords (b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDuplicateFirstCumulativeRecord (bool b) {
    if (duplicateFirstCumulativeRecord != b) {
        duplicateFirstCumulativeRecord = b;
        Util::setSetting("duplicateFirstCumulativeRecord", b);
	    griddedPlot->duplicateFirstCumulativeRecord (b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setInterpolateValues (bool b) {
    if (interpolateValues != b) {
        interpolateValues = b;
        Util::setSetting("interpolateValues", b);
	    griddedPlot->setInterpolateValues (b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setWindArrowsOnGribGrid (bool b) {
    if (windArrowsOnGribGrid != b) {
        windArrowsOnGribGrid = b;
        Util::setSetting("windArrowsOnGribGrid", b);
	    griddedPlot->setWindArrowsOnGrid (b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setColorMapData (const DataCode &dtc)
{
	//DBGQS (DataCodeStr::toString (dtc));
    if (drawer)
    {
		Util::setSetting ("colorMapData", DataCodeStr::serialize(dtc));
        drawer->setColorMapData (dtc);
		if (griddedPlot!=NULL && griddedPlot->isReaderOk()) {
			griddedPlot->setUseJetStreamColorMap (
						Util::getSetting("useJetStreamColorMap", false).toBool());
		}
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setCurrentArrowsOnGribGrid (bool b) {
    if (currentArrowsOnGribGrid != b) {
        currentArrowsOnGribGrid = b;
        Util::setSetting("currentArrowsOnGribGrid", b);
	    griddedPlot->setCurrentArrowsOnGrid (b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setColorMapSmooth (bool b) {
    if (drawer->colorMapSmooth != b) {
        drawer->colorMapSmooth = b;
        Util::setSetting("colorMapSmooth", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawCurrentArrows (bool b) {
    if (drawer->showCurrentArrows != b) {
        drawer->showCurrentArrows = b;
        Util::setSetting("showCurrentArrows", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawWindArrows (bool b) {
    if (drawer->showWindArrows != b) {
        drawer->showWindArrows = b;
        Util::setSetting("showWindArrows", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setBarbules (bool b) {
    if (drawer->showBarbules != b) {
        drawer->showBarbules = b;
        Util::setSetting("showBarbules", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setThinArrows (bool b) {
	bool actual = Util::getSetting("thinWindArrows", false).toBool();
    if (actual != b) {
        Util::setSetting("thinWindArrows", b);
		if (griddedPlot) {
			griddedPlot->updateGraphicsParameters ();
		}
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setGribGrid (bool b) {
    if (drawer->showGribGrid != b) {
        drawer->showGribGrid = b;
        Util::setSetting("showGribGrid", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setPressureMinMax (bool b) {
    if (drawer->showPressureMinMax != b) {
        drawer->showPressureMinMax = b;
        Util::setSetting("showPressureMinMax", b);
        mustRedraw = true;
        update();
    }
}

//-------------------------------------------------------
void Terrain::setDrawIsobars (bool b) {
    if (drawer->showIsobars != b) {
        drawer->showIsobars = b;
        Util::setSetting("showIsobars", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setIsobarsStep (double step)
{
    if (drawer->isobarsStep != step) {
        Util::setSetting("isobarsStep", step);
        drawer->isobarsStep = step;
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawIsobarsLabels (bool b) {
    if (drawer->showIsobarsLabels != b) {
        drawer->showIsobarsLabels = b;
        Util::setSetting("showIsobarsLabels", b);
        mustRedraw = true;
        update();
    }
}
//=============================================================
void Terrain::setDrawIsotherms0 (bool b) {
    if (drawer->showIsotherms0 != b) {
        drawer->showIsotherms0 = b;
        Util::setSetting("showIsotherms0", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setIsotherms0Step (double step)
{
    if (drawer->isotherms0Step != step) {
        Util::setSetting("isotherms0Step", step);
        drawer->isotherms0Step = step;
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawIsotherms0Labels (bool b) {
    if (drawer->showIsotherms0Labels != b) {
        drawer->showIsotherms0Labels = b;
        Util::setSetting("showIsotherms0Labels", b);
        mustRedraw = true;
        update();
    }
}
//=================================================================
void Terrain::setIsotherms_Altitude (Altitude alt)
{
    if (drawer->isothermsAltitude != alt) {
        drawer->isothermsAltitude = alt;
		//DBGQS (AltitudeStr::toString (alt));
        Util::setSetting ("isothermsAltitude", AltitudeStr::serialize(alt));
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawIsotherms (bool b) {
    if (drawer->showIsotherms != b) {
        drawer->showIsotherms = b;
        Util::setSetting("showIsotherms", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setIsotherms_Step (double step)
{
    if (drawer->isotherms_Step != step) {
		Util::setSetting("isotherms_Step", step);
		drawer->isotherms_Step = step;
		mustRedraw = true;
		update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawIsotherms_Labels (bool b) {
    if (drawer->showIsotherms_Labels != b) {
        drawer->showIsotherms_Labels = b;
        Util::setSetting("showIsotherms_Labels", b);
        mustRedraw = true;
        update();
    }
}
//=================================================================
void Terrain::setLinesThetaE_Altitude (Altitude alt)
{
    if (drawer->linesThetaEAltitude != alt) {
        drawer->linesThetaEAltitude = alt;
		DBGQS (AltitudeStr::toString (alt));
        Util::setSetting ("linesThetaEAltitude", AltitudeStr::serialize(alt));
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawLinesThetaE (bool b) {
    if (drawer->showLinesThetaE != b) {
        drawer->showLinesThetaE = b;
        Util::setSetting("showLinesThetaE", b);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setLinesThetaE_Step (double step) {
    if (drawer->linesThetaE_Step != step) {
		Util::setSetting("linesThetaE_Step", step);
		drawer->linesThetaE_Step = step;
		mustRedraw = true;
		update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawLinesThetaE_Labels (bool b) {
    if (drawer->showLinesThetaE_Labels != b) {
        drawer->showLinesThetaE_Labels = b;
        Util::setSetting("showLinesThetaE_Labels", b);
        mustRedraw = true;
        update();
    }
}

//=================================================================
bool  Terrain::getSelectedRectangle(double *x0, double *y0, double *x1, double *y1)
{
    if (selX0!=selX1 && selY0!=selY1)
    {   // Si nécessaire, réoriente le rectangle sélectionné
        if (selX0 > selX1) {
            *x0 = selX1;
            *x1 = selX0;
        }
        else {
            *x0 = selX0;
            *x1 = selX1;
        }
        if (selY0 > selY1) {
            *y0 = selY0;
            *y1 = selY1;
        }
        else {
            *y0 = selY1;
            *y1 = selY0;
        }
        return true;
    }
    else {
        return false;
    }
}
//---------------------------------------------------------
bool  Terrain::getSelectedLine(double *x0, double *y0, double *x1, double *y1)
{
    *x0 = selX0;
    *x1 = selX1;
    *y0 = selY0;
    *y1 = selY1;
    return true;
}
//---------------------------------------------------------
bool  Terrain::getGribFileRectangle(double *x0, double *y0, double *x1, double *y1)
{
	if (griddedPlot) {
		if (griddedPlot->isReaderOk()) {
			return griddedPlot->getReader()->getZoneExtension (x0,y0, x1,y1);
		}
	}
    return false;
}

//---------------------------------------------------------
// Grib or IAC files or ...
//---------------------------------------------------------
FileDataType Terrain::loadMeteoDataFile (QString fileName, bool zoom)
{
    indicateWaitingMap();
	currentFileType = DATATYPE_NONE;
	bool ok = false;
	
	taskProgress = new LongTaskProgress (this);
	assert (taskProgress);
	taskProgress->continueDownload = true;
	
	if (griddedPlot != NULL) {
		delete griddedPlot;
		griddedPlot = NULL;
	}
	if (iacPlot != NULL) {
		delete iacPlot;
		iacPlot = NULL;
	}
	taskProgress->setMessage (LTASK_OPEN_FILE);
	taskProgress->setValue (0);
    //--------------------------------------------------------
    // Ouverture du fichier
    //--------------------------------------------------------
    ZUFILE *file = zu_open (qPrintable(fileName), "rb", ZU_COMPRESS_AUTO);
    if (file == NULL) {
        erreur("Can't open file: %s", qPrintable(fileName));
		taskProgress->setVisible (false);
		delete taskProgress;
		taskProgress = NULL;
        return DATATYPE_NONE;
    }
    int nbrecs = GribReader::countGribRecords (file, taskProgress);

    //----------------------------------------------
	GriddedPlotter  *griddedPlot_Temp = NULL;
	if (nbrecs>0 && !ok && taskProgress->continueDownload) {	// try to load a GRIB file
		//DBGQS("try to load a GRIB1 file: "+fileName);
		taskProgress->setWindowTitle (tr("Open file")+" GRIB");
		taskProgress->setVisible (true);
		taskProgress->setValue (0);
		griddedPlot_Temp = new GribPlot ();
		assert(griddedPlot_Temp);
		griddedPlot_Temp->loadFile (fileName, taskProgress, nbrecs);    // GRIB file ?
		if (griddedPlot_Temp->isReaderOk()) {
			currentFileType = DATATYPE_GRIB;
			ok = true;
		}
		else {
			delete griddedPlot_Temp;
			griddedPlot_Temp = NULL;
		}
	}
	if (nbrecs>0 && !ok && taskProgress->continueDownload) {	// try to load a GRIB2 file
		//DBGQS("try to load a GRIB2 file: "+fileName);
		taskProgress->setWindowTitle (tr("Open file")+" GRIB2");
		taskProgress->setVisible (true);
		taskProgress->setValue (0);
		griddedPlot_Temp = new Grib2Plot ();
		assert(griddedPlot_Temp);
		griddedPlot_Temp->loadFile (fileName, taskProgress, nbrecs);    // GRIB file ?
		if (griddedPlot_Temp->isReaderOk()) {
			currentFileType = DATATYPE_GRIB;
			ok = true;
		}
		else {
			delete griddedPlot_Temp;
			griddedPlot_Temp = NULL;
		}
	}
	if (!ok && taskProgress->continueDownload) {	// try to load a MBLUE file
		//DBG("try to load a MBLUE file");
		taskProgress->setWindowTitle (tr("Open file")+" MBLUE");
		taskProgress->setVisible (true);
		taskProgress->setValue (0);
		griddedPlot_Temp = new MbluePlot ();
		assert(griddedPlot_Temp);
		griddedPlot_Temp->loadFile (fileName, taskProgress);    // MBLUE file ?
		if (griddedPlot_Temp->isReaderOk()) {
			griddedPlot_Temp->setFastInterpolation (fastInterpolation_MBlue);
			currentFileType = DATATYPE_MBLUE;
			ok = true;
		}
		else {
			delete griddedPlot_Temp;
			griddedPlot_Temp = NULL;
		}
	}
	if (!ok && taskProgress->continueDownload) {	// try to load a IAC file
		//DBG("try to load a IAC file");
		iacPlot = new IacPlot ();
		assert(iacPlot);
		iacPlot->loadFile (fileName);      // IAC file ?
		if (iacPlot->isReaderOk()) {
			currentFileType = DATATYPE_IAC;
			ok = true;
		}
		else {
			delete iacPlot;
			iacPlot = NULL;
		}
	}
	
	taskProgress->setVisible (false);
	
	griddedPlot = griddedPlot_Temp;
	if (ok) {	// initializes data plotter
		switch (currentFileType) {
			case DATATYPE_GRIB :
				griddedPlot_Temp->setInterpolateValues (interpolateValues);
				griddedPlot_Temp->setWindArrowsOnGrid  (windArrowsOnGribGrid);
				griddedPlot_Temp->setCurrentArrowsOnGrid  (currentArrowsOnGribGrid);
				griddedPlot_Temp->duplicateFirstCumulativeRecord (duplicateFirstCumulativeRecord);
				griddedPlot_Temp->duplicateMissingWaveRecords (duplicateMissingWaveRecords);
				griddedPlot_Temp->setCurrentDateClosestFromNow ();
				griddedPlot_Temp->setUseJetStreamColorMap (
								Util::getSetting("useJetStreamColorMap", false).toBool());
				break;
			case DATATYPE_MBLUE :
				griddedPlot_Temp->setInterpolateValues (interpolateValues);
				griddedPlot_Temp->setWindArrowsOnGrid  (windArrowsOnGribGrid);
				griddedPlot_Temp->setCurrentArrowsOnGrid  (currentArrowsOnGribGrid);
				griddedPlot_Temp->setCurrentDateClosestFromNow ();
				griddedPlot_Temp->setUseJetStreamColorMap (
								Util::getSetting("useJetStreamColorMap", false).toBool());
				break;
			default :
				break;
		}
	}
	else {
		//DBG("ERROR: unknown file type");
	}
	
	isSelectionZoneEnCours = false;
	isDraggingMapEnCours = false;
    selX0 = selY0 = 0;
    selX1 = selY1 = 0;
    isEarthMapValid = false;
	mustRedraw = true;
    if (zoom) {
        zoomOnFileZone();    // Zoom sur la zone couverte par le fichier GRIB
    }
    update();

	bool cancelled = ! taskProgress->continueDownload;
	delete taskProgress;
	taskProgress = NULL;
	
	if (cancelled)
		return DATATYPE_CANCELLED;
	else
		return currentFileType;
}

//---------------------------------------------------------
GriddedPlotter *Terrain::getGriddedPlotter ()
{
    if (   currentFileType == DATATYPE_GRIB
		|| currentFileType == DATATYPE_MBLUE
	) {
		return griddedPlot;
    }
	else {
		return NULL;
	}
}

//---------------------------------------------------------
void   Terrain::closeMeteoDataFile()
{
	if (griddedPlot != NULL) {
		delete griddedPlot;
		griddedPlot = NULL;
	}
	if (iacPlot != NULL) {
		delete iacPlot;
		iacPlot = NULL;
	}
	currentFileType = DATATYPE_NONE;
	mustRedraw = true;
    update();
}

//---------------------------------------------------------
void Terrain::zoomOnFileZone ()
{
	double x0,y0, x1,y1;
	bool ok = false;
    if (   currentFileType == DATATYPE_GRIB
		|| currentFileType == DATATYPE_MBLUE
    ) {
		if (griddedPlot!=NULL && griddedPlot->isReaderOk()) {
			ok = griddedPlot->getReader()->getZoneExtension(&x0,&y0, &x1,&y1);
		}
    }
    else if (currentFileType == DATATYPE_IAC) {
		if (iacPlot!=NULL && iacPlot->isReaderOk()) {
			ok = iacPlot->getReader()->getZoneExtension(&x0,&y0, &x1,&y1);
		}
    }

	if (ok)	{
		this->zoomOnZone (x0, y0, x1, y1);
	}
}
//---------------------------------------------------------
void  Terrain::setSpecialZone  (double x0, double y0, double x1, double y1)
{
	double tmp;
	if (x0 > y0) {
		tmp=x0; x0=x1; x1=tmp;
	}
	if (y0 > y0) {
		tmp=y0; y0=y1; y1=tmp;
	}
	specialZoneX0 = x0;
	specialZoneY0 = y0;
	specialZoneX1 = x1;
	specialZoneY1 = y1;
}

//---------------------------------------------------------
void  Terrain::showSpecialZone (bool b)
{
 	mustShowSpecialZone = b;
}

//---------------------------------------------------------
void Terrain::zoomOnZone (double x0, double y0, double x1, double y1)
{
	double mh, mv;
	mh = fabs(x0-x1)*0.05;
	mv = fabs(y0-y1)*0.05;
	proj->setVisibleArea (x0-mh,y0-mv, x1+mh,y1+mv);
	setProjection (proj);
}

//---------------------------------------------------------
void Terrain::slotMustRedraw()
{
    indicateWaitingMap();
    isEarthMapValid = false;
	mustRedraw = true;
    update();
}
//---------------------------------------------------------
void Terrain::setCurrentDate(time_t t)
{
    if (griddedPlot->getCurrentDate() != t)
    {
        indicateWaitingMap();
        griddedPlot->setCurrentDate(t);
		mustRedraw = true;
        update();
    }
}


//---------------------------------------------------------
// Events
//---------------------------------------------------------
//---------------------------------------------------------
void Terrain::enterEvent (QEvent * /*e*/) {
//printf("enter\n");
    enterCursor = cursor();
    setCursor(myCrossCursor);
}
//---------------------------------------------------------
void Terrain::leaveEvent (QEvent * e) {
//printf("leave\n");
	emit mouseLeave (e);
    setCursor(enterCursor);
}

//---------------------------------------------------------
void  Terrain::keyPressEvent (QKeyEvent *e)
{
//printf("Terrain::keyPressEvent\n");
	keyModifiers = e->modifiers();
    if (keyModifiers == Qt::ControlModifier) {
        setCursor(Qt::ClosedHandCursor);
    }
    else if (keyModifiers == Qt::ShiftModifier) {
        setCursor(Qt::UpArrowCursor);
    }
    else {
        setCursor(myCrossCursor);
    }
}
//---------------------------------------------------------
void  Terrain::keyReleaseEvent (QKeyEvent *e)
{
//printf("keyReleaseEvent\n");
	keyModifiers = e->modifiers();
    if (keyModifiers == Qt::ControlModifier) {
        setCursor(Qt::ClosedHandCursor);
    }
    else if (keyModifiers == Qt::ShiftModifier) {
        setCursor(Qt::UpArrowCursor);
    }
    else {
        setCursor(myCrossCursor);
    }
}
//---------------------------------------------------------
void  Terrain::wheelEvent(QWheelEvent * e)
{
//printf("wheelEvent\n");
	double k = 1.3;
	
	if (e->delta() > 0)
		deltaZoomWheel *= k;
	else
		deltaZoomWheel /= k;
	
	// Le timer évite les multiples update() pendant les changements de taille
    timerZoomWheel->stop();	 // pas d'update() tout de suite
    timerZoomWheel->start(100); // update() seulement après une petite inactivité
}
//---------------------------------------------------------
void Terrain::slotTimerZoomWheel () {
    if (deltaZoomWheel != 1) {
		//printf("slotTimerZoomWheel\n");
		proj->zoom(deltaZoomWheel);
		deltaZoomWheel = 1;
		setProjection(proj);
		//DBGN(proj->getScale());
    }
}


//---------------------------------------------------------
void Terrain::mousePressEvent (QMouseEvent * e) {
//printf("press\n");
    if (e->button() == Qt::LeftButton)
    {
        // Début de sélection de zone rectangulaire
		if (e->modifiers() == Qt::ControlModifier) {
		// TODO use  tiles to drag map
			isDraggingMapEnCours = true;
		}
		else {
			isSelectionZoneEnCours = true;
		}
        proj->screen2map(e->x(),e->y(), &selX1, &selY1);
        selX0 = selX1;
        selY0 = selY1;
		globalX0 = e->globalX();
		globalY0 = e->globalY();
        update();
    }
}
//---------------------------------------------------------
void Terrain::mouseReleaseEvent (QMouseEvent * e) {
    double x0, y0, x1, y1;
    
    globalX0 = 0;
    globalY0 = 0;
	
    if (isDraggingMapEnCours)
    {
		// TODO use  tiles to drag map
        isDraggingMapEnCours = false;
		setProjection (proj);
    }
    if (isSelectionZoneEnCours)
    {
        isSelectionZoneEnCours = false;
        proj->screen2map(e->x(),e->y(), &selX1, &selY1);
        if (getSelectedRectangle(&x0,&y0, &x1,&y1))
        {
            emit selectionOK(x0, y0, x1, y1);
            // Zoom sur la sélection si Shift appuyé
            if (e->modifiers() == Qt::ShiftModifier) {
                this->slot_Zoom_Sel();
            }
        }
        else {
            emit mouseClicked(e);
        }
    }
    else {
        emit mouseClicked(e);
    }
}

//---------------------------------------------------------
void Terrain::mouseMoveEvent (QMouseEvent * e) 
{
    if (isDraggingMapEnCours)
    {
		// TODO use  tiles to drag map
		double x0, y0, x1, y1;
		int mx = lastMouseX-e->x();
		int my = lastMouseY-e->y();
		proj->screen2map (mx, my,  &x0, &y0);
		proj->screen2map (mx+width(), my+height(),  &x1, &y1);
		proj->setVisibleArea (x0,y0, x1,y1);
		setProjection (proj);
    }
    else if (isSelectionZoneEnCours)
    {
		proj->screen2map (e->x(),e->y(), &selX1, &selY1);
		update();	// draw selection rectangle
    }
    
	lastMouseX = e->x();
	lastMouseY = e->y();
	emit mouseMoved(e);
}

//---------------------------------------------------------
void Terrain::resizeEvent (QResizeEvent * /*e*/)
{
	static int cpt = 0;
	cpt ++;
	
    indicateWaitingMap();

	int W = width();
	int H = height();
	double x, y;
	proj->screen2map(0, 0, &x, &y);	// position du coin haut-gauche
    proj->setScreenSize(W, H);
	
    if (cpt > 1) {   // pas de repositionnement lors de l'ouverture (1er resize)
    	proj->setMapPointInScreen(x,y, 0,0); // coin haut-gauche fixe
    }
    isResizing = true;
	
	// Le timer évite les multiples update() pendant les changements de taille
    timerResize->stop();	 // pas d'update() tout de suite
	timerResize->start(100); // update() seulement après une petite inactivité
}
//---------------------------------------------------------
void Terrain::slotTimerResize () {
    if (isResizing) {
		isEarthMapValid = false;
		mustRedraw = true;
        isResizing = false;
        update();
    }
}

//---------------------------------------------------------
// paintEvent
//---------------------------------------------------------
void Terrain::paintEvent(QPaintEvent *)
{
    QPainter pnt (this);
    QColor transp;
    int r = 100;
    if (!isResizing || !firstDrawingIsDone)
    {
		firstDrawingIsDone = true;
        // Draw the map and the GRIB data
        QCursor oldcursor = cursor();
        setCursor(Qt::WaitCursor);
        
        switch (currentFileType) {
			case DATATYPE_GRIB :
			case DATATYPE_MBLUE :
				drawer->draw_GSHHS_and_GriddedData 
					(pnt, mustRedraw, isEarthMapValid, proj, griddedPlot, drawCartouche);
				break;
			case DATATYPE_IAC :
				drawer->draw_GSHHS_and_IAC 
					(pnt, mustRedraw, isEarthMapValid, proj, iacPlot, drawCartouche);
				break;
			default :
				drawer->draw_GSHHS (pnt, mustRedraw, isEarthMapValid, proj);
        }
		
        setCursor(oldcursor);
		isEarthMapValid = true;
		mustRedraw = false;
		pleaseWait = false;
        
        if (selX0!=selX1 && selY0!=selY1) {
            // Draw the rectangle of the selected zone
            pnt.setPen(selectColor);
            transp = QColor(r,r,r, 80);
            pnt.setBrush(transp);
            int x0,y0, x1,y1;
            proj->map2screen(selX0, selY0, &x0, &y0);
            proj->map2screen(selX1, selY1, &x1, &y1);
            pnt.drawRect(x0, y0, x1-x0, y1-y0);
            
            if (showOrthodromie)
            {
                QPen penLine(Qt::white);
                penLine.setWidthF(1.6);
                pnt.setPen(penLine);
                draw_Orthodromie(pnt);
            }
        }
    }
    else {
        switch (currentFileType) {
			case DATATYPE_GRIB :
			case DATATYPE_MBLUE :
				drawer->draw_GSHHS_and_GriddedData 
						(pnt, false, true, proj, griddedPlot);
				break;
			case DATATYPE_IAC :
				drawer->draw_GSHHS_and_IAC 
						(pnt, false, true, proj, iacPlot);
				break;
			default :
				drawer->draw_GSHHS (pnt, mustRedraw, isEarthMapValid, proj);
        }
	}
    
    if (mustShowSpecialZone) {
		if (specialZoneX0!=specialZoneX1 && specialZoneY0!=specialZoneY1) {
			pnt.setPen(QColor(Qt::white));
			r = 80;
			transp = QColor(r,r,r, 50);
			pnt.setBrush(transp);
			int x0,y0, x1,y1;
			proj->map2screen(specialZoneX0, specialZoneY0, &x0, &y0);
			proj->map2screen(specialZoneX1, specialZoneY1, &x1, &y1);
			pnt.drawRect(x0, y0, x1-x0, y1-y0);
		}
    }
    
    if (pleaseWait) {
        // Write the message "please wait..." on the map
        QFont fontWait = Font::getFont(FONT_MapWait);
        QFontMetrics fmet(fontWait);
        pnt.setPen(QColor(Qt::white));
        r = 80;
        transp = QColor(r,r,r, 80);
        pnt.setFont(fontWait);
        pnt.setBrush(transp);
        QString txt = tr("  Please wait...  ");
        QRect rect = fmet.boundingRect(txt);

        rect.moveTo(20,20);
        pnt.drawRect(rect);
        pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter , txt);
    }
}
//------------------------------------------------------------------
time_t Terrain::getCurrentDate()
{
	switch (currentFileType) {
		case DATATYPE_GRIB :
		case DATATYPE_MBLUE :
			if (griddedPlot && griddedPlot->isReaderOk() )
				return griddedPlot->getCurrentDate();
			break;
		case DATATYPE_IAC :
			DBGS("TODO: get date IAC")
			return 0;
			break;
		default :
			return 0;
	}
	return 0;
}
//------------------------------------------------------------------------
QPixmap * Terrain::createPixmap (time_t date, int width, int height)
{
	Projection *scaledproj = proj->clone(); 
	MapDrawer  *scaleddrawer = new MapDrawer (*drawer);
	QPixmap *pixmap = NULL;
	
	if (scaledproj && scaleddrawer)
	{
		double x0,y0, x1,y1;
		scaledproj->getVisibleArea (&x0,&y0, &x1,&y1);
		scaledproj->setScreenSize  (width, height);
		scaledproj->setVisibleArea (x0,y0, x1,y1);
		switch (currentFileType) {
			case DATATYPE_GRIB :
			case DATATYPE_MBLUE :
				if (griddedPlot && griddedPlot->isReaderOk() )
				{
					pixmap = scaleddrawer->createPixmap_GriddedData ( 
										date, 
										false, 
										griddedPlot, 
										scaledproj, 
										getListPOIs() );
				}
				break;
			case DATATYPE_IAC :
				if (iacPlot && iacPlot->isReaderOk() )
				{
					DBGS("TODO: save image IAC")
				}
				break;
			default :	// draw only map
				pixmap = scaleddrawer->createPixmap_GriddedData ( 
									date, 
									false, 
									NULL, 
									scaledproj, 
									getListPOIs() );
		}
		delete scaledproj;
		delete scaleddrawer;
	}
	else {
		DBG("Memory problem... ");
	}
	return pixmap;
}

//---------------------------------------------------------
void Terrain::slot_Zoom_In()
{
    proj->zoom(1.3);
    setProjection(proj);
}
void Terrain::slot_Zoom_Out()
{
    proj->zoom(0.7);
    setProjection(proj);
}
//---------------------------------------------------------
void Terrain::slot_Zoom_Sel()
{
    double x0, y0, x1, y1;
    if (getSelectedRectangle(&x0,&y0, &x1,&y1))
    {
        // zoom sur la zone sélectionnée
        proj->setVisibleArea(x0,y0, x1,y1);
        isSelectionZoneEnCours = false;
        selX0 = selY0 = 0;
        selX1 = selY1 = 0;
    }
    else {
        indicateWaitingMap();
        selX0 = selY0 = 0;
        selX1 = selY1 = 0;
        isSelectionZoneEnCours = false;
        isEarthMapValid = false;
		mustRedraw = true;
        zoomOnFileZone();
        update();
    }
	setProjection(proj);
}
//---------------------------------------------------------
void Terrain::slot_Zoom_All()
{
	proj->setVisibleAreaEarth();
    setProjection(proj);
}
//------------------------------------------------
void Terrain::slot_Go_Left()
{
    proj->move( 0.25, 0);
    setProjection(proj);
}
void Terrain::slot_Go_Right()
{
    proj->move(-0.25, 0);
    setProjection(proj);
}
void Terrain::slot_Go_Up()
{
    proj->move(0,  -0.25);
    setProjection(proj);
}
void Terrain::slot_Go_Down()
{
    proj->move(0,  0.25);
    setProjection(proj);
}

//---------------------------------------------------------
void Terrain::setShowPOIs(bool show)
{
	Util::setSetting("showPOIs", show);
	// list of all the POI's
	QList<POI*> lpois = getListPOIs();
	for (int i=0; i<lpois.size(); i++)
	{
		if (show)
			lpois.at(i)->setVisible(true);
		else
			lpois.at(i)->setVisible(false);
	}
}

//-------------------------------------------------------
void Terrain::setGeopotentialData (const DataCode &dtc)
{
//	griddedPlot->getReader()->hasData (dtc);
    if (drawer->getGeopotentialData() != dtc) {
		Util::setSetting ("geopotentialLinesData", DataCodeStr::serialize(dtc));
        drawer->setGeopotentialData (dtc);
        mustRedraw = true;
        update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawGeopotential (bool b)
{
    if (drawer->showGeopotential != b) {
		drawer->showGeopotential = b;
		Util::setSetting ("drawGeopotentialLines", b);
		mustRedraw = true;
		update();
    }
}
//-------------------------------------------------------
void Terrain::setDrawGeopotentialLabels (bool b)
{
    if (drawer->showGeopotentialLabels != b) {
		drawer->showGeopotentialLabels = b;
		Util::setSetting ("drawGeopotentialLinesLabels", b);
		mustRedraw = true;
		update(); 
    }
}
//-------------------------------------------------------
void Terrain::setGeopotentialStep (int step)
{
    if (drawer->geopotentialStep != step)  {
		drawer->geopotentialStep = step;
		Util::setSetting ("drawGeopotentialLinesStep", step);
		mustRedraw = true;
		update();
    }
}
//-------------------------------------------------------
void Terrain::setMBlueFastInterpolation (bool b)
{
	if (b != fastInterpolation_MBlue) {
		Util::setSetting ("MBfastInterpolation", b);
		fastInterpolation_MBlue = b;
		if (griddedPlot) {
			griddedPlot->setFastInterpolation (fastInterpolation_MBlue);
			mustRedraw = true;
			update();
		}
	}
}
//-------------------------------------------------------
DataCode Terrain::getColorMapData ()
{ 
	DataCode dtc = drawer->getColorMapData();
	if (dtc.dataType == GRB_PRV_WIND_XY2D) {
		if (Util::getSetting("useJetStreamColorMap", false).toBool()) {
			dtc.dataType = GRB_PRV_WIND_JET;
		}
	}
	return dtc; 
}

