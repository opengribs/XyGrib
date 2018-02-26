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

#include <QMessageBox>
#include <cmath>
#include <cassert>

#include "DialogLoadMBLUE.h"
#include "DialogServerStatus.h"
#include "DialogProxy.h"
#include "Util.h"

//-------------------------------------------------------------------------------
QString DialogLoadMBLUE::getFile (MeteoblueZone zone, 
						QNetworkAccessManager *manager, QWidget *parent,
						double x0, double y0, double x1, double y1)
{
	DialogLoadMBLUE dial (zone,manager,parent);
	dial.setZone (x0, y0, x1, y1);
	dial.exec ();
	return dial.savedFileName;
}

//-------------------------------------------------------------------------------
void DialogLoadMBLUE::closeEvent (QCloseEvent *)
{
    if (loadmblue) {
        loadmblue->deleteLater ();
		loadmblue = NULL;
	}
}
void DialogLoadMBLUE::slotFinished (int)
{
    if (loadmblue) {
        loadmblue->deleteLater ();
		loadmblue = NULL;
	}
}

//-------------------------------------------------------------------------------
DialogLoadMBLUE::DialogLoadMBLUE (MeteoblueZone zone, 
								  QNetworkAccessManager *manager, QWidget *parent) 
	: DialogBoxBase (parent)
{
    loadmblue = NULL;
	networkManager = manager;
	meteoblueZone = zone;
	savedFileName = "";
    oldcursor = cursor();
    
    setWindowTitle(tr("Download - Meteoblue - Swiss area")); 
    loadInProgress = false;
    QFrame * frameButtonsZone = createFrameButtonsZone(this);

    rain  = true;
    cloud = true;
    pressure = true;
    wind  = true;
    temp  = true;
    humid = true;
    isotherm0 = true;
    tempMin = true;
    tempMax = true;
    snowDepth = true;
    snowCateg = true;
    CAPEsfc = true;
    GUSTsfc = true;

    QGridLayout  *lay = new QGridLayout(this);
    assert(lay);
    lay->addWidget( frameButtonsZone,1,0, Qt::AlignLeft);

    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    connect(btServerStatus, SIGNAL(clicked()), this, SLOT(slotBtServerStatus()));
    connect(btProxy, SIGNAL(clicked()), this, SLOT(slotBtProxy()));

    //------------------------------------------------------
    connect(sbNorth, SIGNAL(valueChanged(double)),
            this,  SLOT(slotParameterUpdated()));
    connect(sbSouth, SIGNAL(valueChanged(double)),
            this,  SLOT(slotParameterUpdated()));
    connect(sbWest, SIGNAL(valueChanged(double)),
            this,  SLOT(slotParameterUpdated()));
    connect(sbEast, SIGNAL(valueChanged(double)),
            this,  SLOT(slotParameterUpdated()));
            
    connect(cbResolution, SIGNAL(activated(int)),
            this,  SLOT(slotParameterUpdated()));
    connect(cbInterval, SIGNAL(activated(int)),
            this,  SLOT(slotParameterUpdated()));
    connect(cbDays, SIGNAL(activated(int)),
            this,  SLOT(slotParameterUpdated()));
    
    connect(chkWind, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkPressure, SIGNAL(stateChanged(int)), this,  SLOT(slotParameterUpdated()));
    connect(chkRain, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkCloud, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkTemp, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkHumid, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkIsotherm0, SIGNAL(stateChanged(int)), this,  SLOT(slotParameterUpdated()));
                
    connect(chkTempMin, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkTempMax, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkSnowCateg, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkFrzRainCateg, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkSnowDepth, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkCAPEsfc, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkGUSTsfc, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
	
    connect(chkAltitude200, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkAltitude300, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkAltitude500, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkAltitude700, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkAltitude850, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkAltitudeAll, SIGNAL(stateChanged(int)), 	this,  SLOT(slotAltitudeAll()));
}

//-------------------------------------------------------------------------------
DialogLoadMBLUE::~DialogLoadMBLUE()
{
    if (loadmblue != NULL)
        delete loadmblue;
}

//----------------------------------------------------
void DialogLoadMBLUE::slotGribMessage(QString msg)
{
    labelMsg->setText(msg);
}

//----------------------------------------------------
void DialogLoadMBLUE::slotGribDataReceived(QByteArray *content, QString fileName)
{
	setCursor(oldcursor);
	QString path;
	QString pathgrib = Util::getSetting("gribFilePath", "").toString();
	QString pathmblue = Util::getSetting("MBlueFilePath", "").toString();
	if (pathmblue != "")
		path = pathmblue;
	else if (pathgrib != "")
		path = pathgrib;
	else
		path = ".";
	path += "/";
	savedFileName = "";
	QFileInfo fi (fileName);
    fileName = Util::getSaveFileName (NULL,
                 tr("Save Meteoblue file"), path+fi.fileName(), "");
    if (fileName != "")
    {
        QFile saveFile (fileName);
        bool ok;
        qint64 nb = 0;
        ok = saveFile.open(QIODevice::WriteOnly);
        if (ok) {
            nb = saveFile.write(*content);
            if (nb > 0) {
            	QFileInfo info(saveFile);
				Util::setSetting("MBlueFilePath", info.absolutePath() );
                saveFile.close();
            }
        }
        if (ok && nb>0) {
            loadInProgress = false;
            btCancel->setText(tr("Cancel"));
            btOK->setEnabled(true);
			savedFileName = fileName;
            accept();
        }
        else {
            QMessageBox::critical (this,
                    tr("Error"),
                    tr("Can't write file."));
        }
    }
    else {
        loadInProgress = false;
        btCancel->setText(tr("Cancel"));
        btOK->setEnabled(true);
        slotGribReadProgress(1,0,100);
    }
}

//----------------------------------------------------
void DialogLoadMBLUE::slotGribFileError(QString error)
{
	setCursor(oldcursor);
    if (! loadInProgress)
        return;
 //DBGQS(error);   
    QString s;
    QMessageBox::critical (this,
                    tr("Error"),
                    tr("Error: ") + error );
    
    loadInProgress = false;
    btCancel->setText(tr("Cancel"));
    btOK->setEnabled(true);
    labelMsg->setText("");
}

//----------------------------------------------------
void DialogLoadMBLUE::slotGribStartLoadData()
{
    timeLoad.start();
}

//----------------------------------------------------
void DialogLoadMBLUE::slotGribReadProgress(int step, int done, int total)
{
    if (step < 2) {
        progressBar->setRange(0,1000);
        progressBar->setValue(step);
    }
    else {
        progressBar->setRange(0,total);
        progressBar->setValue(done);

		QString speedunit = tr("ko/s");
		double speed = done/1024/(timeLoad.elapsed()/1000.0);
		if (speed > 1024) {
			speed = speed/1024;
			speedunit = tr("Mo/s");
		}

		slotGribMessage(tr("Size: %1 ko      Done: %2 ko at %3 %4")
                .arg( total/1024, 5)
                .arg( done/1024, 5)
				.arg(speed,0,'f',1)
				.arg(speedunit)
			);
    }
}

//-------------------------------------------------------------------------------
void DialogLoadMBLUE::updateParameters()
{
    double tmp, xm, ym;
    
    xmin = sbWest->cleanText().toDouble();
    xmax = sbEast->cleanText().toDouble();
    ymin = sbNorth->cleanText().toDouble();
    ymax = sbSouth->cleanText().toDouble();

    resolution = cbResolution->currentText().toDouble();
    interval   = cbInterval->currentText().toInt();
    days       = cbDays->currentText().toInt();

    if (xmin > xmax) {
        tmp = xmin;   xmin = xmax;   xmax = tmp;
    }
    if (ymin < ymax) {    // échelle Y inversée (90=nord)
        tmp = ymin;   ymin = ymax;   ymax = tmp;
    }
    // trop grand ?
    if (fabs(xmax-xmin) >=360)
        xmax = xmin+359.9;
    if (fabs(ymin-ymax) >=180)
        ymin = ymax+179.9;
    
    // trop petit ?
	double szmin = 0.2; 
    if (fabs(xmax-xmin) < szmin) {
        xm = (xmin+xmax)/2;
        xmin = xm - szmin/2;
        xmax = xm + szmin/2;
    }
    if (fabs(ymin-ymax) < szmin) {
		ym = (ymin+ymax)/2;
        ymin = ym + szmin/2;
        ymax = ym - szmin/2;
    }

    wind     = chkWind->isChecked();
    pressure = chkPressure->isChecked();
    rain     = chkRain->isChecked();
    cloud    = chkCloud->isChecked();
    temp     = chkTemp->isChecked();
    humid    = chkHumid->isChecked();
    isotherm0    = chkIsotherm0->isChecked();
	
    tempMin     = chkTempMin->isChecked();
    tempMax     = chkTempMax->isChecked();
    snowDepth   = chkSnowDepth->isChecked();
    snowCateg   = chkSnowCateg->isChecked();
    frzRainCateg = chkFrzRainCateg->isChecked();
    CAPEsfc      = chkCAPEsfc->isChecked();
    GUSTsfc      = chkGUSTsfc->isChecked();
	
	Util::setSetting("MBdownloadIndResolution", cbResolution->currentIndex());
	Util::setSetting("MBdownloadIndInterval",  cbInterval->currentIndex());
	Util::setSetting("MBdownloadIndNbDays",  cbDays->currentIndex());
	
	Util::setSetting("MBdownloadWind",  wind);
	Util::setSetting("MBdownloadPressure", pressure);
	Util::setSetting("MBdownloadRain",  rain);
	Util::setSetting("MBdownloadCloud", cloud);
	Util::setSetting("MBdownloadTemp",  temp);
	Util::setSetting("MBdownloadHumid", humid);
	Util::setSetting("MBdownloadIsotherm0", isotherm0);
	
	Util::setSetting("MBdownloadTempMin",  tempMin);
	Util::setSetting("MBdownloadTempMax",  tempMax);
	Util::setSetting("MBdownloadSnowDepth", snowDepth);
	Util::setSetting("MBdownloadSnowCateg", snowCateg);
	Util::setSetting("MBdownloadFrzRainCateg", frzRainCateg);
	Util::setSetting("MBdownloadCAPEsfc", CAPEsfc);
	Util::setSetting("MBdownloadGUSTsfc", GUSTsfc);
	
	Util::setSetting("MBdownloadAltitudeData200",  chkAltitude200->isChecked());
	Util::setSetting("MBdownloadAltitudeData300",  chkAltitude300->isChecked());
	Util::setSetting("MBdownloadAltitudeData500",  chkAltitude500->isChecked());
	Util::setSetting("MBdownloadAltitudeData700",  chkAltitude700->isChecked());
	Util::setSetting("MBdownloadAltitudeData850",  chkAltitude850->isChecked());
}

//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotParameterUpdated()
{
    updateParameters();

	int npts = (int) (ceil(fabs(xmax-xmin))*ceil(fabs(ymax-ymin))*400 );
    
    int nbrec = (int) days*24/interval +1;
	
	double nbdata = 0;
	
	nbdata += wind  ?    4.4 / 3.6 : 0;
	nbdata += GUSTsfc ?  3.4 / 5.7 : 0;
	nbdata += pressure ? 3.4 / 4.2 : 0;
	nbdata += rain  ?    3.4 / 8.0 : 0;
	nbdata += cloud ?    3.4 / 9.9 : 0;
	nbdata += temp  ?    3.4 / 3.9 : 0;
	nbdata += humid ?    3.4 / 5.3 : 0;
	nbdata += CAPEsfc ?  3.4 / 7.9 : 0;

	if (chkAltitude500->isChecked()) nbdata += 6.3 / 3.3;
	if (chkAltitude700->isChecked()) nbdata += 6.3 / 3.3;
	if (chkAltitude850->isChecked()) nbdata += 6.3 / 3.3; 
    
    int head = (nbdata>0) ? 512 : 0;
	
    double estime = head + nbdata*4*nbrec*npts; 
	
    // compress
/*    if (estime < 2000) estime /= 16;
    else if (estime < 6000) estime /= 14;
    else if (estime < 15000) estime /= 8;
    else if (estime < 100000) estime /= 6; 
    else estime /= 5;*/

//    estime /= 3;    // zip
	
    estime = estime/1024;	// ko
// DBG ("estime= %d ko", (int)estime);

	double compress;
	compress = 4.2;
	slotGribMessage(tr("Size: %1 ko approx").arg((int)(estime/compress)) );
    
    if (estime == 0)
        btOK->setEnabled(false);
    else
        btOK->setEnabled(true);
	
}
//-------------------------------------------------------------------------------
bool  DialogLoadMBLUE::testSelectedZoneInMeteoblueZone ()
{
	double x0,y0, x1,y1;
	if (xmin < xmax) {
		x0 = xmin;
		x1 = xmax;
	}
	else {
		x0 = xmax;
		x1 = xmin;
	}
	if (ymin < ymax) {
		y0 = ymin;
		y1 = ymax;
	}
	else {
		y0 = ymax;
		y1 = ymin;
	}
	double xm0,ym0, xm1,ym1;
	if (MblueReader::getMeteoblueTotalArea (meteoblueZone, &xm0,&ym0, &xm1,&ym1))
	{
		QRectF r1 (x0,y0, x1,y1);
		QRectF rm (xm0,ym0, xm1,ym1);
		
		if (r1.intersects (rm))
				return true;
	}
	
	QMessageBox::critical (this,
			tr("Error"),
			tr("The selected area doesn't contain Meteoblue data."));
	return false;
}

//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotAltitudeAll ()
{
	bool check = chkAltitudeAll->isChecked ();
	chkAltitude200->setChecked (check);
	chkAltitude300->setChecked (check);
	chkAltitude500->setChecked (check);
	chkAltitude700->setChecked (check);
	chkAltitude850->setChecked (check);
}
//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotBtOK()
{
	if (! testSelectedZoneInMeteoblueZone ()) {
		return;
	}
	
    if (loadmblue) {
        loadmblue->deleteLater ();
		loadmblue = NULL;
	}
    loadmblue = new FileLoaderMBLUE (networkManager, this);
	assert (loadmblue);
    connect(loadmblue, SIGNAL(signalGribDataReceived(QByteArray *, QString)),
            this,  SLOT(slotGribDataReceived(QByteArray *, QString)));
    connect(loadmblue, SIGNAL(signalGribReadProgress(int, int, int)),
            this,  SLOT(slotGribReadProgress(int, int, int)));
    connect(loadmblue, SIGNAL(signalGribLoadError(QString)),
            this,  SLOT(slotGribFileError(QString)));
    connect(loadmblue, SIGNAL(signalGribSendMessage(QString)),
            this,  SLOT(slotGribMessage(QString)));
    connect(loadmblue, SIGNAL(signalGribStartLoadData()),
            this,  SLOT(slotGribStartLoadData()));
            
    btCancel->setText(tr("Stop"));

	loadInProgress = true;
    btOK->setEnabled(false);
    
    setCursor(Qt::WaitCursor);
	
	slotParameterUpdated();

    loadmblue->getMblueFile(
					xmin,ymin, xmax,ymax,
					resolution, interval, days,
					wind, pressure, rain, cloud, temp, humid, isotherm0,
					tempMin, tempMax, snowDepth, snowCateg, frzRainCateg,
					CAPEsfc, 
					chkAltitude200->isChecked(),
					chkAltitude300->isChecked(),
					chkAltitude500->isChecked(),
					chkAltitude700->isChecked(),
					chkAltitude850->isChecked(),
					GUSTsfc
				);
}
//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotBtServerStatus()
{
    DialogServerStatus dial (networkManager);
    dial.exec();
}
//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotBtProxy ()
{
    DialogProxy dial (this);
    dial.exec ();
}
//-------------------------------------------------------------------------------
void DialogLoadMBLUE::slotBtCancel()
{
	setCursor(oldcursor);

    if (loadInProgress)
    {
        loadInProgress = false;
        loadmblue->stop();
        btCancel->setText(tr("Cancel"));
        progressBar->setRange(0,100);
        progressBar->setValue(0);
        slotParameterUpdated();
    }
    else {
        reject();
    }
}

//-------------------------------------------------------------------------------
void DialogLoadMBLUE::setZone (double x0, double y0, double x1, double y1)
{
	double tmp;
	if (x0 > x1)  { tmp=x0; x0=x1; x1=tmp; }
	if (y1 > y0)  { tmp=y0; y0=y1; y1=tmp; }	
    sbNorth->setValue ( ceil(y0) );
    sbSouth->setValue ( floor(y1) );
    sbWest->setValue  ( floor(x0) );
    sbEast->setValue  ( ceil (x1) );
	
    progressBar->setRange(0,100);
    progressBar->setValue(0);
    slotParameterUpdated();
	
	testSelectedZoneInMeteoblueZone ();
}

//-------------------------------------------------------------------------------
QFrame *DialogLoadMBLUE::createFrameButtonsZone(QWidget *parent)
{
    QFrame * ftmp;
    QFrame * frm = new QFrame(parent);
    assert(frm);
    QVBoxLayout  *lay = new QVBoxLayout(frm);
    assert(lay);
	lay->setContentsMargins (0,0,0,0);
	lay->setSpacing (3);
	
    int ind, lig,col;
	//------------------------------------------------
	// Geographic area
	//------------------------------------------------
    int sizemin = 0;
    sbNorth = new QDoubleSpinBox();
    assert(sbNorth);
    sbNorth->setDecimals(1);
    sbNorth->setMinimum(-90);
    sbNorth->setMaximum(90);
    sbNorth->setSuffix(tr(" °N"));
    sbNorth->setSingleStep (0.2);    
	
    sbSouth = new QDoubleSpinBox();
    assert(sbSouth);
    sbSouth->setDecimals(1);
    sbSouth->setMinimum(-90);
    sbSouth->setMaximum(90);
    sbSouth->setSuffix(tr(" °N"));
    sbSouth->setSingleStep (0.2);    
    
    sbWest = new QDoubleSpinBox();
    assert(sbWest);
    sbWest->setDecimals(1);
    sbWest->setMinimum(-360);
    sbWest->setMaximum(360);
    sbWest->setSuffix(tr(" °E"));
    sbWest->setSingleStep (0.2);    
    
    sbEast = new QDoubleSpinBox();
    assert(sbEast);
    sbEast->setDecimals(1);
    sbEast->setMinimum(-360);
    sbEast->setMaximum(360);
    sbEast->setSuffix(tr(" °E"));
    sbEast->setSingleStep (0.2);    

	//------------------------------------------------
	// Résolution, intervalle, durée
	//------------------------------------------------
    cbResolution = new QComboBox();
    assert(cbResolution);
    cbResolution->addItems(QStringList()<< "0.5" << "1" << "2");
    cbResolution->setMinimumWidth (sizemin);
	ind = Util::getSetting("MBdownloadIndResolution", 1).toInt();
	ind = Util::inRange(ind, 0, cbResolution->count()-1);
    cbResolution->setCurrentIndex(ind);
    
    cbInterval = new QComboBox();
    assert(cbInterval);
    cbInterval->addItems(QStringList()<<"1"<<"2"<<"3"<<"4"<<"6"<<"8"<<"12"<<"24");
    cbInterval->setMinimumWidth (sizemin);
	ind = Util::getSetting("MBdownloadIndInterval", 1).toInt();
	ind = Util::inRange(ind, 0, cbInterval->count()-1);
    cbInterval->setCurrentIndex(ind);
    
    cbDays = new QComboBox();
    assert(cbDays);
    cbDays->addItems(QStringList()<< "1"<<"2"<<"3");
    cbDays->setMinimumWidth (sizemin);
	ind = Util::getSetting("MBdownloadIndNbDays", 4).toInt();	
	ind = Util::inRange(ind, 0, cbDays->count()-1);
    cbDays->setCurrentIndex(ind);

	//------------------------------------------------
	// Choix des données météo
	//------------------------------------------------
    chkWind     = new QCheckBox(tr("Wind (10 m)"));
    assert(chkWind);
    chkPressure = new QCheckBox(tr("Mean sea level pressure"));
    assert(chkPressure);
    chkRain     = new QCheckBox(tr("Total precipitation"));
    assert(chkRain);
    chkCloud     = new QCheckBox(tr("Cloud cover"));
    assert(chkCloud);
    chkTemp     = new QCheckBox(tr("Temperature (2 m)"));
    assert(chkTemp);
    chkHumid    = new QCheckBox(tr("Relative humidity (2 m)"));
    assert(chkHumid);
    chkIsotherm0    = new QCheckBox(tr("Isotherm 0°C"));
    assert(chkIsotherm0);
    
    chkTempMin     = new QCheckBox(tr("Temperature min (2 m)"));
    assert(chkTempMin);
    chkTempMax     = new QCheckBox(tr("Temperature max (2 m)"));
    assert(chkTempMax);
    chkSnowCateg     = new QCheckBox(tr("Snow (snowfall possible)"));
    assert(chkSnowCateg);
    chkFrzRainCateg     = new QCheckBox(tr("Frozen rain (rainfall possible)"));
    assert(chkFrzRainCateg);
    chkSnowDepth     = new QCheckBox(tr("Snow (depth)"));
    assert(chkSnowDepth);
    chkCAPEsfc     = new QCheckBox(tr("CAPE (surface)"));
    assert(chkCAPEsfc);
    chkGUSTsfc     = new QCheckBox(tr("Wind gust"));
    assert(chkGUSTsfc);

	
    //--------------------------------------------------------------------------------
    chkWind->setChecked    (Util::getSetting("MBdownloadWind", true).toBool());
    chkPressure->setChecked(Util::getSetting("MBdownloadPressure", true).toBool());
    chkRain->setChecked    (Util::getSetting("MBdownloadRain", true).toBool());
    chkCloud->setChecked   (Util::getSetting("MBdownloadCloud", true).toBool());
    chkTemp->setChecked    (Util::getSetting("MBdownloadTemp", true).toBool());
    chkHumid->setChecked   (Util::getSetting("MBdownloadHumid", true).toBool());
    chkIsotherm0->setChecked  (Util::getSetting("MBdownloadIsotherm0", true).toBool());
    
    chkTempMin->setChecked    (Util::getSetting("MBdownloadTempMin", false).toBool());
    chkTempMax->setChecked    (Util::getSetting("MBdownloadTempMax", false).toBool());
    chkSnowDepth->setChecked  (Util::getSetting("MBdownloadSnowDepth", true).toBool());
    chkSnowCateg->setChecked  (Util::getSetting("MBdownloadSnowCateg", true).toBool());
    chkCAPEsfc->setChecked  (Util::getSetting("MBdownloadCAPEsfc", true).toBool());
    chkGUSTsfc->setChecked  (Util::getSetting("MBdownloadGUSTsfc", true).toBool());
    chkFrzRainCateg->setChecked  (Util::getSetting("MBdownloadFrzRainCateg", true).toBool());
	
	//----------------------------------------------------------------
    chkAltitude850  = new QCheckBox ("850 "+tr("hPa"));
    assert (chkAltitude850);
    chkAltitude850->setChecked  (Util::getSetting("MBdownloadAltitudeData850", false).toBool());
    chkAltitude700  = new QCheckBox ("700 "+tr("hPa"));
    assert (chkAltitude700);
    chkAltitude700->setChecked  (Util::getSetting("MBdownloadAltitudeData700", false).toBool());
    chkAltitude500  = new QCheckBox ("500 "+tr("hPa"));
    assert (chkAltitude500);
    chkAltitude500->setChecked  (Util::getSetting("MBdownloadAltitudeData500", false).toBool());
    chkAltitude300  = new QCheckBox ("300 "+tr("hPa"));
    assert (chkAltitude300);
    chkAltitude300->setChecked  (Util::getSetting("MBdownloadAltitudeData300", false).toBool());
    chkAltitude200  = new QCheckBox ("200 "+tr("hPa"));
    assert (chkAltitude200);
    chkAltitude200->setChecked  (Util::getSetting("MBdownloadAltitudeData200", false).toBool());

    chkAltitudeAll = new QCheckBox (tr("All"));
    assert (chkAltitudeAll);
	chkAltitudeAll->setChecked  (
		      chkAltitude200->isChecked() && chkAltitude300->isChecked() 
		   && chkAltitude500->isChecked() && chkAltitude700->isChecked() 
		   && chkAltitude850->isChecked()
		);

	//----------------------------------------------------------------
    btOK     = new QPushButton(tr("Download"), this);
    assert(btOK);
    btCancel = new QPushButton(tr("Cancel"), this);
    assert(btCancel);
    btServerStatus = new QPushButton(tr("Server status"), this);
    assert(btServerStatus);
    btProxy = new QPushButton(tr("Connection"), this);
    assert(btProxy);

    progressBar = new QProgressBar();
    assert(progressBar);

    QLayout  *tlay;
    QGridLayout  *tgrid;
	//------------------------------------------------
	// Disposition des widgets
	//------------------------------------------------
	
	ftmp = new QFrame(this);
    tgrid = new QGridLayout(ftmp);
    assert(tgrid);
	tgrid->setContentsMargins (0,0,0,0);
		tgrid->addWidget( new QLabel(tr("Latitude min :")), 0, 0, Qt::AlignRight);
		tgrid->addWidget( sbNorth, 0, 1);
		tgrid->addWidget( new QLabel(tr("Latitude max :")), 0, 2, Qt::AlignRight);
		tgrid->addWidget( sbSouth, 0, 3);
		
		tgrid->addWidget( new QLabel(tr("Longitude min :")), 1, 0, Qt::AlignRight);
		tgrid->addWidget( sbWest, 1, 1);
		tgrid->addWidget( new QLabel(tr("Longitude max :")), 1, 2, Qt::AlignRight);
		tgrid->addWidget( sbEast, 1, 3);

    lay->addWidget( ftmp);
    
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
	
	ftmp = new QFrame(this);
    tlay = new QHBoxLayout(ftmp);
    assert(tlay);
	tlay->setContentsMargins (0,0,0,0);
		
/*		tlay->addWidget( new QLabel(tr("Resolution :")));
		tlay->addWidget( cbResolution);
		tlay->addWidget( new QLabel(tr(" °")));
		//-------------------------
		addSeparator (tlay, 'V'); */
		//-------------------------
		tlay->addWidget( new QLabel(tr("Interval :")));
		tlay->addWidget( cbInterval);
		tlay->addWidget( new QLabel(tr(" hours")));
		//-------------------------
		addSeparator (tlay, 'V'); 
		//-------------------------
		tlay->addWidget( new QLabel(tr("Period :")));
		tlay->addWidget( cbDays);
		tlay->addWidget( new QLabel(tr(" days")));
    lay->addWidget( ftmp);
    
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
	ftmp = new QFrame(this);
    tgrid = new QGridLayout(ftmp);
    assert(tgrid);
	tgrid->setContentsMargins (0,0,0,0);
	tgrid->setSpacing (0);
    	//-----------------------------
    	// Colonne 1
    	//-----------------------------
    	col = 0;
    	lig = 0;
		tgrid->addWidget( chkWind ,       lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkPressure ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkTemp ,      lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkTempMin ,   lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkTempMax ,   lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkIsotherm0 , lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkCAPEsfc ,  lig++, col, Qt::AlignLeft);
    	//-----------------------------
    	// Colonne 2
    	//-----------------------------
    	col = 1;
    	lig = 0;
		tgrid->addWidget( chkGUSTsfc ,  lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkCloud ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkHumid ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkRain ,    lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkSnowCateg ,  lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkSnowDepth ,  lig++, col, Qt::AlignLeft);
		//tgrid->addWidget( chkFrzRainCateg , lig++, col, Qt::AlignLeft);
    lay->addWidget( ftmp);
    
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
	lay->addWidget (new QLabel (
			tr("Atmosphere: wind, temperature, relative humidity.") 
			//+"\n"+
			//tr("Warning : these data increase strongly the size of the GRIB file.")
		));
	//lay->addWidget( chkAltitudeData );
	ftmp = new QFrame(this);
	assert (ftmp);
		tlay = new QHBoxLayout(ftmp);
		assert (tlay);
		tlay->setContentsMargins (0,0,0,0);
		tlay->addWidget (chkAltitude850);
		tlay->addWidget (chkAltitude700);
		tlay->addWidget (chkAltitude500);
		//tlay->addWidget (chkAltitude300);
		//tlay->addWidget (chkAltitude200);
		addSeparator (tlay, 'V'); 
		addSeparator (tlay, 'V'); 
		tlay->addWidget (chkAltitudeAll);
	lay->addWidget (ftmp);
	
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
    lay->addWidget( progressBar );
    //-------------------------
    labelMsg = new QLabel();
    lay->addWidget( labelMsg );
    //lay->addWidget(new QLabel(tr("File size max: 20000 ko.")));
    lay->addWidget(new QLabel(tr("File size max: ")+ "102400 Ko (100 Mo)"));
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
	ftmp = new QFrame(this);
    tlay = new QHBoxLayout(ftmp);
    assert(tlay);
	tlay->setContentsMargins (0,0,0,0);
		tlay->addWidget (btOK);
		tlay->addWidget (btServerStatus);
		tlay->addWidget (btProxy);
		tlay->addWidget (btCancel);
    lay->addWidget( ftmp);

    return frm;
}
//----------------------------------------------------------------------------
void DialogLoadMBLUE::addSeparator (QLayout *layout, char orientation)
{
	QFrame *ftmp;
	ftmp = new QFrame ();
	assert (ftmp);
	if (orientation == 'H') {
		ftmp->setFrameShape(QFrame::HLine);
	}
	else {
		ftmp->setFrameShape(QFrame::VLine);
	}
	ftmp->setStyleSheet ("color:#AAAAAA");
	layout->addWidget(ftmp);
}









