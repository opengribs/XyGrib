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

#include <cmath>
#include <cassert>

#include <QMessageBox>
#include <QTabWidget>

#include "DialogLoadGRIB.h"
#include "DialogServerStatus.h"
#include "DialogProxy.h"
#include "Util.h"

DialogLoadGRIB *globalDial = NULL;

//-------------------------------------------------------------------------------
QString DialogLoadGRIB::getFile (QNetworkAccessManager *netManager, QWidget *parent,
						double x0, double y0, double x1, double y1)
{
	if (!globalDial)
    {
        qDebug() << "creating first globalDial";
		globalDial = new DialogLoadGRIB(netManager,parent);
        globalDial->slotAtmModelSettings();
        globalDial->slotWaveModelSettings();
    }
	globalDial->setZone (x0, y0, x1, y1);
	globalDial->exec ();
	globalDial->saveParametersSettings ();
	return globalDial->savedFileName;
}

//-------------------------------------------------------------------------------
void DialogLoadGRIB::closeEvent (QCloseEvent *)
{
    if (loadgrib) {
        loadgrib->deleteLater ();
		loadgrib = NULL;
	}
}
void DialogLoadGRIB::slotFinished (int)
{
    if (loadgrib) {
        loadgrib->deleteLater ();
		loadgrib = NULL;
	}
}

//-------------------------------------------------------------------------------
DialogLoadGRIB::DialogLoadGRIB (QNetworkAccessManager *netManager, QWidget *parent)
	: DialogBoxBase (parent)
{
    oldcursor = cursor();
    loadgrib = NULL;
	networkManager = netManager;
	savedFileName = "";
    setWindowTitle(tr("Download - GRIB"));
    loadInProgress = false;
    QFrame * frameButtonsZone = createFrameButtonsZone(this);

    rain  = true;
    cloud = true;
    pressure = true;
    wind  = true;
    temp  = true;
    humid = true;
    isotherm0 = true;
//    tempMin = true;
//    tempMax = true;
    snowDepth = true;
    snowCateg = true;
    CAPEsfc = true;
    CINsfc = true;
    GUSTsfc = true;
//    SUNSDsfc = false;

    QGridLayout  *lay = new QGridLayout(this);
    assert(lay);
    lay->addWidget( frameButtonsZone,1,0, Qt::AlignLeft);
    connect(this, SIGNAL(finished(int)), this, SLOT(slotFinished(int)));

    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    connect(btServerStatus, SIGNAL(clicked()), this, SLOT(slotBtServerStatus()));
    connect(btProxy, SIGNAL(clicked()), this, SLOT(slotBtProxy()));

    //------------------------------------------------------
    connect(sbNorth, SIGNAL(valueChanged(double)), this,  SLOT(slotParameterUpdated()));
    connect(sbSouth, SIGNAL(valueChanged(double)), this,  SLOT(slotParameterUpdated()));
    connect(sbWest, SIGNAL(valueChanged(double)), this,  SLOT(slotParameterUpdated()));
    connect(sbEast, SIGNAL(valueChanged(double)), this,  SLOT(slotParameterUpdated()));

    connect(cbModel, SIGNAL(activated(int)), this,  SLOT(slotAtmModelSettings()));
    connect(cbWvModel, SIGNAL(activated(int)), this,  SLOT(slotWaveModelSettings()));

    connect(cbResolution, SIGNAL(activated(int)), this,  SLOT(slotParameterUpdated()));
    connect(cbInterval, SIGNAL(activated(int)), this,  SLOT(slotParameterUpdated()));
    connect(cbDays, SIGNAL(activated(int)), this,  SLOT(slotParameterUpdated()));
    connect(cbRunCycle, SIGNAL(activated(int)), this,  SLOT(slotParameterUpdated()));
    
    connect(chkWind, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkPressure, SIGNAL(stateChanged(int)), this,  SLOT(slotParameterUpdated()));
    connect(chkRain, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkCloud, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
	// connect(chkCloudLayers, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkTemp, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkHumid, SIGNAL(stateChanged(int)), 	this,  SLOT(slotParameterUpdated()));
    connect(chkIsotherm0, SIGNAL(stateChanged(int)), this,  SLOT(slotParameterUpdated()));
                
//    connect(chkTempMin, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
//    connect(chkTempMax, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
    connect(chkSnowCateg, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
    connect(chkFrzRainCateg, SIGNAL(stateChanged(int)), this, SLOT(slotParameterUpdated()));
    connect(chkSnowDepth, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
    connect(chkCAPEsfc, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
    connect(chkCINsfc, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
    connect(chkGUSTsfc, SIGNAL(stateChanged(int)), 	this, SLOT(slotParameterUpdated()));
	
    connect(chkAltitude_All, SIGNAL(stateChanged(int)), this, SLOT(slotAltitude_All()));
    connect(chkAltitude200, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude300, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude400, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude500, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude600, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude700, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude850, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude925, SIGNAL(stateChanged(int)), this, SLOT(slotAltSelections()));
    connect(chkAltitude_SkewT, SIGNAL(stateChanged(int)), this, SLOT(slotAltSkew()));
	
    connect(chkWaveSig, SIGNAL(stateChanged(int)),this, SLOT(slotParameterUpdated()));
    connect(chkWaveSwell, SIGNAL(stateChanged(int)),this, SLOT(slotParameterUpdated()));
    connect(chkWaveWind, SIGNAL(stateChanged(int)),this, SLOT(slotParameterUpdated()));
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotAltitude_All ()
{
	bool check = chkAltitude_All->isChecked ();
	chkAltitude200->setChecked (check);
	chkAltitude300->setChecked (check);
	chkAltitude400->setChecked (check);
	chkAltitude500->setChecked (check);
	chkAltitude600->setChecked (check);
	chkAltitude700->setChecked (check);
	chkAltitude850->setChecked (check);
	chkAltitude925->setChecked (check);
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotAltSelections()
// individual altitude selection boxes need to be mutually exclusive
// with skewt selection box
{
     if ( chkAltitude200->isChecked()
          || chkAltitude300->isChecked()
          || chkAltitude400->isChecked()
          || chkAltitude500->isChecked()
          || chkAltitude600->isChecked()
          || chkAltitude700->isChecked()
          || chkAltitude850->isChecked()
          || chkAltitude925->isChecked()
             )
    {
        chkAltitude_SkewT->setChecked(false);
    }
     slotParameterUpdated();

}
void DialogLoadGRIB::slotAltSkew()
// individual altitude selection boxes need to be mutually exclusive
// with skewt selection box
{
    if (chkAltitude_SkewT->isChecked())
    {
        chkAltitude200->setChecked(false);
        chkAltitude300->setChecked(false);
        chkAltitude400->setChecked(false);
        chkAltitude500->setChecked(false);
        chkAltitude600->setChecked(false);
        chkAltitude700->setChecked(false);
        chkAltitude850->setChecked(false);
        chkAltitude925->setChecked(false);
        chkAltitude_All->setChecked(false);
    }
    slotParameterUpdated();

}
//void DialogLoadGRIB::slotFnmocWW3_All ()
//{
//    bool check = chkWindAll->isChecked ();
//    chkWaveSig->setChecked (check);
//    chkWaveMax->setChecked (check);
//    chkWaveSwell->setChecked (check);
//    chkWaveWind->setChecked (check);
//	chkFnmocWW3_prim->setChecked (check);
//	chkFnmocWW3_scdy->setChecked (check);
//	chkFnmocWW3_wcap->setChecked (check);
//}

//-------------------------------------------------------------------------------
DialogLoadGRIB::~DialogLoadGRIB()
{
}

//----------------------------------------------------
void DialogLoadGRIB::slotGribMessage(QString msg)
{
    labelMsg->setText(msg);
}

//----------------------------------------------------
void DialogLoadGRIB::slotGribDataReceived (QByteArray *content, QString fileName)
{
	setCursor(oldcursor);
	QString path = Util::getSetting("gribFilePath", "").toString();
	if (path == "")
		path = "./";
	else
		path += "/";
	savedFileName = "";
//    QFileInfo fi (fileName + ".bz2");
    QFileInfo fi (fileName);
    fileName = Util::getSaveFileName (NULL,
                 tr("Save GRIB file"), path+fi.fileName());
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
				Util::setSetting("gribFilePath", info.absolutePath() );
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
void DialogLoadGRIB::slotGribFileError (QString error)
{
	setCursor(oldcursor);
    if (! loadInProgress)
        return;
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
void DialogLoadGRIB::slotGribStartLoadData()
{
    timeLoad.start();
}

//----------------------------------------------------
void DialogLoadGRIB::slotGribReadProgress(int step, int done, int total)
{
    if (step < 2) {
        progressBar->setRange(0,1000);
        progressBar->setValue(step);
    }
    else {
        progressBar->setRange(0,total);
        progressBar->setValue(done);

        QString speedunit = tr("kb/s");
		double speed = done/1024/(timeLoad.elapsed()/1000.0);
		if (speed > 1024) {
			speed = speed/1024;
            speedunit = tr("Mb/s");
		}

        slotGribMessage(tr("Size: %1 kb      Done: %2 ko at %3 %4")
                .arg( total/1024, 5)
                .arg( done/1024, 5)
				.arg(speed,0,'f',1)
				.arg(speedunit)
			);
    }
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::saveParametersSettings ()
{
    Util::setSetting("downloadIndModel", cbModel->currentIndex());
    Util::setSetting("downloadIndWaveModel", cbWvModel->currentIndex());

    Util::setSetting("downloadIndResolution", cbResolution->currentIndex());
    Util::setSetting("downloadIndInterval",  cbInterval->currentIndex());
	Util::setSetting("downloadIndNbDays",  cbDays->currentIndex());
    Util::setSetting("downloadRunCycle",  cbRunCycle->itemData(cbRunCycle->currentIndex()));
	
	Util::setSetting("downloadWind",  wind);
	Util::setSetting("downloadPressure", pressure);
	Util::setSetting("downloadRain",  rain);
	Util::setSetting("downloadCloud", cloud);
	Util::setSetting("downloadCloudLayers", cloudLayers);
	Util::setSetting("downloadTemp",  temp);
	Util::setSetting("downloadHumid", humid);
	Util::setSetting("downloadIsotherm0", isotherm0);
	
//	Util::setSetting("downloadTempMin",  tempMin);
//	Util::setSetting("downloadTempMax",  tempMax);
	Util::setSetting("downloadSnowDepth", snowDepth);
	Util::setSetting("downloadSnowCateg", snowCateg);
	Util::setSetting("downloadFrzRainCateg", frzRainCateg);
	Util::setSetting("downloadCAPEsfc", CAPEsfc);
	Util::setSetting("downloadCINsfc", CINsfc);
	Util::setSetting("downloadGUSTsfc", GUSTsfc);
//	Util::setSetting("downloadSUNSDsfc", SUNSDsfc);
	
	Util::setSetting("downloadAltitudeData200",  chkAltitude200->isChecked());
	Util::setSetting("downloadAltitudeData300",  chkAltitude300->isChecked());
	Util::setSetting("downloadAltitudeData400",  chkAltitude400->isChecked());
	Util::setSetting("downloadAltitudeData500",  chkAltitude500->isChecked());
	Util::setSetting("downloadAltitudeData600",  chkAltitude600->isChecked());
	Util::setSetting("downloadAltitudeData700",  chkAltitude700->isChecked());
	Util::setSetting("downloadAltitudeData850",  chkAltitude850->isChecked());
	Util::setSetting("downloadAltitudeData925",  chkAltitude925->isChecked());
	Util::setSetting("downloadSkewtData",  chkAltitude_SkewT->isChecked());
	
    Util::setSetting("downoadWaveSig",  chkWaveSig->isChecked());
//	Util::setSetting("downloadFnmocWW3_max",  chkWaveMax->isChecked());
    Util::setSetting("downloadWaveSwell",  chkWaveSwell->isChecked());
    Util::setSetting("downloadWaveWind",  chkWaveWind->isChecked());
//	Util::setSetting("downloadFnmocWW3_prim", chkFnmocWW3_prim->isChecked());
//	Util::setSetting("downloadFnmocWW3_scdy", chkFnmocWW3_scdy->isChecked());
//	Util::setSetting("downloadFnmocWW3_wcap", chkFnmocWW3_wcap->isChecked());
//	Util::setSetting ("downloadIndWaveModel", waveDataModel);
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::updateParameters ()
{
    double tmp, xm, ym;
    
    ymax = sbNorth->cleanText().toDouble();
    ymin = sbSouth->cleanText().toDouble();
    xmin = sbWest->cleanText().toDouble();
    xmax = sbEast->cleanText().toDouble();

    atmosphericModel = cbModel->currentText();
    waveModel = cbWvModel->currentText();
    resolution = cbResolution->currentText().toDouble();
    interval   = cbInterval->currentText().toInt();
    days       = cbDays->currentText().toInt();
    cycle      = cbRunCycle->currentData().toString();

    if (xmin > xmax) {
        tmp = xmin;   xmin = xmax;   xmax = tmp;
    }
    if (ymin > ymax) {    // inverted Y scale (90 = north) ???
        tmp = ymin;   ymin = ymax;   ymax = tmp;
    }
    // too big ?
    if (fabs(xmax-xmin) >=360)
        xmax = xmin+359.9;
    if (fabs(ymin-ymax) >=180)
        ymin = ymax+179.9;
    
    // too small ?
    if (fabs(xmax-xmin) < 2*resolution) {
        xm = (xmin+xmax)/2;
        xmin = xm - 2*resolution;
        xmax = xm + 2*resolution;
    }
    if (fabs(ymin-ymax) < 2*resolution) {
        ym = (ymin+ymax)/2;
        ymin = ym + 2*resolution;
        ymax = ym - 2*resolution;
    }

    wind     = chkWind->isChecked();
    pressure = chkPressure->isChecked();
    rain     = chkRain->isChecked();
    cloud    = chkCloud->isChecked();
    cloudLayers    = false;
//     cloudLayers    = chkCloudLayers->isChecked();
    temp     = chkTemp->isChecked();
    humid    = chkHumid->isChecked();
    isotherm0    = chkIsotherm0->isChecked();
	
//    tempMin     = chkTempMin->isChecked();
//    tempMax     = chkTempMax->isChecked();
    snowDepth   = chkSnowDepth->isChecked();
    snowCateg   = chkSnowCateg->isChecked();
    frzRainCateg = chkFrzRainCateg->isChecked();
    CAPEsfc      = chkCAPEsfc->isChecked();
    CINsfc      = chkCINsfc->isChecked();
    GUSTsfc      = chkGUSTsfc->isChecked();
//    SUNSDsfc     = chkSUNSDsfc->isChecked();
	
//	if (bt_FNMOC_WW3_GLB->isChecked())
//		waveDataModel = FNMOC_WW3_GLB;
//	else
//		waveDataModel = FNMOC_WW3_MED;
	
}

//-------------------------------------------------------------------------------
// sets allowed meta data values in dialog and activates relevent parameter choices
void DialogLoadGRIB::slotAtmModelSettings()
{
    QString amod = cbModel->currentText();
    int ind = 0;
    if (amod == "GFS")
    {
        // set res, days, cyc options

        cbResolution->clear();
        cbResolution->addItems(QStringList()<< "0.25"<< "0.5" << "1.0");
        cbDays->clear();
        cbDays->addItems(QStringList()<< "1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10");
        ind = Util::getSetting("downloadIndNbDays", 7).toInt();
        ind = Util::inRange(ind, 0, cbDays->count()-1);
        cbDays->setCurrentIndex(ind);
        cbRunCycle->clear();
        cbRunCycle->insertItem (ind++, tr("Last"), "last");
        cbRunCycle->insertItem (ind++, tr("0 hr"), "00");
        cbRunCycle->insertItem (ind++, tr("6 hr"), "06");
        cbRunCycle->insertItem (ind++, tr("12 hr"), "12");
        cbRunCycle->insertItem (ind++, tr("18 hr"), "18");
        // reactivate parameters that may have been disabled
        chkSnowCateg->setCheckable(true);
        chkFrzRainCateg->setCheckable(true);
        chkCINsfc->setCheckable(true);
        chkIsotherm0->setCheckable(true);
        chkSnowDepth->setCheckable(true);

    }
    else if (amod == "ICON")
    {
        cbResolution->clear();
        cbResolution->addItem("0.25");
        cbDays->clear();
        cbDays->addItems(QStringList()<< "1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8");
        ind = Util::getSetting("downloadIndNbDays", 7).toInt();
        ind = Util::inRange(ind, 0, cbDays->count()-1);
        cbDays->setCurrentIndex(ind);
        cbRunCycle->clear();
        cbRunCycle->insertItem (ind++, tr("Last"), "last");
        cbRunCycle->insertItem (ind++, tr("0 hr"), "00");
        cbRunCycle->insertItem (ind++, tr("12 hr"), "12");
        // deactivate unvalid parameters
        chkSnowCateg->setCheckable(false); chkSnowCateg->repaint();
        chkFrzRainCateg->setCheckable(false); chkFrzRainCateg->repaint();
        chkCINsfc->setCheckable(false); chkCINsfc->repaint();

    }
    else if (amod == "Arpege")
    {
        cbResolution->clear();
        cbResolution->addItem("0.5");
        cbDays->clear();
        cbDays->addItems(QStringList()<< "1"<<"2"<<"3"<<"4");
        ind = Util::getSetting("downloadIndNbDays", 7).toInt();
        ind = Util::inRange(ind, 0, cbDays->count()-1);
        cbDays->setCurrentIndex(ind);
        cbRunCycle->clear();
        cbRunCycle->insertItem (ind++, tr("Last"), "last");
        cbRunCycle->insertItem (ind++, tr("0 hr"), "00");
        cbRunCycle->insertItem (ind++, tr("12 hr"), "12");
        // deactivate unvalid parameters
        chkSnowCateg->setCheckable(false); chkSnowCateg->repaint();
        chkFrzRainCateg->setCheckable(false); chkFrzRainCateg->repaint();
        chkCINsfc->setCheckable(false); chkCINsfc->repaint();
        chkIsotherm0->setCheckable(false); chkIsotherm0->repaint();
        chkSnowDepth->setCheckable(false); chkSnowDepth->repaint();

    }

    // propagate the change
    slotParameterUpdated ();

}

//-------------------------------------------------------------------------------
// activates or deactivates wave parameter options
void DialogLoadGRIB::slotWaveModelSettings()
{
    QString wmod = cbWvModel->currentText();

    if (wmod == "None")
    {
        // deactivate the parameter check boxes
        chkWaveSig->setCheckable(false); chkWaveSig->repaint();
        chkWaveSwell->setCheckable(false); chkWaveSwell->repaint();
        chkWaveWind->setCheckable(false); chkWaveWind->repaint();
    }
    else
    {
        // activate all of them
        chkWaveSig->setCheckable(true);
        chkWaveSwell->setCheckable(true);
        chkWaveWind->setCheckable(true);
    }
    // propagate the change
    slotParameterUpdated ();
}

//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotParameterUpdated ()
{
    // propage the change
    updateParameters();

    // then start to calculate the size of the request

    // number of points according to atmospheric resolution
	int npts = (int) (  ceil(fabs(xmax-xmin)/resolution)
                       * ceil(fabs(ymax-ymin)/resolution) );


    // Number of GribRecords
    int nbrec = (int) days*24/interval +1;
	
    int nbPress = pressure ?  nbrec   : 0;
    int nbWind  = wind     ?  2*nbrec : 0;
    int nbRain  = rain     ?  nbrec-1 : 0;
    int nbCloud = cloud    ?  nbrec-1 : 0;
    int nbCloudLayers = cloudLayers  ?  nbrec-1 : 0;
    int nbTemp  = temp     ?  nbrec   : 0;
    int nbHumid = humid    ?  nbrec   : 0;
    int nbIsotherm0 = isotherm0    ?  nbrec   : 0;
    
//    int nbTempMin  = tempMin ?  nbrec-1  : 0;
//    int nbTempMax  = tempMax ?  nbrec-1  : 0;
    int nbSnowDepth  = snowDepth ?  nbrec  : 0;
    int nbSnowCateg  = snowCateg ?  nbrec-1  : 0;
    int nbFrzRainCateg = frzRainCateg ?  nbrec-1  : 0;
    int nbCAPEsfc  = CAPEsfc ?  nbrec : 0;
    int nbCINsfc  = CINsfc ?  nbrec : 0;
    int nbGUSTsfc  = GUSTsfc ?  nbrec : 0;
//    int nbSUNSDsfc  = SUNSDsfc ?  nbrec : 0;
    
    int head = 179;
    int estime = 0;
    int nbits;  // this can vary when c3 packing is used. Average numbers are used here
    
    nbits = 12;
    estime += nbWind*(head+(nbits*npts)/8+2 );

    nbits = 9;
    estime += nbTemp*(head+(nbits*npts)/8+2 );

//    estime += nbTempMin*(head+(nbits*npts)/8+2 );
//    estime += nbTempMax*(head+(nbits*npts)/8+2 );

    nbits = 9;
    estime += nbRain*(head+24+(nbits*npts)/8+2 );

    nbits = 14;
    estime += nbPress*(head+(nbits*npts)/8+2 );

    nbits = 7;
    estime += nbCloud*(head+24+(nbits*npts)/8+2 );
    estime += 9*nbCloudLayers*(head+(nbits*npts)/8+2 );

    nbits = 8;
    estime += nbSnowDepth*(head+(nbits*npts)/8+2 );
    estime += nbSnowCateg*(head+24+(nbits*npts)/8+2 );
    estime += nbFrzRainCateg*(head+24+(nbits*npts)/8+2 );

    nbits = 10;
    estime += nbHumid*(head+(nbits*npts)/8+2 );

    nbits = 14;
    estime += nbIsotherm0*(head+(nbits*npts)/8+2 );

    nbits = 12;
    estime += nbCAPEsfc*(head+(nbits*npts)/8+2 );

    nbits = 11;
    estime += nbCINsfc*(head+(nbits*npts)/8+2 );

    nbits = 9;
    estime += nbGUSTsfc*(head+(nbits*npts)/8+2 );
//    estime += nbSUNSDsfc*(head+(nbits*npts)/8+2 );

	int nbalt = 0;
	if (chkAltitude200->isChecked()) nbalt++;
	if (chkAltitude300->isChecked()) nbalt++;
	if (chkAltitude400->isChecked()) nbalt++;
	if (chkAltitude500->isChecked()) nbalt++;
	if (chkAltitude600->isChecked()) nbalt++;
	if (chkAltitude700->isChecked()) nbalt++;
	if (chkAltitude850->isChecked()) nbalt++;
	if (chkAltitude925->isChecked()) nbalt++;
    nbits = 11;
	estime += nbrec*nbalt*5*(head+(nbits*npts)/8+2 );
	
	int nbskewt = 0;
	if (chkAltitude_SkewT->isChecked())
        nbskewt = 69;
	estime += nbrec*nbskewt*(head+(nbits*npts)/8+2 );

    // and now the wave estimate

    // recalculate number of points based on which model is used
    if (waveModel == "WW3") // 0.5 deg
    {
        npts = (int) (  ceil(fabs(xmax-xmin)/0.5)
                               * ceil(fabs(ymax-ymin)/0.5) );
        nbrec = (int) fmin(6,days)*24/interval +1;
    }
    if (waveModel == "GWAM") // 0.25 deg
    {
        npts = (int) (  ceil(fabs(xmax-xmin)/0.25)
                               * ceil(fabs(ymax-ymin)/0.25) );
        nbrec = (int) fmin(6,days)*24/interval +1;
    }
    if (waveModel == "EWAM") // 0.1 x 0.05 deg
    {
        npts = (int) (  ceil(fabs(xmax-xmin)/0.1)
                               * ceil(fabs(ymax-ymin)/0.05) );
        nbrec = (int) fmin(2,days)*24/interval +1;
    }


    if (chkWaveSig->isChecked())
    {
        nbits = 9;
        estime += nbrec*(head+(nbits*npts)/8+2);
    }


    if (chkWaveSwell->isChecked())
    {
        nbits = 15;
        estime += nbrec*(head+(nbits*npts)/8+2);
        nbits = 9;
        estime += nbrec*(head+(nbits*npts)/8+2);
        nbits = 10;
        estime += nbrec*(head+(nbits*npts)/8+2);

    }
    if (chkWaveWind->isChecked())
    {
        nbits = 15;
        estime += nbrec*(head+(nbits*npts)/8+2);
        nbits = 8;
        estime += nbrec*(head+(nbits*npts)/8+2);
        nbits = 10;
        estime += nbrec*(head+(nbits*npts)/8+2);

    }




    double estimeko = estime/1024.0;	// size in kb
	QString ssz;
	if (estimeko <= 100)
        ssz = QString("%1 KB").arg(estimeko,0,'f',2);
	else if (estimeko <= 1024)
        ssz = QString("%1 KB").arg(estimeko,0,'f',1);
	else
        ssz = QString("%1 MB").arg(estimeko/1024.0,0,'f',1);
	
    slotGribMessage(tr("Size: ≃ ") + ssz + tr(" (max 100 MB)"));
    
    if (estime == 0)
        btOK->setEnabled(false);
    else
        btOK->setEnabled(true);
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotBtOK()
{
    setCursor(Qt::WaitCursor);
    btCancel->setText(tr("Stop"));
    loadInProgress = true;
    btOK->setEnabled(false);
    if (loadgrib) {
        loadgrib->deleteLater ();
		loadgrib = NULL;
	}
    loadgrib = new FileLoaderGRIB (networkManager, this);
    assert(loadgrib);
    connect(loadgrib, SIGNAL(signalGribDataReceived(QByteArray *, QString)),
            this,  SLOT(slotGribDataReceived(QByteArray *, QString)));
    connect(loadgrib, SIGNAL(signalGribReadProgress(int, int, int)),
            this,  SLOT(slotGribReadProgress(int, int, int)));
    connect(loadgrib, SIGNAL(signalGribLoadError(QString)),
            this,  SLOT(slotGribFileError(QString)));
    connect(loadgrib, SIGNAL(signalGribSendMessage(QString)),
            this,  SLOT(slotGribMessage(QString)));
    connect(loadgrib, SIGNAL(signalGribStartLoadData()),
            this,  SLOT(slotGribStartLoadData()));
    loadgrib->getGribFile (atmosphericModel,
                    xmin, xmax, ymin, ymax,
                    resolution, interval, days, cycle,
					wind, pressure, rain, cloud, temp, humid, isotherm0,
                    snowDepth, snowCateg, frzRainCateg,
					CAPEsfc, CINsfc,
					chkAltitude200->isChecked(),
					chkAltitude300->isChecked(),
					chkAltitude400->isChecked(),
					chkAltitude500->isChecked(),
					chkAltitude600->isChecked(),
					chkAltitude700->isChecked(),
					chkAltitude850->isChecked(),
					chkAltitude925->isChecked(),
					chkAltitude_SkewT->isChecked(),
					GUSTsfc,
                    waveModel,
                    chkWaveSig->isChecked(),
                    chkWaveSwell->isChecked(),
                    chkWaveWind->isChecked()

				);
}
//-------------------------------------------------------------------------------
// TODO - Appears to be unused! can be removed
QString DialogLoadGRIB::createStringParameters ()
{
    QString parameters = "";
    if (wind)
        parameters += "W;";
    if (pressure)
        parameters += "P;";
    if (rain)
        parameters += "R;";
    if (cloud)
        parameters += "C;";
    if (temp)
        parameters += "T;";
    if (humid)
        parameters += "H;";
	if (isotherm0)
		parameters += "I;";
//    if (tempMin)
//        parameters += "m;";
//    if (tempMax)
//        parameters += "M;";
    if (snowDepth)
        parameters += "S;";
    if (snowCateg)
        parameters += "s;";
    if (frzRainCateg)
        parameters += "Z;";
    if (CAPEsfc)
        parameters += "c;";
    if (CINsfc)
        parameters += "i;";
    if (GUSTsfc)
        parameters += "G;";
//    if (SUNSDsfc)
//        parameters += "D;";
	
	return parameters;
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotBtServerStatus ()
{
    DialogServerStatus dial (networkManager, this);
    dial.exec ();
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotBtProxy ()
{
    DialogProxy dial (this);
    dial.exec ();
}
//-------------------------------------------------------------------------------
void DialogLoadGRIB::slotBtCancel()
{
	setCursor(oldcursor);
	if (loadInProgress)
    {
        loadInProgress = false;
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
void DialogLoadGRIB::setZone (double x0, double y0, double x1, double y1)
{
	double tmp;
	if (x0 > x1)  { tmp=x0; x0=x1; x1=tmp; }
    if (y0 > y1)  { tmp=y0; y0=y1; y1=tmp; }
	
    sbNorth->setValue ( ceil (y1) );
    sbSouth->setValue ( floor(y0) );
    sbWest->setValue  ( floor(x0) );
    sbEast->setValue  ( ceil (x1) );
	
    progressBar->setRange (0,100);
    progressBar->setValue (0);
    slotParameterUpdated ();
}
//-------------------------------------------------------------------------------
QFrame *DialogLoadGRIB::createFrameButtonsZone(QWidget *parent)
{
    int ind, lig,col;
    QFrame * ftmp, *ftmp2;
    QFrame * frm = new QFrame(parent);
    assert(frm);
    QVBoxLayout  *lay = new QVBoxLayout(frm);
    assert(lay);
	lay->setContentsMargins (0,0,0,0);
	lay->setSpacing (3);
	//------------------------------------------------
	// Geographic area
	//------------------------------------------------
    int sizemin = 0;
    sbNorth = new QDoubleSpinBox(this);
    assert(sbNorth);
    sbNorth->setDecimals(0);
    sbNorth->setMinimum(-90);
    sbNorth->setMaximum(90);
    // TODO suffix needs to be part of value. i.e. -25N is not acceptable
//    sbNorth->setSuffix(tr(" °N"));
    sbSouth = new QDoubleSpinBox(this);
    assert(sbSouth);
    sbSouth->setDecimals(0);
    sbSouth->setMinimum(-90);
    sbSouth->setMaximum(90);
//    sbSouth->setSuffix(tr(" °N"));
    sbWest = new QDoubleSpinBox(this);
    assert(sbWest);
    sbWest->setDecimals(0);
    sbWest->setMinimum(-360);
    sbWest->setMaximum(360);
//    sbWest->setSuffix(tr(" °E"));
    sbEast = new QDoubleSpinBox(this);
    assert(sbEast);
    sbEast->setDecimals(0);
    sbEast->setMinimum(-360);
    sbEast->setMaximum(360);
//    sbEast->setSuffix(tr(" °E"));

    //model combobox
    cbModel = new QComboBox(this);
    assert(cbModel);
    cbModel->addItems(QStringList()<< "GFS"<< "ICON" << "Arpege"  );
    cbModel->setMinimumWidth (sizemin);
    ind = Util::getSetting("downloadIndModel", 0).toInt();
    ind = Util::inRange(ind, 0, cbModel->count()-1);
    cbModel->setCurrentIndex(ind);

    //Wave model combobox
    cbWvModel = new QComboBox(this);
    assert(cbWvModel);
    cbWvModel->addItems(QStringList()<< "None" << "WW3"<< "GWAM" << "EWAM"  );
    cbWvModel->setMinimumWidth (sizemin);
    ind = Util::getSetting("downloadIndWaveModel", 0).toInt();
    ind = Util::inRange(ind, 0, cbWvModel->count()-1);
    cbWvModel->setCurrentIndex(ind);


    //------------------------------------------------
	// Résolution, intervalle, durée
	//------------------------------------------------
    cbResolution = new QComboBox(this);
    assert(cbResolution);
    cbResolution->addItems(QStringList()<< "0.25"<< "0.5" << "1.0");
    //gfs_p25_, gfs_p50_, gfs_1p0_, icon_p25_, arpege
    cbResolution->setMinimumWidth (sizemin);
	ind = Util::getSetting("downloadIndResolution", 0).toInt();
	ind = Util::inRange(ind, 0, cbResolution->count()-1);
    cbResolution->setCurrentIndex(ind);

    
    cbInterval = new QComboBox(this);
    assert(cbInterval);
    cbInterval->addItems(QStringList()<< "3" << "6" << "12" << "24");
    cbInterval->setMinimumWidth (sizemin);
	ind = Util::getSetting("downloadIndInterval", 0).toInt();
	ind = Util::inRange(ind, 0, cbInterval->count()-1);
    cbInterval->setCurrentIndex(ind);
    
    cbDays = new QComboBox(this);
    assert(cbDays);
    cbDays->addItems(QStringList()<< "1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10");
    cbDays->setMinimumWidth (sizemin);
	ind = Util::getSetting("downloadIndNbDays", 7).toInt();	
	ind = Util::inRange(ind, 0, cbDays->count()-1);
    cbDays->setCurrentIndex(ind);
    
    cbRunCycle = new QComboBox(this);
    assert(cbRunCycle);
	ind = 0;
    cbRunCycle->insertItem (ind++, tr("Last"), "last");
    cbRunCycle->insertItem (ind++, tr("0 hr"), "00");
    cbRunCycle->insertItem (ind++, tr("6 hr"), "06");
    cbRunCycle->insertItem (ind++, tr("12 hr"), "12");
    cbRunCycle->insertItem (ind++, tr("18 hr"), "18");
    cbRunCycle->setMinimumWidth (sizemin);
    QString data = Util::getSetting("downloadRunCycle", "").toString();
    ind = cbRunCycle->findData (data);
    ind = Util::inRange(ind, 0, cbRunCycle->count()-1);
    cbRunCycle->setCurrentIndex (ind);

	//------------------------------------------------
    // Choice of weather data
	//------------------------------------------------
    chkWind     = new QCheckBox(tr("Wind (10 m)"));
    assert(chkWind);
    chkPressure = new QCheckBox(tr("Mean sea level pressure"));
    assert(chkPressure);
    chkRain     = new QCheckBox(tr("Total precipitation"));
    assert(chkRain);
    chkCloud     = new QCheckBox(tr("Cloud cover")+" ("+tr("total")+")");
    assert(chkCloud);
    chkCloudLayers     = new QCheckBox(tr("Cloud cover")+" ("+tr("layers")+")");
    assert(chkCloudLayers);
    chkTemp     = new QCheckBox(tr("Temperature (2 m)"));
    assert(chkTemp);
    chkHumid    = new QCheckBox(tr("Relative humidity (2 m)"));
    assert(chkHumid);
    chkIsotherm0    = new QCheckBox(tr("Isotherm 0°C"));
    assert(chkIsotherm0);
    
//    chkTempMin     = new QCheckBox(tr("Temperature min (2 m)"));
//    assert(chkTempMin);
//    chkTempMax     = new QCheckBox(tr("Temperature max (2 m)"));
//    assert(chkTempMax);
    chkSnowCateg     = new QCheckBox(tr("Snow (snowfall possible)"));
    assert(chkSnowCateg);
    chkFrzRainCateg     = new QCheckBox(tr("Frozen rain (rainfall possible)"));
    assert(chkFrzRainCateg);
    chkSnowDepth     = new QCheckBox(tr("Snow (depth)"));
    assert(chkSnowDepth);
    chkCAPEsfc     = new QCheckBox(tr("CAPE")+" ("+tr("surface")+")");
    assert(chkCAPEsfc);
    chkCINsfc     = new QCheckBox(tr("CIN")+" ("+tr("surface")+")");
    assert(chkCINsfc);
    chkGUSTsfc     = new QCheckBox(tr("Wind gust (surface)"));
    assert(chkGUSTsfc);
//    chkSUNSDsfc     = new QCheckBox(tr("Sunshine duration"));
//    assert(chkSUNSDsfc);
	
    //--------------------------------------------------------------------------------
    chkWind->setChecked    (Util::getSetting("downloadWind", true).toBool());
    chkPressure->setChecked(Util::getSetting("downloadPressure", true).toBool());
    chkRain->setChecked    (Util::getSetting("downloadRain", true).toBool());
    chkCloud->setChecked   (Util::getSetting("downloadCloud", true).toBool());
    chkCloudLayers->setChecked   (Util::getSetting("downloadCloudLayers", true).toBool());
    chkTemp->setChecked    (Util::getSetting("downloadTemp", true).toBool());
    chkHumid->setChecked   (Util::getSetting("downloadHumid", true).toBool());
    chkIsotherm0->setChecked  (Util::getSetting("downloadIsotherm0", true).toBool());
    	
//    chkTempMin->setChecked    (Util::getSetting("downloadTempMin", false).toBool());
//    chkTempMax->setChecked    (Util::getSetting("downloadTempMax", false).toBool());
    chkSnowDepth->setChecked  (Util::getSetting("downloadSnowDepth", true).toBool());
    chkSnowCateg->setChecked  (Util::getSetting("downloadSnowCateg", true).toBool());
    chkFrzRainCateg->setChecked  (Util::getSetting("downloadFrzRainCateg", true).toBool());
    chkCAPEsfc->setChecked  (Util::getSetting("downloadCAPEsfc", true).toBool());
    chkCINsfc->setChecked  (Util::getSetting("downloadCINsfc", true).toBool());
    chkGUSTsfc->setChecked  (Util::getSetting("downloadGUSTsfc", true).toBool());
//    chkSUNSDsfc->setChecked (Util::getSetting("downloadSUNSDsfc", false).toBool());
	//----------------------------------------------------------------
    chkAltitude925  = new QCheckBox ("925 "+tr("hPa"));
    assert (chkAltitude925);
    chkAltitude925->setChecked  (Util::getSetting("downloadAltitudeData925", false).toBool());
    chkAltitude850  = new QCheckBox ("850 "+tr("hPa"));
    assert (chkAltitude850);
    chkAltitude850->setChecked  (Util::getSetting("downloadAltitudeData850", false).toBool());
    chkAltitude700  = new QCheckBox ("700 "+tr("hPa"));
    assert (chkAltitude700);
    chkAltitude700->setChecked  (Util::getSetting("downloadAltitudeData700", false).toBool());
	
    chkAltitude400  = new QCheckBox ("400 "+tr("hPa"));
    assert (chkAltitude400);
    chkAltitude400->setChecked  (Util::getSetting("downloadAltitudeData400", false).toBool());
    chkAltitude500  = new QCheckBox ("500 "+tr("hPa"));
    assert (chkAltitude500);
    chkAltitude500->setChecked  (Util::getSetting("downloadAltitudeData500", false).toBool());
    chkAltitude600  = new QCheckBox ("600 "+tr("hPa"));
    assert (chkAltitude600);
    chkAltitude600->setChecked  (Util::getSetting("downloadAltitudeData600", false).toBool());
	
    chkAltitude300  = new QCheckBox ("300 "+tr("hPa"));
    assert (chkAltitude300);
    chkAltitude300->setChecked  (Util::getSetting("downloadAltitudeData300", false).toBool());
    chkAltitude200  = new QCheckBox ("200 "+tr("hPa"));
    assert (chkAltitude200);
    chkAltitude200->setChecked  (Util::getSetting("downloadAltitudeData200", false).toBool());

	chkAltitude_SkewT  = new QCheckBox (tr("Skewt-T"));
    assert (chkAltitude_SkewT);
    chkAltitude_SkewT->setChecked  (Util::getSetting("downloadSkewtData", false).toBool());

    chkAltitude_All = new QCheckBox (tr("All"));
    assert (chkAltitude_All);
	chkAltitude_All->setChecked  (
		      chkAltitude200->isChecked() && chkAltitude300->isChecked() && chkAltitude400->isChecked() 
		   && chkAltitude500->isChecked() && chkAltitude600->isChecked() && chkAltitude700->isChecked() 
		   && chkAltitude850->isChecked() && chkAltitude925->isChecked()
		);
	//----------------------------------------------------------------
	// Waves
	//----------------------------------------------------------------
    chkWaveSig  = new QCheckBox (tr("Significant height"));
    assert (chkWaveSig);
//    chkWaveMax  = new QCheckBox (tr("Maximum waves"));
//    assert (chkWaveMax);
    chkWaveSwell  = new QCheckBox (tr("Swell"));
    assert (chkWaveSwell);
    chkWaveWind  = new QCheckBox (tr("Wind waves"));
    assert (chkWaveWind);
//    chkFnmocWW3_prim  = new QCheckBox (tr("Primary waves"));
//    assert (chkFnmocWW3_prim);
//    chkFnmocWW3_scdy  = new QCheckBox (tr("Secondary waves"));
//    assert (chkFnmocWW3_scdy);
//    chkFnmocWW3_wcap  = new QCheckBox (tr("Whitecap probability"));
//    assert (chkFnmocWW3_wcap);
	
    chkWaveSig->setChecked  (Util::getSetting("downoadWaveSig", false).toBool());
//    chkWaveMax->setChecked  (Util::getSetting("downloadFnmocWW3_max", false).toBool());
    chkWaveSwell->setChecked  (Util::getSetting("downloadWaveSwell", false).toBool());
    chkWaveWind->setChecked  (Util::getSetting("downloadWaveWind", false).toBool());
//    chkFnmocWW3_prim->setChecked (Util::getSetting("downloadFnmocWW3_prim", false).toBool());
//    chkFnmocWW3_scdy->setChecked (Util::getSetting("downloadFnmocWW3_scdy", false).toBool());
//    chkFnmocWW3_wcap->setChecked (Util::getSetting("downloadFnmocWW3_wcap", false).toBool());
	
//    chkWindAll = new QCheckBox (tr("All"));
//    assert (chkWindAll);
//    chkWindAll->setChecked  (
//              chkWaveSig->isChecked() && chkWaveSwell->isChecked()
//                && chkWaveWind->isChecked()
//		);
	




//    waveDataModel = (DataCenterModel)(Util::getSetting("downloadIndWaveModel", FNMOC_WW3_MED).toInt());

//    bt_FNMOC_WW3_GLB = new QRadioButton
//		(tr("FNMOC-WW3-GLOBAL: all oceans (7 days, 1°x1°)"));
//	bt_FNMOC_WW3_MED  = new QRadioButton
//		(tr("FNMOC-WW3-MEDIT: Mediterranean Sea, Atlantic NE (3 days, 0.2°x0.2°)"));
	
//	if (waveDataModel == FNMOC_WW3_MED)
//		bt_FNMOC_WW3_MED->setChecked (true);
//	else
//		bt_FNMOC_WW3_GLB->setChecked (true);
	
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

    QLayout  *tlay, *vlay;
    QGridLayout  *tgrid;
	//------------------------------------------------
    // Layout of widgets
	//------------------------------------------------
	ftmp = new QFrame(this);
    tgrid = new QGridLayout(ftmp);
    assert(tgrid);
	tgrid->setContentsMargins (0,0,0,0);
		tgrid->addWidget( new QLabel(tr("Latitude min :")), 0, 0, Qt::AlignRight);
        tgrid->addWidget( sbSouth, 0, 1);
		tgrid->addWidget( new QLabel(tr("Latitude max :")), 0, 2, Qt::AlignRight);
        tgrid->addWidget( sbNorth, 0, 3);
		tgrid->addWidget( new QLabel(tr("Longitude min :")), 1, 0, Qt::AlignRight);
		tgrid->addWidget( sbWest, 1, 1);
		tgrid->addWidget( new QLabel(tr("Longitude max :")), 1, 2, Qt::AlignRight);
		tgrid->addWidget( sbEast, 1, 3);
    lay->addWidget( ftmp);
    //-------------------------
    addSeparator (lay, 'H');


    ftmp = new QFrame(this);
    tgrid = new QGridLayout(ftmp);
    assert(tgrid);
    tgrid->setContentsMargins (0,0,0,0);
    tgrid->addWidget( new QLabel(tr("Atmospheric Model :")), 0, 0, Qt::AlignRight);
    tgrid->addWidget( cbModel, 0, 1);
    //-------------------------
//    addSeparator (tlay, 'V');
    //-------------------------
    tgrid->addWidget( new QLabel(tr("Wave Model :")), 0, 2, Qt::AlignRight);
    tgrid->addWidget( cbWvModel, 0, 3);
    lay->addWidget( ftmp);

//    ftmp = new QFrame(this);
//    tlay = new QHBoxLayout(ftmp);
//    assert(tlay);
//    tlay->setContentsMargins (0,0,0,0);
//    tlay->addWidget( new QLabel(tr("Wave Model:")));
//    tlay->addWidget( cbWvModel);
//    lay->addWidget( ftmp);


    addSeparator (lay, 'H');
    //-------------------------
	ftmp = new QFrame(this);
    tlay = new QHBoxLayout(ftmp);
    assert(tlay);
	tlay->setContentsMargins (0,0,0,0);
		tlay->addWidget( new QLabel(tr("Resolution:")));
		tlay->addWidget( cbResolution);
		tlay->addWidget( new QLabel(tr("°")));
		//-------------------------
		addSeparator (tlay, 'V'); 
		//-------------------------
		tlay->addWidget( new QLabel(tr("Interval:")));
		tlay->addWidget( cbInterval);
		tlay->addWidget( new QLabel(tr("hours")));
		//-------------------------
		addSeparator (tlay, 'V'); 
		//-------------------------
		tlay->addWidget( new QLabel(tr("Period:")));
		tlay->addWidget( cbDays);
		tlay->addWidget( new QLabel(tr("days")));
		//-------------------------
		addSeparator (tlay, 'V'); 
		//-------------------------
        tlay->addWidget( new QLabel(tr("Run cycle:")));
        tlay->addWidget( cbRunCycle);
    lay->addWidget( ftmp);
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
	QTabWidget *tabWidget = new QTabWidget (this);
    lay->addWidget (tabWidget);
	//-------------------------------------------
	// Standard TAB
	//-------------------------------------------
	QWidget  *tabbox;
	ftmp = new QFrame (this);
    tgrid = new QGridLayout (ftmp);
    assert (tgrid);
	tgrid->setContentsMargins (4,2,4,2);
	tgrid->setSpacing (2);
        // Column 1
    	col = 0;
    	lig = 0;
		tgrid->addWidget( chkWind ,      lig++, col, Qt::AlignLeft);
        tgrid->addWidget( chkGUSTsfc , lig++, col, Qt::AlignLeft);
        tgrid->addWidget( chkPressure ,  lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkTemp ,      lig++, col, Qt::AlignLeft);
//		tgrid->addWidget( chkTempMin ,   lig++, col, Qt::AlignLeft);
//		tgrid->addWidget( chkTempMax ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkIsotherm0 , lig++, col, Qt::AlignLeft);
		// CAPE + CIN in the same frame
		ftmp2 = new QFrame(this);
		assert (ftmp2);
		tlay = new QHBoxLayout (ftmp2);
		assert (tlay);
		tlay->setContentsMargins (0,0,0,0);
			tlay->addWidget( chkCAPEsfc );
			tlay->addWidget( chkCINsfc );
		tgrid->addWidget( ftmp2,  lig++, col, Qt::AlignLeft);
    	// Colonne 2
    	col = 1;
    	lig = 0;
		tgrid->addWidget( chkCloud ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkHumid ,   lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkRain ,    lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkSnowCateg ,  lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkSnowDepth ,  lig++, col, Qt::AlignLeft);
		tgrid->addWidget( chkFrzRainCateg , lig++, col, Qt::AlignLeft);
    tabWidget->addTab (ftmp, tr("Surface Data"));
	//-------------------------------------------
	// Altitude TAB
	//-------------------------------------------
	tabbox = new QWidget (ftmp);
	assert (tabbox);
	vlay = new QVBoxLayout (tabbox);
	assert (vlay);
	vlay->addWidget (new QLabel (
			tr("Atmosphere: geopotential altitude, wind, temperature, theta-e, relative humidity.") 
			+"\n"+
            tr("Warning: Selecting this data significantly increases the size of the GRIB file.")
		));
		ftmp = new QFrame(this);
		assert (ftmp);
		tlay = new QHBoxLayout (ftmp);
		assert (tlay);
		tlay->setContentsMargins (0,0,0,0);
		QFrame *fgrid = new QFrame(this);
		assert (fgrid);
		QGridLayout *gtmp = new QGridLayout (fgrid);
		assert (gtmp);
		gtmp->setContentsMargins (0,0,0,0);
			gtmp->addWidget (chkAltitude925, 0,0);
			gtmp->addWidget (chkAltitude850, 0,1);
			gtmp->addWidget (chkAltitude700, 0,2);
			gtmp->addWidget (chkAltitude600, 0,3);
			
			gtmp->addWidget (chkAltitude500, 1,1);
			gtmp->addWidget (chkAltitude400, 1,0);
			gtmp->addWidget (chkAltitude300, 1,2);
			gtmp->addWidget (chkAltitude200, 1,3);
		tlay->addWidget (fgrid);
		addSeparator (tlay, 'V'); 
		tlay->addWidget (chkAltitude_All);
		addSeparator (tlay, 'V'); 
		tlay->addWidget (chkAltitude_SkewT);
	vlay->addWidget (ftmp);
    tabWidget->addTab (tabbox, tr("Altitude Data"));
	//-------------------------------------------
	// Waves TAB
	//-------------------------------------------
	tabbox = new QWidget (ftmp);
	assert (tabbox);
    tgrid = new QGridLayout (tabbox);
    assert (tgrid);
	tgrid->setContentsMargins (4,2,4,2);
    tgrid->setSpacing (1);
    	// Colonne 1
    	col = 0;
    	lig = 0;
        tgrid->addWidget (chkWaveSig, lig++, col, Qt::AlignLeft);
//        tgrid->addWidget (chkWaveMax, lig++, col, Qt::AlignLeft);
        tgrid->addWidget (chkWaveSwell, lig++, col, Qt::AlignLeft);
        // Colonne 2
//        col = 1;
//        lig = 0;
        tgrid->addWidget (chkWaveWind, lig++, col, Qt::AlignLeft);
		
//		tgrid->addWidget (newSeparator ('H'), lig++, 0, 1, 2);
//        tgrid->addWidget (chkWindAll, lig++, 0, 1, 2, Qt::AlignCenter);
//		tgrid->addWidget (newSeparator ('H'), lig++, 0, 1, 2);
		
//		tgrid->addWidget (bt_FNMOC_WW3_GLB, lig++, 0, 1, 2);
//		tgrid->addWidget (bt_FNMOC_WW3_MED, lig++, 0, 1, 2);

//		tgrid->addWidget (chkFnmocWW3_prim, lig++, col, Qt::AlignLeft);
//		tgrid->addWidget (chkFnmocWW3_scdy, lig++, col, Qt::AlignLeft);
//		tgrid->addWidget (chkFnmocWW3_wcap, lig++, col, Qt::AlignLeft);

    tabWidget->addTab (tabbox, tr("Wave Data"));
	
    //-------------------------
    addSeparator (lay, 'H'); 
    //-------------------------
    lay->addWidget( progressBar );
    //-------------------------
    labelMsg = new QLabel();
    lay->addWidget( labelMsg );
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
void DialogLoadGRIB::addSeparator (QLayout *layout, char orientation)
{
	QFrame *ftmp = newSeparator (orientation);
	layout->addWidget(ftmp);
}
//----------------------------------------------------------------------------
QFrame *DialogLoadGRIB::newSeparator (char orientation)
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
	return ftmp;
}









