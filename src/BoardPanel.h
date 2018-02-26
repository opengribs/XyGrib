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

#ifndef BOARDTOOLBAR_H
#define BOARDTOOLBAR_H

#include <QApplication>
#include <QToolBar>
#include <QScrollBar>
#include <QScrollArea>
#include <QLabel>
#include <QHBoxLayout>
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QDockWidget>

#include "DataPointInfo.h"
#include "Util.h"


//----------------------------------------------------------
class BoardPanelCell : public QFrame
{
	public:
		BoardPanelCell (QWidget *parent, QLayout *lay);
		
		void setVisible (bool b) {visible=b; QFrame::setVisible(b);}
		bool isVisible ()        {return visible;}
		
	private:
		bool visible;
};

//----------------------------------------------------------
class BoardPanel : public QDockWidget
{ Q_OBJECT
	public:
		BoardPanel(QWidget *parent);
		
		void showDataPointInfo (const DataPointInfo &pf, const Altitude &windAlt);
		void showPosition (double x, double y);
		void clearPosition ();
		void updateLabelsSizes ();

	public slots:
		void actionTriggered (bool b);
		
	//------------------------------------------------------------------------
	private:        
		void     createInterface ();
		void     addWidget (QWidget *widget);
		
		void mouseReleaseEvent (QMouseEvent *);
		void createMenuPopup ();
		void addAction (QString title,	QString settingName, 
							BoardPanelCell *cell);
		
		QHash <QAction*,QString>  hashActionSettingName;
		QHash <QAction*,BoardPanelCell*>  hashActionBoardPanelCell;
		
		QHBoxLayout * newQHBoxLayout ();
		QVBoxLayout * newQVBoxLayout ();
		
		QWidget    *mainFrame;
		QVBoxLayout *mainFrameLay;
		QScrollArea *scrollarea;
		
		BoardPanelCell *cellPosition;
		BoardPanelCell *cellWind;
		BoardPanelCell *cellCurrent;
		BoardPanelCell *cellGust;
		BoardPanelCell *cellPressure;
		BoardPanelCell *cellTemp;
		BoardPanelCell *cellDewPoint;
		BoardPanelCell *cellTempMinMax;
		BoardPanelCell *cellRain;
		BoardPanelCell *cellClouds;
		BoardPanelCell *cellHumidRel;
		BoardPanelCell *cellIsotherm0;
		BoardPanelCell *cellSnowDepth;
		BoardPanelCell *cellCAPECIN;
		BoardPanelCell *cellAltitude;
		BoardPanelCell *cellWaves;

		QMenu    *menuPopup;
		
		QLabel     lbLon;
		QLabel     lbLat;
		QLabel     lbPres;
		QLabel     lbTemp;
		QLabel     lbTempMin;
		QLabel     lbTempMax;
		QLabel     lbWindDir;
		QLabel     lbWindBf;
		QLabel     lbWindSpeed;
		QLabel     lbCurrentDir;
		QLabel     lbCurrentSpeed;
		QLabel     lbRain;
		QLabel     lbCloud;
		QLabel     lbHumid;
		QLabel     lbDewPoint;
		QLabel     lbIsotherm0HGT;
		QLabel     lbSnowDepth;
		QLabel     lbSnowCateg;
		QLabel     lbCAPEsfc;
		QLabel     lbCINsfc;
		QLabel     lbGUSTsfc;
		
		QLabel     lbWindTitle;
		QLabel     lbTempTitle;
		QLabel     lbAltTitle;
		QLabel     lb925;
		QLabel     lb850;
		QLabel     lb700;
		QLabel     lb600;
		QLabel     lb500;
		QLabel     lb400;
		QLabel     lb300;	
		QLabel     lb200;	
		
		QLabel     lbWaves_sig;	
		QLabel     lbWaves_max;	
		QLabel     lbWaves_swl;	
		QLabel     lbWaves_wnd;	
		QLabel     lbWaves_pri;	
		QLabel     lbWaves_sec;	
		//QLabel     lbWaves_pek;	
		QLabel     lbWaves_wcap;	
		
		QString  formatWave (QString title, float ht, float dir, float per);
		Altitude lastDefinedTempAltitude;
};

#endif
