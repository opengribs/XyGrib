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

#include <QMessageBox>
#include <cmath>
#include <cassert>

#include "DialogServerStatus.h"
#include "Util.h"
#include "Version.h"

//-------------------------------------------------------------------------------
DialogServerStatus::DialogServerStatus 
			(QNetworkAccessManager *manager, QWidget *parent)
	: DialogBoxBase (parent)
{
	QString page;
	downloadError = false;
    reply_step1 = nullptr;
    page = "/getstatus.php";
    setWindowTitle (tr("Server"));
    QFrame *ftmp;
    QLabel *label;
    frameGui = createFrameGui(this);
    layout = new QGridLayout(this);
	layout->setVerticalSpacing (0);
	layout->setContentsMargins (10,6,10,6);
    int lig=0;
    //-------------------------
    lig ++;
    QFont font;
    font.setBold(true);
    label = new QLabel(tr("File server status"), this);
    label->setFont(font);
    layout->addWidget( label,    lig,0, 1,-1, Qt::AlignCenter);
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    layout->addWidget( frameGui,  lig,0,   1, 2);
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    btOK     = new QPushButton(tr("Ok"), this);
    layout->addWidget( btOK,    lig,0 );

    if (Util::getSetting("showDarkSkin", true).toBool())
        this->setStyleSheet(sstStyleSheet);



	//===============================================================
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    //===============================================================
    timeLoad.start();
    QNetworkRequest request = Util::makeNetworkRequest ("http://"+Util::getServerName()+page);
	reply_step1 = manager->get (request);
	connect (reply_step1, SIGNAL(downloadProgress (qint64,qint64)), 
                    this, SLOT(downloadProgress (qint64,qint64)));
	connect (reply_step1, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
    connect (reply_step1, SIGNAL(finished()),
             this, SLOT(slotFinished ()));
}
//-------------------------------------------------------------------------------
DialogServerStatus::~DialogServerStatus() {
	if (reply_step1) {
		reply_step1->deleteLater ();
        reply_step1 = nullptr;
	}
}

//-------------------------------------------------------------------------------
void DialogServerStatus::slotNetworkError (QNetworkReply::NetworkError /*err*/)
{
	if (!downloadError) {
        downloadError = true;
		errorMessage = reply_step1->errorString();
		QMessageBox::critical (this, tr("Error"), errorMessage);
	}
}	

//-------------------------------------------------------------------------------
void DialogServerStatus::slotFinished()
{
	QString strDur; 
    int tp = timeLoad.elapsed();
    QTextStream(&strDur) << tp << " ms";
	
	if (downloadError) {
        lbResponseStatus->setText (tr("error")+" ("+strDur+")");
        lbMessage->setText("Error: "+errorMessage);
    }
    else //if (done == total)
    {
        lbResponseStatus->setText (tr("OK")+" ("+strDur+")");

        QByteArray data = reply_step1->readAll ();
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        QJsonObject jsondata = jsondoc.object();

        QString sstatus, seta;

        for ( int i=0; i< ar_statuses_keys.count(); i++)
        {

            QJsonObject srvstat_x = jsondata[ ar_statuses_keys[i]  ].toObject();
            ar_lbRunDate[i]->setText (srvstat_x["reference"].toString());
            ar_lbUpdateTime[i]->setText (srvstat_x["posted_at"].toString());
            sstatus = srvstat_x["status"].toString();
            seta = srvstat_x["eta"].toString();
            if (seta != "")
                seta = " - ETA: " + seta;
            if (sstatus == "working")
                sstatus = tr("Working");
            if (sstatus == "waiting")
                sstatus = tr("Waiting");
            ar_lbCurrentJob[i]->setText (sstatus + seta);
        }


    }
}

//-------------------------------------------------------------------------------
void DialogServerStatus::slotBtOK()
{
    accept();
}
//-------------------------------------------------------------------------------
void DialogServerStatus::slotBtCancel()
{
    reject();
}

//=============================================================================
// GUI
//=============================================================================
QFrame *DialogServerStatus::createFrameGui(QWidget *parent)
{
    QFont fontBold;
    fontBold.setBold(true);
    QFrame * frm = new QFrame(parent);
    QFrame * ftmp;
    QLabel * label;
    QGridLayout  *lay = new QGridLayout(frm);
	lay->setVerticalSpacing (0);
    int lig=0;
    //-------------------------
    lig ++;
    label = new QLabel(tr("Connection :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lbResponseStatus = new QLabel("", frm);
    lay->addWidget( lbResponseStatus, lig,1, Qt::AlignLeft);


    //ar_statuses = new QHash <QString,const QString>();

    ar_statuses_keys.append("gfs");
    ar_statuses_keys.append("ico");
    ar_statuses_keys.append("arp");
    ar_statuses_keys.append("ww3");
    ar_statuses_keys.append("gwa");
    ar_statuses_keys.append("ewa");

    ar_statuses.insert("gfs", "NOAA-GFS");
    ar_statuses.insert("ico", "ICON Global");
    ar_statuses.insert("arp", "Arpege Global");
    ar_statuses.insert("ww3", "WW3");
    ar_statuses.insert("gwa", "GWAM");
    ar_statuses.insert("ewa", "EWAM");


    for ( int i=0; i< ar_statuses.count(); i++)
    {
        QString key = ar_statuses_keys.at(i);//  x.key();// ar_statuses.keys()[i];
        lig ++;
        ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
        //-------------------------
        lig ++;

        label = new QLabel(ar_statuses.value(key), frm);
        label->setFont (fontBold);
        lay->addWidget( label,    lig,0, Qt::AlignLeft);
        //-------------------------
        lig ++;
        label = new QLabel(tr("Forecast date :"), frm);
        lay->addWidget( label,    lig,0, Qt::AlignRight);
        ar_lbRunDate[i] = new QLabel("", frm);
        lay->addWidget( ar_lbRunDate[i], lig,1, Qt::AlignLeft);
        //-------------------------
        lig ++;
        label = new QLabel (tr("Update time :"), frm);
        lay->addWidget (label,    lig,0, Qt::AlignRight);
        ar_lbUpdateTime[i] = new QLabel ("", frm);
        lay->addWidget (ar_lbUpdateTime[i], lig,1, Qt::AlignLeft);
        //-------------------------
        lig ++;
        label = new QLabel(tr("Activity :"), frm);
        lay->addWidget( label,    lig,0, Qt::AlignRight);
        ar_lbCurrentJob[i] = new QLabel("", frm);
        lay->addWidget( ar_lbCurrentJob[i], lig,1, Qt::AlignLeft);

    }

    //-------------------------
	// Messages
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    lbMessage = new QLabel("                                                                    ", frm);
    lay->addWidget( lbMessage, lig,0,1,2, Qt::AlignLeft);

    return frm;
}











