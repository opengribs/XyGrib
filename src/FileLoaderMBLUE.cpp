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

#include <QApplication>
#include <QTextStream>
#include <QDomDocument>
#include <QMessageBox>

#include <cassert>

#include "FileLoaderMBLUE.h"
#include "Util.h"
#include "Version.h"

//-------------------------------------------------------------------------------
FileLoaderMBLUE::FileLoaderMBLUE (QNetworkAccessManager *manager, QWidget *parent)
			: FileLoader (manager)
{
	this->parent = parent;
    step = 0;
	downloadError = false;
	reply_step1 = NULL;
	reply_step2 = NULL;
	scriptpath = "/noaa/";
	scriptstock = "313O562/";
    zygriblog = "a07622b82b18524d2088c9b272bb3feeb0eb1737";
    zygribpwd = "61c9b2b17db77a27841bbeeabff923448b0f6388";
}
//-------------------------------------------------------------------------------
FileLoaderMBLUE::~FileLoaderMBLUE () 
{
	if (reply_step1) {
		reply_step1->close ();
		downloadError = true;
	}
	if (reply_step2) {
		reply_step2->close ();
		downloadError = true;
	}
}

//-------------------------------------------------------------------------------
void FileLoaderMBLUE::stop () 
{
	if (reply_step1) {
		reply_step1->close ();
		downloadError = true;
	}
	if (reply_step2) {
		reply_step2->close ();
		downloadError = true;
	}
}

//-------------------------------------------------------------------------------
void FileLoaderMBLUE::getMblueFile(
        float x0, float y0, float x1, float y1,
        float /*resolution*/, int interval, int days,
        bool wind, bool pressure, bool rain,
        bool cloud, bool temp, bool humid, bool isotherm0,
		bool tempMin, bool tempMax, bool snowDepth,
		bool snowCateg, bool frzRainCateg,
		bool CAPEsfc, 
		bool altitudeData200,
		bool altitudeData300,
		bool altitudeData500,
		bool altitudeData700,
		bool altitudeData850,
		bool GUSTsfc
	)
{
    QString page;
	step = 0;
    //----------------------------------------------------------------
    // Etape 1 : Demande la création du fichier Grib (nom en retour)
    //----------------------------------------------------------------
    QString parameters = "";
    if (wind) {
        parameters += "W";
    }
    if (GUSTsfc) {
        parameters += "G";
    }
    if (pressure) {
        parameters += "P";
    }
    if (rain) {
        parameters += "R";
    }
    if (cloud) {
        parameters += "C";
    }
    if (temp) {
        parameters += "T";
    }
    if (humid) {
        parameters += "H";
    }
	if (isotherm0) {
		parameters += "I";
    }    
    if (tempMin) {
        parameters += "m";
    }
    if (tempMax) {
        parameters += "M";
    }
    if (snowDepth) {
        parameters += "S";
    }
    if (snowCateg) {
        parameters += "s";
    }
    if (frzRainCateg) {
        parameters += "Z";
    }
    if (CAPEsfc) {
        parameters += "c";
    }
    if (altitudeData200) parameters += "2";
    if (altitudeData300) parameters += "3";
    if (altitudeData500) parameters += "5";
    if (altitudeData700) parameters += "7";
    if (altitudeData850) parameters += "8";
    if (parameters != "")
    {
        step = 1;
        emit signalGribSendMessage(
        		tr("Make file on server... Please wait..."));
        emit signalGribReadProgress(step, 0, 0);
		QString phpfilename;
		phpfilename = scriptpath+"getmbluefile.php?";
        QTextStream(&page) << phpfilename
                           << "but=prepfile"
                           << "&ver=1"
                           << "&hrs=" << interval
                           << "&htot=" << days*24
                           << "&la1=" <<  (y0)
                           << "&la2=" <<  (y1)
                           << "&lo1=" <<  (x0)
                           << "&lo2=" <<  (x1)
                           << "&par=" << parameters
                           << "&l=" << zygriblog
                           << "&m=" << zygribpwd
                           << "&client=" << Version::getCompleteName()
                           ;
		QNetworkRequest request = Util::makeNetworkRequest ("http://"+Util::getServerName()+page,x0,y0,x1,y1);
		reply_step1 = networkManager->get (request);
		connect (reply_step1, SIGNAL(downloadProgress (qint64,qint64)), 
				 this, SLOT(downloadProgress (qint64,qint64)));
		connect (reply_step1, SIGNAL(error(QNetworkReply::NetworkError)),
				 this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
		connect (reply_step1, SIGNAL(finished()),
				 this, SLOT(slotFinished_step1 ()));
    }
}

//-------------------------------------------------------------------------------
void FileLoaderMBLUE::slotNetworkError (QNetworkReply::NetworkError /*err*/)
{
	if (! downloadError) {
		if (sender() == reply_step1) {
			emit signalGribLoadError (reply_step1->errorString());
			downloadError = true;
		}
		else if (sender() == reply_step2) {
			emit signalGribLoadError (reply_step2->errorString());
			downloadError = true;
		}
	}
}
//-------------------------------------------------------------------------------
void FileLoaderMBLUE::downloadProgress (qint64 done, qint64 total)
{
	if (downloadError) {
		step = 1000;
		return;
	}
	if (sender() == reply_step2) {
		emit signalGribReadProgress (step, done, total);
	}
}
//-------------------------------------------------------------------------------
void FileLoaderMBLUE::slotFinished_step1 ()
{
	if (!downloadError) {
		QString page;
		//int unzipFileSize=0;
		//-------------------------------------------
		// Retour de l'étape 1 : préparation du fichier
		//-------------------------------------------
		QByteArray data = reply_step1->readAll ();
		
		QString strbuf (data);
		QStringList lsbuf = strbuf.split("\n");

		QString status;
        for (int i=0; i < lsbuf.size(); i++)
        {
            QStringList lsval = lsbuf.at(i).split(":");
            if (lsval.size() >= 2) {
                if (lsval.at(0) == "status")
                    status = lsval.at(1);
                else if (lsval.at(0) == "file") 
                    fileName = QString(lsval.at(1)).replace(".mbz","%20");
                else if (lsval.at(0) == "size")
                    fileSize = lsval.at(1).toInt();
                else if (lsval.at(0) == "checksum")
                    checkSumSHA1 = lsval.at(1);
                //else if (lsval.at(0) == "unzipsize")
                //    unzipFileSize = lsval.at(1).toInt();
				else if (lsval.at(0) == "message") {
					QString m = QUrl::fromPercentEncoding (lsval.at(1).toUtf8());
					QMessageBox::warning (parent, tr("Information"), m);
				}
            }
        }
        //--------------------------------------------------
        // Etape 2 : Demande le contenu du fichier
        //--------------------------------------------------
        if (status == "ok") {
            step = 2;
            QString s;
            s = tr("Total size : ") + QString("%1 ko").arg(fileSize/1024, 0);
            emit signalGribSendMessage(s);
            QTextStream(&page) << scriptpath
			<<"313O562/"<<fileName;
            emit signalGribStartLoadData();            
			QNetworkRequest request;
			request.setUrl (QUrl("http://"+Util::getServerName()+page) );
			reply_step2 = networkManager->get (request);
			connect (reply_step2, SIGNAL(downloadProgress (qint64,qint64)), 
					 this, SLOT(downloadProgress (qint64,qint64)));
			connect (reply_step2, SIGNAL(error(QNetworkReply::NetworkError)),
					 this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
			connect (reply_step2, SIGNAL(finished()),
					this, SLOT(slotFinished_step2 ()));
        }
		else {
			emit signalGribLoadError (status);
			downloadError = true;
		}
    }
}

//-------------------------------------------------------------------------------
void FileLoaderMBLUE::slotFinished_step2 ()
{
	if (!downloadError) {
		//--------------------------------------------------
		// Reçu le contenu du fichier
		//--------------------------------------------------
		step = 1000;
		arrayContent = reply_step2->readAll ();
		if (arrayContent.size() == 0) {
			emit signalGribLoadError (tr("Empty file."));
			return;
		}
		//--------------------------------------------------
		// Vérifie le checksum
		//--------------------------------------------------
		emit signalGribSendMessage(tr("CheckSum control"));
		if (Util::sha1 (arrayContent) == checkSumSHA1)
		{
			//--------------------------------------------------
			// Signale la fin du téléchargement
			//--------------------------------------------------
			emit signalGribSendMessage(tr("Finish")
							+ QString(" %1 ko").arg(arrayContent.size()/1024));
			emit signalGribDataReceived (&arrayContent, fileName.replace("%20",".mbz"));
		}
		else {
			emit signalGribLoadError(tr("Bad checksum."));
		}
	}
}


