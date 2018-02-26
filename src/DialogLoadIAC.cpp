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
#include <QFileInfo>
#include <cmath>
#include <cassert>

#include "DialogLoadIAC.h"
#include "Util.h"

//-------------------------------------------------------------------------------
QString DialogLoadIAC::getFile (QNetworkAccessManager *manager, QWidget *parent)
{
	DialogLoadIAC dial (manager,parent);
	dial.exec ();
	return dial.savedFileName;
}

//-------------------------------------------------------------------------------
void DialogLoadIAC::closeEvent (QCloseEvent *)
{
    if (IACloader) {
        IACloader->deleteLater ();
		IACloader = NULL;
	}
}
void DialogLoadIAC::slotFinished (int)
{
    if (IACloader) {
        IACloader->deleteLater ();
		IACloader = NULL;
	}
}

//-------------------------------------------------------------------------------
DialogLoadIAC::DialogLoadIAC (QNetworkAccessManager *manager, QWidget *parent) 
	: DialogBoxBase (parent)
{
	IACloader = NULL;
	networkManager = manager;
	savedFileName = "";
    
    setWindowTitle(tr("Download - IAC fleetcode"));
    loadInProgress = false;
    QFrame * frameButtonsZone = createFrameButtonsZone(this);
    
    QGridLayout  *lay = new QGridLayout(this);
    assert(lay);
    lay->addWidget( frameButtonsZone,1,0, Qt::AlignLeft);

    connect(btCancel,   SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btAnalyse,  SIGNAL(clicked()), this, SLOT(slotBtDownload()));
    connect(btForecast, SIGNAL(clicked()), this, SLOT(slotBtDownload()));
}

//-------------------------------------------------------------------------------
DialogLoadIAC::~DialogLoadIAC()
{
    if (IACloader != NULL)
        delete IACloader;
}

//----------------------------------------------------
void DialogLoadIAC::slotLoaderMessage (QString msg)
{
    labelMsg->setText(msg);
}

//----------------------------------------------------
void DialogLoadIAC::slotLoaderDataReceived (QByteArray *content, QString fileName)
{
	QString path = Util::getSetting("gribFilePath", "").toString();
	if (path == "")
		path = "./";
	else
		path += "/";
	savedFileName = "";
	QFileInfo fi (fileName);
    fileName = Util::getSaveFileName (NULL,
                 tr("Save IAC file : %1 octets").arg(content->size()),
                 path+fi.fileName(), "");
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
        slotLoaderReadProgress(1,0,100);
    }
	btAnalyse->setEnabled(true);
	btForecast->setEnabled(true);
	btCancel->setText(tr("Cancel"));
	loadInProgress = false;
}

//----------------------------------------------------
void DialogLoadIAC::slotLoaderFileError (QString error)
{
    if (! loadInProgress)
        return;
    QString s;    
    QMessageBox::critical (this,
                    tr("Error"),
                    tr("Error: ") + error );
    loadInProgress = false;
    btCancel->setText(tr("Cancel"));
	btAnalyse->setEnabled(true);
	btForecast->setEnabled(true);
    labelMsg->setText("");
}

//----------------------------------------------------
void DialogLoadIAC::slotLoaderStartLoadData ()
{
    timeLoad.start();
}

//----------------------------------------------------
void DialogLoadIAC::slotLoaderReadProgress (int , int done, int total)
{
	progressBar->setRange(0,total);
	progressBar->setValue(done);
	
	slotLoaderMessage(tr("Size: %1 ko      Done: %2 ko     ")
			.arg( total )
			.arg( done )
	);
}

//-------------------------------------------------------------------------------
void DialogLoadIAC::slotBtDownload()
{
    if (IACloader) {
        IACloader->deleteLater ();
		IACloader = NULL;
	}
    IACloader = new FileLoaderIAC (networkManager, this);
    assert (IACloader);
    connect(IACloader, SIGNAL(signalIACdataReceived(QByteArray *, QString)),
            this,  SLOT(slotLoaderDataReceived(QByteArray *, QString)));
    connect(IACloader, SIGNAL(signalIACreadProgress(int, int, int)),
            this,  SLOT(slotLoaderReadProgress(int, int, int)));
    connect(IACloader, SIGNAL(signalIACloadError(QString)),
            this,  SLOT(slotLoaderFileError(QString)));
    connect(IACloader, SIGNAL(signalIACsendMessage(QString)),
            this,  SLOT(slotLoaderMessage(QString)));
    connect(IACloader, SIGNAL(signalIACstartLoadData()),
            this,  SLOT(slotLoaderStartLoadData()));
            
    //------------------------------------------------------
    btCancel->setText(tr("Stop"));
	btAnalyse->setEnabled(false);
	btForecast->setEnabled(false);
    loadInProgress = true;
    
    if (sender() == btAnalyse)
    	IACloader->getFile (FileLoaderIAC::ANALYSE_FILE);
    if (sender() == btForecast)
	    IACloader->getFile (FileLoaderIAC::FORECAST_FILE);
}

//-------------------------------------------------------------------------------
void DialogLoadIAC::slotBtCancel()
{
    if (loadInProgress)
    {
        loadInProgress = false;
        IACloader->stop();
        btCancel->setText(tr("Cancel"));
        progressBar->setRange(0,100);
        progressBar->setValue(0);
		btAnalyse->setEnabled(true);
		btForecast->setEnabled(true);
    }
    else {
        reject();
    }
}

//-------------------------------------------------------------------------------
QFrame *DialogLoadIAC::createFrameButtonsZone(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
    assert(frm);
    QGridLayout  *lay = new QGridLayout(frm);
    assert(lay);
    int lig;
    QFrame *ftmp;

    btAnalyse   = new QPushButton(tr("Current analyse"), this);
    assert(btAnalyse);
    btForecast	= new QPushButton(tr("Forecast +24h"), this);
    assert(btAnalyse);
    btCancel = new QPushButton(tr("Cancel"), this);
    assert(btCancel);

    progressBar = new QProgressBar (this);
    assert(progressBar);
	progressBar->setRange(0,100);
	progressBar->setValue(0);

    lig = 0;
    lay->addWidget(
    	new QLabel(
			tr( "IAC file (fleetcode) from NOAA :\n"
				"analyse (pressure, isobars, fronts) and 24h forecast.\n"
				"\n"
				"Only one zone : Europe + Atlantic North-East\n"
				"\n"
				"4 runs in a day.\n"
				"\n"
			  )),
    	lig, 0, 1,-1);
    
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    lay->addWidget( progressBar, lig,0, 1, -1);

    //-------------------------
    lig ++;
    labelMsg = new QLabel();
    lay->addWidget( labelMsg, lig,0, 1, -1);
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    lay->addWidget( btAnalyse,  lig,0,  1, 1);
    lay->addWidget( btForecast, lig,1,  1, 1);
    lay->addWidget( btCancel, 	lig,2,  1, 1);

    return frm;
}











