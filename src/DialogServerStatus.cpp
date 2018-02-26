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
	page = "/noaa/getGfsRunLog.php";
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
void DialogServerStatus::downloadProgress_step1 (qint64 done, qint64 total)
{
	QString strDur; 
    int tp = timeLoad.elapsed();
    QTextStream(&strDur) << tp << " ms";
	
	if (downloadError) {
        lbResponseStatus->setText (tr("error")+" ("+strDur+")");
        lbMessage->setText("Error: "+errorMessage);
    }
    else if (done == total)
    {
        lbResponseStatus->setText (tr("ok")+" ("+strDur+")");
        
		QByteArray data = reply_step1->readAll ();
		QHash <QString,QString> allNoaa = readData (data);
		QString sdate, shour;
		//------------------------------------------------------
		sdate = getData (allNoaa, "gfs_run_date");
		shour = getData (allNoaa, "gfs_run_hour");
        lbRunDate->setText (sdate+"     "+shour);
        lbGfsUpdateTime->setText (getData (allNoaa, "gfs_update_time"));
        lbCurrentJob->setText (getData (allNoaa, "current_job"));
		//------------------------------------------------------
		sdate = getData (allNoaa, "FNMOC-WW3_run_date");
		shour = getData (allNoaa, "FNMOC-WW3_run_hour");
        lbFnmocWW3_RunDate->setText (sdate+"     "+shour);
        lbFnmocWW3_UpdateTime->setText (getData (allNoaa, "FNMOC-WW3_update_time"));
        lbFnmocWW3_CurrentJob->setText (getData (allNoaa, "FNMOC-WW3_current_job"));
		//------------------------------------------------------
		sdate = getData (allNoaa, "FNMOC-WW3-MED_run_date");
		shour = getData (allNoaa, "FNMOC-WW3-MED_run_hour");
        lbFnmocWW3_Med_RunDate->setText (sdate+"     "+shour);
        lbFnmocWW3_Med_UpdateTime->setText (getData (allNoaa, "FNMOC-WW3-MED_update_time"));
        lbFnmocWW3_Med_CurrentJob->setText (getData (allNoaa, "FNMOC-WW3-MED_current_job"));
		//------------------------------------------------------
// 		sdate = getData (allNoaa, "MBLUE-NMM4_run_date");
// 		shour = getData (allNoaa, "MBLUE-NMM4_run_hour");
//         lbMblueNMM4RunDate->setText (sdate+"     "+shour);
//         lbMblueNMM4UpdateTime->setText (getData (allNoaa, "MBLUE-NMM4_update_time"));
//         lbMblueNMM4CurrentJob->setText (getData (allNoaa, "MBLUE-NMM4_current_job"));
		//------------------------------------------------------
		lbMessage->setText (allNoaa.value ("message"));
    }
}

//-------------------------------------------------------------------------------
QString DialogServerStatus::getData (const QHash <QString,QString> &data, const QString &key)
{
    QString rep = data.value (key, "");
/*    if (rep == "")
		rep=tr("invalid format");*/
	return rep;
}

//-------------------------------------------------------------------------------
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
    //-------------------------
	// NOAAA-GFS
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    label = new QLabel(tr("NOAA-GFS"), frm);
	label->setFont (fontBold);
    lay->addWidget( label,    lig,0, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Forecast date :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lbRunDate = new QLabel("", frm);
    lay->addWidget( lbRunDate, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel (tr("Update time :"), frm);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
    lbGfsUpdateTime = new QLabel ("", frm);
    lay->addWidget (lbGfsUpdateTime, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Activity :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lbCurrentJob = new QLabel("", frm);
    lay->addWidget( lbCurrentJob, lig,1, Qt::AlignLeft);
    //-------------------------
	// FNMOC-WW3
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    label = new QLabel (tr("FNMOC-WW3: Oceans"), frm);
	label->setFont (fontBold);
    lay->addWidget (label,    lig,0, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel (tr("Forecast date :"), frm);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_RunDate = new QLabel ("", frm);
    lay->addWidget (lbFnmocWW3_RunDate, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel (tr("Update time :"), frm);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_UpdateTime = new QLabel ("", frm);
    lay->addWidget (lbFnmocWW3_UpdateTime, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Activity :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_CurrentJob = new QLabel("", frm);
    lay->addWidget (lbFnmocWW3_CurrentJob, lig,1, Qt::AlignLeft);
    //-------------------------
	// FNMOC-WW3-MED
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); lay->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    label = new QLabel (tr("FNMOC-WW3: Mediterranean"), frm);
	label->setFont (fontBold);
    lay->addWidget (label,    lig,0, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel (tr("Forecast date :"), frm);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_Med_RunDate = new QLabel ("", frm);
    lay->addWidget (lbFnmocWW3_Med_RunDate, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel (tr("Update time :"), frm);
    lay->addWidget (label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_Med_UpdateTime = new QLabel ("", frm);
    lay->addWidget (lbFnmocWW3_Med_UpdateTime, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Activity :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    lbFnmocWW3_Med_CurrentJob = new QLabel("", frm);
    lay->addWidget (lbFnmocWW3_Med_CurrentJob, lig,1, Qt::AlignLeft);
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











