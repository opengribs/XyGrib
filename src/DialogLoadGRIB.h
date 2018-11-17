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

#ifndef DIALOGLOAD_GRIB_H
#define DIALOGLOAD_GRIB_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>

#include "DataDefines.h"
#include "DialogBoxBase.h"
#include "FileLoaderGRIB.h"

class DialogLoadGRIB : public DialogBoxBase
{ Q_OBJECT
    public:
        DialogLoadGRIB (QNetworkAccessManager *manager, QWidget *parent);
        ~DialogLoadGRIB ();
        
        void setZone (double x0, double y0, double x1, double y1);
		
		static QString getFile (QNetworkAccessManager *manager, QWidget *parent,
						double x0, double y0, double x1, double y1);

    public slots:
        void slotBtOK();
        void slotBtCancel();
        void slotBtServerStatus();
        void slotBtProxy();
        void slotGribDataReceived(QByteArray *content, QString fileName);
        void slotGribReadProgress(int step, int done, int total);
        void slotGribLoadError(const QString& error);
        void slotGribMessage(const QString& msg);
        void slotGribStartLoadData();
        void slotAtmModelSettings();
        void slotWaveModelSettings();
        void slotParameterUpdated();
        void slotAltitude_All ();
//        void slotFnmocWW3_All ();
		void slotFinished (int result);
        void slotAltSelections();
        void slotAltSkew();

    private:
        FileLoaderGRIB    *loadgrib;
		QNetworkAccessManager *networkManager;
		QString  savedFileName;
		
        bool     loadInProgress;
        QTime    timeLoad;
        QCursor  oldcursor;
        
        QDoubleSpinBox *sbNorth;
        QDoubleSpinBox *sbSouth;
        QDoubleSpinBox *sbWest;
        QDoubleSpinBox *sbEast;
        
        QComboBox *cbResolution;
        QComboBox *cbModel;
        QComboBox *cbWvModel;
        QComboBox *cbInterval;
        QComboBox *cbDays;
        QComboBox *cbRunCycle;

        QCheckBox *chkWind;
        QCheckBox *chkPressure;
        QCheckBox *chkRain;
        QCheckBox *chkCloud;
        QCheckBox *chkCloudLayers;
        QCheckBox *chkTemp;
        QCheckBox *chkHumid;
        QCheckBox *chkIsotherm0;
//        QCheckBox *chkTempMin;
//        QCheckBox *chkTempMax;
        QCheckBox *chkSnowCateg;
        QCheckBox *chkFrzRainCateg;
        QCheckBox *chkSnowDepth;
        QCheckBox *chkCAPEsfc;
        QCheckBox *chkCINsfc;
        QCheckBox *chkReflectivity;
        QCheckBox *chkGUSTsfc;
//        QCheckBox *chkSUNSDsfc;
		
        QCheckBox *chkAltitude_SkewT;
        QCheckBox *chkAltitude_All;
        QCheckBox *chkAltitude200;
        QCheckBox *chkAltitude300;
        QCheckBox *chkAltitude400;
        QCheckBox *chkAltitude500;
        QCheckBox *chkAltitude600;
        QCheckBox *chkAltitude700;
        QCheckBox *chkAltitude850;
        QCheckBox *chkAltitude925;
		
//        QCheckBox *chkWindAll;
        QCheckBox *chkWaveSig;
//        QCheckBox *chkWaveMax;
        QCheckBox *chkWaveSwell;
        QCheckBox *chkWaveWind;
        
        QPushButton *btOK;
        QPushButton *btCancel;
        QPushButton *btServerStatus;
        QPushButton *btProxy;

        QProgressBar *progressBar;
        QLabel       *labelMsg;
        
        QFrame *createFrameButtonsZone(QWidget *parent);
        
        QString atmosphericModel;
        QString waveModel;
        double   xmin,ymin,xmax,ymax,resolution;
        int     interval,days;
        QString cycle;
        
        bool    rain, cloud, pressure, wind, temp, humid, isotherm0;
//        bool	tempMin, tempMax, snowDepth, snowCateg, frzRainCateg;
        bool	snowDepth, snowCateg, frzRainCateg;
        bool 	CAPEsfc, CINsfc, cloudLayers;
        bool 	reflectivity, GUSTsfc;
//		bool 	SUNSDsfc;
		
        void    updateParameters();
		void    saveParametersSettings ();
        void    setWaveSelectors();

        void    addSeparator (QLayout *layout, char orientation);	// 'H' or 'V'
        QFrame *newSeparator (char orientation);	// 'H' or 'V'
//        QString createStringParameters ();
                
        void    closeEvent (QCloseEvent *event);
};


#endif
