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
	reply_step1 = NULL;
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
    layout->addWidget( btOK,    lig,0);

    if (Util::getSetting("showDarkSkin", true).toBool())
        this->setStyleSheet(sstStyleSheet);



	//===============================================================
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    //===============================================================
    timeLoad.start();
    QNetworkRequest request = Util::makeNetworkRequest ("http://"+Util::getServerName()+page);
	reply_step1 = manager->get (request);
	connect (reply_step1, SIGNAL(downloadProgress (qint64,qint64)), 
					this, SLOT(downloadProgress_step1 (qint64,qint64)));
	connect (reply_step1, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
    connect (reply_step1, SIGNAL(finished()),
             this, SLOT(slotFinished_step1 ()));
}
//-------------------------------------------------------------------------------
DialogServerStatus::~DialogServerStatus() {
	if (reply_step1) {
		reply_step1->deleteLater ();
		reply_step1 = NULL;
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
// TODO - this slot can be removed. Not used any more
void DialogServerStatus::downloadProgress_step1 (qint64 done, qint64 total)
{
}
void DialogServerStatus::slotFinished_step1()
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

        QString sdate, shour;

        for ( int i=0; i< ar_statuses.count(); i++)
        {

            QJsonObject srvstat_x = jsondata[ ar_statuses.keys()[i]  ].toObject();
            sdate = srvstat_x["reference"].toString();
            shour = srvstat_x["last_cycle"].toString();
            ar_lbRunDate[i]->setText (sdate+"     "+shour);
            ar_lbUpdateTime[i]->setText (srvstat_x["posted_at"].toString());
            ar_lbCurrentJob[i]->setText (srvstat_x["status"].toString());
        }


//        //------------------------------------------------------
//        QJsonObject srvstat_gfs = jsondata["gfs"].toObject();
//        sdate = srvstat_gfs["reference"].toString();
//        shour = srvstat_gfs["last_cycle"].toString();
//        lbRunDate->setText (sdate+"     "+shour);
//        lbGfsUpdateTime->setText (srvstat_gfs["posted_at"].toString());
//        lbCurrentJob->setText (srvstat_gfs["status"].toString());
//        //------------------------------------------------------
//        QJsonObject srvstat_ico = jsondata["ico"].toObject();
//        sdate = srvstat_ico["reference"].toString();
//        shour = srvstat_ico["last_cycle"].toString();
//        lbFnmocWW3_RunDate->setText (sdate+"     "+shour);
//        lbFnmocWW3_UpdateTime->setText (srvstat_ico["posted_at"].toString());
//        lbFnmocWW3_CurrentJob->setText (srvstat_ico["status"].toString());
//        //------------------------------------------------------
//        QJsonObject srvstat_arp = jsondata["ico"].toObject();
//        sdate = srvstat_arp["reference"].toString();
//        shour = srvstat_arp["last_cycle"].toString();
//        lbFnmocWW3_Med_RunDate->setText (sdate+"     "+shour);
//        lbFnmocWW3_Med_UpdateTime->setText (srvstat_arp["posted_at"].toString());
//        lbFnmocWW3_Med_CurrentJob->setText (srvstat_arp["status"].toString());
        //------------------------------------------------------
// 		sdate = getData (allNoaa, "MBLUE-NMM4_run_date");
// 		shour = getData (allNoaa, "MBLUE-NMM4_run_hour");
//         lbMblueNMM4RunDate->setText (sdate+"     "+shour);
//         lbMblueNMM4UpdateTime->setText (getData (allNoaa, "MBLUE-NMM4_update_time"));
//         lbMblueNMM4CurrentJob->setText (getData (allNoaa, "MBLUE-NMM4_current_job"));
        //------------------------------------------------------


        //lbMessage->setText (allNoaa.value ("message"));
    }
}

//-------------------------------------------------------------------------------
// TODO - this method can be removed. Not used any more
QString DialogServerStatus::getData (const QHash <QString,QString> &data, const QString &key)
{
    QString rep = data.value (key, "");
/*    if (rep == "")
		rep=tr("invalid format");*/
	return rep;
}

//-------------------------------------------------------------------------------
// TODO - this method can be removed. Not used any more
QHash <QString,QString> DialogServerStatus::readData (const QByteArray &data)
{
	QHash <QString,QString> result;
	QString tmp;
	QString strbuf (data);
	QStringList lsbuf = strbuf.split("\n");
    for (int i=0; i < lsbuf.size(); i++)
    {
        QStringList lsval = lsbuf.at(i).split(":");
        if (lsval.size() > 1) {
			QString val;
			for (int i=1; i<lsval.size(); i++) {
				if (i>1) val += ":";
				val += lsval.at(i);
			}
			val = val.trimmed ();
			// DBGQS (lsval.at(0)+ " -> "+val);
			//-----------------------------------------------
			// GFS
			//-----------------------------------------------
            if ( lsval.at(0) == "gfs_run_date") {
                if (val.size()==8) {   // format: 20080523
                    QDateTime dt= QDateTime::fromString(val+"0000","yyyyMMddHHmm");
                    if (dt.isValid()) {
						result.insert ("gfs_run_date", dt.toString("yyyy-MM-dd"));
                    }
                }
            }
            if ( lsval.at(0) == "gfs_run_hour") {
				result.insert ("gfs_run_hour", val+" h UTC");
            }
            if ( lsval.at(0) == "gfs_update_time") {
				result.insert ("gfs_update_time", val);
            }
			//-----------------------------------------------
			// FNMOC-WW3
			//-----------------------------------------------
            if ( lsval.at(0) == "FNMOC-WW3_run_date") {
                if (val.size()==8) {   // format: 20080523
                    QDateTime dt= QDateTime::fromString(val+"0000","yyyyMMddHHmm");
                    if (dt.isValid()) {
						result.insert ("FNMOC-WW3_run_date", dt.toString("yyyy-MM-dd"));
                    }
                }
            }
            if ( lsval.at(0) == "FNMOC-WW3_run_hour") {
				result.insert ("FNMOC-WW3_run_hour", val+" h UTC");
            }
            if ( lsval.at(0) == "FNMOC-WW3_update_time") {
				result.insert ("FNMOC-WW3_update_time", val);
            }
            if ( lsval.at(0) == "FNMOC-WW3_current_job") {
				result.insert ("FNMOC-WW3_current_job", val);
            }
			//-----------------------------------------------
			// FNMOC-WW3-MED
			//-----------------------------------------------
            if ( lsval.at(0) == "FNMOC-WW3-MED_run_date") {
                if (val.size()==8) {   // format: 20080523
                    QDateTime dt= QDateTime::fromString(val+"0000","yyyyMMddHHmm");
                    if (dt.isValid()) {
						result.insert ("FNMOC-WW3-MED_run_date", dt.toString("yyyy-MM-dd"));
                    }
                }
            }
            if ( lsval.at(0) == "FNMOC-WW3-MED_run_hour") {
				result.insert ("FNMOC-WW3-MED_run_hour", val+" h UTC");
            }
            if ( lsval.at(0) == "FNMOC-WW3-MED_update_time") {
				result.insert ("FNMOC-WW3-MED_update_time", val);
            }
            if ( lsval.at(0) == "FNMOC-WW3-MED_current_job") {
				result.insert ("FNMOC-WW3-MED_current_job", val);
            }
			//-----------------------------------------------
			// MBLUE-NMM4
			//-----------------------------------------------
            if ( lsval.at(0) == "MBLUE-NMM4_run_date") {
                if (val.size()==8) {   // format: 20080523
                    QDateTime dt= QDateTime::fromString(val+"0000","yyyyMMddHHmm");
                    if (dt.isValid()) {
						result.insert ("MBLUE-NMM4_run_date", dt.toString("yyyy-MM-dd"));
                    }
                }
            }
            if ( lsval.at(0) == "MBLUE-NMM4_run_hour") {
				result.insert ("MBLUE-NMM4_run_hour", val+" h UTC");
            }
            if ( lsval.at(0) == "MBLUE-NMM4_update_time") {
				result.insert ("MBLUE-NMM4_update_time", val);
            }
            if ( lsval.at(0) == "MBLUE-NMM4_current_job") {
				result.insert ("MBLUE-NMM4_current_job", val);
            }
			//-----------------------------------------------
			// Messages
			//-----------------------------------------------
            if (lsval.at(0) == "client") {
				result.insert ("client", val);
            }
            if (lsval.at(0) == "current_job") {
				result.insert ("current_job", val);
            }
            if (lsval.at(0) == "message") {
				result.insert ("message", val);
            }
        }
    }
	return result;
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
    ar_statuses.insert("gfs", "NOAA-GFS");
    ar_statuses.insert("ico", "ICON Global");
    ar_statuses.insert("arp", "Arpege Global");
    ar_statuses.insert("ww3", "WW3");
    ar_statuses.insert("gwa", "GWAM");
    ar_statuses.insert("ewa", "EWAM");


    QHash<QString, QString>::iterator x;
    int i=0;
    for (x = ar_statuses.begin(); x != ar_statuses.end(); ++x)
    //var ix = ar_statuses.begin();
    //foreach ( a in ar_statuses.Iterator )
    //for ( int i=0; i< ar_statuses.count(); i++)
    {
        QString key = x.key();// ar_statuses.keys()[i];
        lig ++;
        ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
        //-------------------------
        lig ++;
        label = new QLabel(x.value(), frm);
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

        i++;
    }

//    //-------------------------
//	// NOAAA-GFS
//    //-------------------------
//    lig ++;
//    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
//    //-------------------------
//    lig ++;
//    label = new QLabel(tr("NOAA-GFS"), frm);
//	label->setFont (fontBold);
//    lay->addWidget( label,    lig,0, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel(tr("Forecast date :"), frm);
//    lay->addWidget( label,    lig,0, Qt::AlignRight);
//    lbRunDate = new QLabel("", frm);
//    lay->addWidget( lbRunDate, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("Update time :"), frm);
//    lay->addWidget (label,    lig,0, Qt::AlignRight);
//    lbGfsUpdateTime = new QLabel ("", frm);
//    lay->addWidget (lbGfsUpdateTime, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel(tr("Activity :"), frm);
//    lay->addWidget( label,    lig,0, Qt::AlignRight);
//    lbCurrentJob = new QLabel("", frm);
//    lay->addWidget( lbCurrentJob, lig,1, Qt::AlignLeft);
//    //-------------------------
//	// FNMOC-WW3
//    //-------------------------
//    lig ++;
//    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("FNMOC-WW3: Oceans"), frm);
//	label->setFont (fontBold);
//    lay->addWidget (label,    lig,0, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("Forecast date :"), frm);
//    lay->addWidget (label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_RunDate = new QLabel ("", frm);
//    lay->addWidget (lbFnmocWW3_RunDate, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("Update time :"), frm);
//    lay->addWidget (label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_UpdateTime = new QLabel ("", frm);
//    lay->addWidget (lbFnmocWW3_UpdateTime, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel(tr("Activity :"), frm);
//    lay->addWidget( label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_CurrentJob = new QLabel("", frm);
//    lay->addWidget (lbFnmocWW3_CurrentJob, lig,1, Qt::AlignLeft);
//    //-------------------------
//	// FNMOC-WW3-MED
//    //-------------------------
//    lig ++;
//    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("FNMOC-WW3: Mediterranean"), frm);
//	label->setFont (fontBold);
//    lay->addWidget (label,    lig,0, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("Forecast date :"), frm);
//    lay->addWidget (label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_Med_RunDate = new QLabel ("", frm);
//    lay->addWidget (lbFnmocWW3_Med_RunDate, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel (tr("Update time :"), frm);
//    lay->addWidget (label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_Med_UpdateTime = new QLabel ("", frm);
//    lay->addWidget (lbFnmocWW3_Med_UpdateTime, lig,1, Qt::AlignLeft);
//    //-------------------------
//    lig ++;
//    label = new QLabel(tr("Activity :"), frm);
//    lay->addWidget( label,    lig,0, Qt::AlignRight);
//    lbFnmocWW3_Med_CurrentJob = new QLabel("", frm);
//    lay->addWidget (lbFnmocWW3_Med_CurrentJob, lig,1, Qt::AlignLeft);


    //-------------------------
	// MBLUE-NMM4
    //-------------------------
//     lig ++;
//     ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
//     //-------------------------
//     lig ++;
//     label = new QLabel (tr("METEOBLUE-NMM"), frm);
// 	label->setFont (fontBold);
//     lay->addWidget (label,    lig,0, Qt::AlignLeft);
//     //-------------------------
//     lig ++;
//     label = new QLabel (tr("Forecast date :"), frm);
//     lay->addWidget (label,    lig,0, Qt::AlignRight);
//     lbMblueNMM4RunDate = new QLabel ("", frm);
//     lay->addWidget (lbMblueNMM4RunDate, lig,1, Qt::AlignLeft);
//     //-------------------------
//     lig ++;
//     label = new QLabel (tr("Update time :"), frm);
//     lay->addWidget (label,    lig,0, Qt::AlignRight);
//     lbMblueNMM4UpdateTime = new QLabel ("", frm);
//     lay->addWidget (lbMblueNMM4UpdateTime, lig,1, Qt::AlignLeft);
//     //-------------------------
//     lig ++;
//     label = new QLabel(tr("Activity :"), frm);
//     lay->addWidget( label,    lig,0, Qt::AlignRight);
//     lbMblueNMM4CurrentJob = new QLabel("", frm);
//     lay->addWidget (lbMblueNMM4CurrentJob, lig,1, Qt::AlignLeft);
    //-------------------------
	// Messages
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    lbMessage = new QLabel("", frm);
    lay->addWidget( lbMessage, lig,0,1,2, Qt::AlignLeft);

    return frm;
}











