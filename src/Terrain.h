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

#ifndef TERRAIN_H
#define TERRAIN_H

#include <QWidget>
#include <QToolBar>
#include <QBitmap>
#include <QTimer>
#include <QPen>

#include "GshhsReader.h"
#include "GisReader.h"
#include "Projection.h"
#include "POI.h"

#include "MapDrawer.h"
#include "GribPlot.h"
#include "Grib2Plot.h"
#include "IacPlot.h"
#include "MbluePlot.h"
#include "LongTaskProgress.h"


//==============================================================================
class Terrain : public QWidget
{
    Q_OBJECT

public:
    Terrain (QWidget *parent, Projection *proj, GshhsReader *gshhsReader);

	void    setCurrentDate (time_t t);
	time_t  getCurrentDate ();
    
    MapDrawer   *getDrawer()      {return drawer;}
    Projection  *getProjection()  {return proj;}
    
    FileDataType  loadMeteoDataFile (QString fileName, bool zoom);
	FileDataType  getMeteoFileType()  {return currentFileType;}

	void  closeMeteoDataFile();

	//--------------------------------------------------------
	GriddedPlotter  *getGriddedPlotter ();
	IacPlot     *getIacPlot ()    {return iacPlot;}
	//--------------------------------------------------------
	
    void  indicateWaitingMap();    // Affiche un message d'attente
    
    bool  isSelectingZone()      {return isSelectionZoneEnCours;}
    
    void  getLastMousePosition (int *x, int *y) { *x = lastMouseX;
												  *y = lastMouseY; }
	
    void  zoomOnZone      (double x0, double y0, double x1, double y1);
    void  setSpecialZone  (double x0, double y0, double x1, double y1);
	void  showSpecialZone (bool b);
    
    
    bool  getSelectedRectangle (double *x0, double *y0, double *x1, double *y1);
    bool  getSelectedLine      (double *x0, double *y0, double *x1, double *y1);
    bool  getGribFileRectangle (double *x0, double *y0, double *x1, double *y1);
    
	QList<POI*> getListPOIs() { return findChildren <POI*>(); }
	
	void     setColorMapData (const DataCode &dtc);
	DataCode getColorMapData ();
					
	QPixmap * createPixmap (time_t date, int width, int height);
    
public slots :
    // Map
    void setProjection (Projection *);
    void setDrawRivers (bool);
    void setDrawLonLatGrid (bool);
    void setDrawCountriesBorders (bool);
    void setDrawOrthodromie (bool);
    void setCountriesNames  (bool);
    void slotTemperatureLabels (bool);
    void setMapQuality (int q);
	void setMBlueFastInterpolation (bool b);
	
    void slot_Zoom_In  ();
    void slot_Zoom_Out ();
    void slot_Zoom_Sel ();
    void slot_Zoom_All ();
    void slot_Go_Left  ();
    void slot_Go_Right ();
    void slot_Go_Up   ();
    void slot_Go_Down ();
    
    void setShowPOIs (bool);
    void updateGraphicsParameters ();
    
    void setColorMapSmooth 	  (bool);
    void setDuplicateFirstCumulativeRecord 	  (bool);
    void setDuplicateMissingWaveRecords 	  (bool);
    void setInterpolateValues 	  (bool);
    void setWindArrowsOnGribGrid  (bool);
    void setDrawWindArrows    (bool);
    void setCurrentArrowsOnGribGrid  (bool);
    void setDrawCurrentArrows    (bool);
    void setBarbules          (bool);
    void setThinArrows        (bool);

    void setGribGrid          (bool);
    void setCitiesNamesLevel  (int level);
	void setWaveArrowsType    (int type);
    
    void setDrawIsobars       (bool);
    void setDrawIsobarsLabels (bool);    
    void setIsobarsStep		  (double step);
    void setPressureMinMax    (bool);

	void setGeopotentialData	   (const DataCode &dtc);
    void setDrawGeopotential       (bool);
    void setDrawGeopotentialLabels (bool);    
    void setGeopotentialStep       (int);    
	
    void setDrawIsotherms0       (bool);
    void setDrawIsotherms0Labels (bool);
    void setIsotherms0Step	     (double step);
    
    void setDrawIsotherms       (bool);
    void setDrawIsotherms_Labels (bool);
    void setIsotherms_Step	      (double step);
    void setIsotherms_Altitude   (Altitude alt);
    
    void setDrawLinesThetaE       (bool);
    void setDrawLinesThetaE_Labels (bool);
    void setLinesThetaE_Step	    (double step);
    void setLinesThetaE_Altitude   (Altitude alt);
	
    void slotTimerResize();
    void slotTimerZoomWheel();
    void slotMustRedraw();
    
signals:
    void selectionOK  (double x0, double y0, double x1, double y1);
    void mouseClicked (QMouseEvent * e);
    void mouseMoved   (QMouseEvent * e);
    void mouseLeave   (QEvent * e);


private:
	MapDrawer *drawer;
	FileDataType  currentFileType;

	//-----------------------------------------------
    Projection  *proj;
    GisReader   *gisReader;

    GriddedPlotter  *griddedPlot;
    IacPlot         *iacPlot;
    
    bool        isEarthMapValid;
    bool        mustRedraw;
    bool    	isResizing;
    bool    	firstDrawingIsDone;
    bool		fastInterpolation_MBlue;
	int 		lastMouseX, lastMouseY;
	
	LongTaskProgress *taskProgress;

    QTimer      *timerResize;
    QTimer      *timerZoomWheel;
    QCursor		myCrossCursor;
    QCursor     enterCursor;
	double 		deltaZoomWheel;
        
    void  draw_OrthodromieSegment
            (QPainter &pnt, double x0,double y0, double x1,double y1, int recurs=0);
    
    //-----------------------------------------------
    void  paintEvent(QPaintEvent *e);
    void  resizeEvent (QResizeEvent *e);
    
    void  keyPressEvent (QKeyEvent *e);
    void  keyReleaseEvent (QKeyEvent *e);
    
    void  mousePressEvent (QMouseEvent * e);
    void  mouseReleaseEvent (QMouseEvent * e);
    void  mouseMoveEvent (QMouseEvent * e);
    void  enterEvent (QEvent * e);
    void  leaveEvent (QEvent * e);
    void  wheelEvent(QWheelEvent * e) ;
    void  zoomOnFileZone();

	//-----------------------------------------------
    bool     isSelectionZoneEnCours;
    bool     isDraggingMapEnCours;
    double   selX0, selY0, selX1, selY1;   // sélection de zone (repère carte)
    double   globalX0, globalY0;		// global position of the mouse
    double   specialZoneX0, specialZoneY0, specialZoneX1, specialZoneY1;   // special zone
    bool     mustShowSpecialZone;
    
	QColor  selectColor;
	Qt::KeyboardModifiers keyModifiers;

    int     quality;
    
    //-----------------------------------------------
    // Flags indiquant les éléments à dessiner
    //-----------------------------------------------
    bool  showOrthodromie;
    bool  duplicateFirstCumulativeRecord;
	bool  duplicateMissingWaveRecords;
    bool  interpolateValues;
    bool  windArrowsOnGribGrid;
    bool  currentArrowsOnGribGrid;
	bool  drawCartouche;
    
    //-----------------------------------------------
    void  draw_Orthodromie(QPainter &painter);
	void createCrossCursor ();
    bool pleaseWait;     // long task in progress

};

#endif
