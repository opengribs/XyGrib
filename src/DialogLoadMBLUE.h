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

#ifndef DIALOGLOAD_MBLUE_H
#define DIALOGLOAD_MBLUE_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>

#include "DialogBoxBase.h"
#include "FileLoaderMBLUE.h"
#include "MblueReader.h"

class DialogLoadMBLUE : public DialogBoxBase
{ Q_OBJECT
    public:
        DialogLoadMBLUE (MeteoblueZone zone, 
						 QNetworkAccessManager *manager, QWidget *parent);
		
        ~DialogLoadMBLUE ();
        
        void setZone(double x0, double y0, double x1, double y1);
		
		static QString getFile (MeteoblueZone zone, 
								QNetworkAccessManager *manager, QWidget *parent,
								double x0, double y0, double x1, double y1);

    public slots:
        void slotBtOK();
        void slotBtCancel();
        void slotBtServerStatus();
        void slotBtProxy();
        void slotGribDataReceived(QByteArray *content, QString fileName);
        void slotGribReadProgress(int step, int done, int total);
        void slotGribFileError(QString error);
        void slotGribMessage(QString msg);
        void slotGribStartLoadData();
        void slotParameterUpdated();
        void slotAltitudeAll();
		void slotFinished (int result);

    private:
        FileLoaderMBLUE   *loadmblue;
		QNetworkAccessManager *networkManager;
		MeteoblueZone meteoblueZone;
		QString  savedFileName;
		
		bool  testSelectedZoneInMeteoblueZone ();
		
		bool     loadInProgress;
        QTime    timeLoad;
        QCursor  oldcursor;
        
        QDoubleSpinBox *sbNorth;
        QDoubleSpinBox *sbSouth;
        QDoubleSpinBox *sbWest;
        QDoubleSpinBox *sbEast;
        
        QComboBox *cbResolution;
        QComboBox *cbInterval;
        QComboBox *cbDays;

        QCheckBox *chkWind;
        QCheckBox *chkPressure;
        QCheckBox *chkRain;
        QCheckBox *chkCloud;
        QCheckBox *chkTemp;
        QCheckBox *chkHumid;
        QCheckBox *chkIsotherm0;
        QCheckBox *chkTempMin;
        QCheckBox *chkTempMax;
        QCheckBox *chkSnowCateg;
        QCheckBox *chkFrzRainCateg;
        QCheckBox *chkSnowDepth;
        QCheckBox *chkCAPEsfc;
        QCheckBox *chkGUSTsfc;
		
        QCheckBox *chkAltitudeAll;
        QCheckBox *chkAltitude200;
        QCheckBox *chkAltitude300;
        QCheckBox *chkAltitude500;
        QCheckBox *chkAltitude700;
        QCheckBox *chkAltitude850;
        
        QPushButton *btOK;
        QPushButton *btCancel;
        QPushButton *btProxy;
        QPushButton *btServerStatus;

        QProgressBar *progressBar;
        QLabel       *labelMsg;
        
        QFrame *createFrameButtonsZone(QWidget *parent);
        
        double   xmin,ymin,xmax,ymax,resolution;
        int     interval,days;
        
        bool    rain, cloud, pressure, wind, temp, humid, isotherm0;
        bool	tempMin, tempMax, snowDepth, snowCateg, frzRainCateg;
        bool 	CAPEsfc;
		bool 	GUSTsfc;
		
        void    updateParameters();
        void    addSeparator (QLayout *layout, char orientation);	// 'H' or 'V'
                                
        void    closeEvent (QCloseEvent *event);
};


#endif
