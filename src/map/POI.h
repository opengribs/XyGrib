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

#ifndef POI_H
#define POI_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QDialog>
#include <QPushButton>

#include "Projection.h"

//===================================================================
class POI : public QWidget
{ Q_OBJECT
    public:
    	friend class POI_Editor;

		// Create POI from old setting style (version<=3.3.0) : obsolete !
        POI (QString seralizedPOI_oldFormat); //
        
        POI (uint code);    // read POI from old native settings
        
        POI (uint code,     // read POI from .ini settings
				Projection *proj, QWidget *ownerSlotsPOI, QWidget *parentWindow);

        POI	(uint code, QString name, double lon, double lat,
        			Projection *proj, QWidget *ownerSlotsPOI, QWidget *parentWindow);
        
        void	writeSettings ();
        bool	isValid ()  {return valid;}
		
        void  	setProjection (Projection *proj);
        void  	drawContent (QPainter &painter, Projection *proj, bool drawingInPixmap);

        uint     getCode ()      {return code;}
        QString  getName ()      {return name;}
        double   getLongitude () {return lon;}
        double   getLatitude ()  {return lat;}

        void setName      (QString name);
        void setLongitude (double lon) {this->lon=lon;}
        void setLatitude  (double lat) {this->lat=lat;}
        
		QString serialize ();

		void setDisplayParams ( QColor markColor,
							    QFont  labelFont,
								QColor textColor,
								QColor bgColor     );
		// Restore background color for all selected POIs, TH20110103
		static void restoreBgOfSelectedPOIs( void );

	public slots:
		void projectionUpdated ();
		void timerClickEvent ();
	
	signals:
		void signalOpenMeteotablePOI (POI *poi);
		void signalPOImoved (POI *poi);
    
    private:
        void	readSettings (uint code, bool fromNativeOldSettings);
    	
    	bool	 valid;
    	unsigned int code;
    	QString      name;
		double       lon, lat;	 // Position in world map (degrees)
		int       	 pi, pj;	 // Position in screen (pixels)
		int			 hpx, hpy;	 // Hot point offset in widget (pixels)
		int			 xLabel;
		
		Projection   *proj;
    	QCursor   enterCursor;
		QWidget   *parent;
		
	    QColor    markColor;
		QColor    labelBgColor, labelTextColor, labelBgColorMarkedPOI; // need backup var for selected POIs bg-color, TH20110103
	    QFont 	  labelFont;
	    
	    int		  textHeight;
	    QWidget	  *widgetFocus;
	    
	    bool	showLabel;
	    bool	isMovable, moveInCourse;
		int		xMouse, yMouse; 	// Relative mouse  position when moving
		double  lastLon, lastLat;	// old position when moving, for cancel move
	    
	    void  createWidget(QWidget *ownerSlotsPOI);
	    
	    void  paintEvent (QPaintEvent *event);
		void  enterEvent (QEvent * e);
		void  leaveEvent (QEvent * e);
		
		int   countClick;
		void  mousePressEvent(QMouseEvent * e);
		void  mouseReleaseEvent(QMouseEvent * e);
		void  mouseMoveEvent(QMouseEvent * e);

		void  keyPressEvent (QKeyEvent *e);
		void  keyReleaseEvent (QKeyEvent *e);

		void  adjustGeometry();
};

//----------------------------------------------
// added by Tim Holtschneider
// list storing selected POIs
extern QList<POI*> GLOB_listSelectedPOI;

#endif
