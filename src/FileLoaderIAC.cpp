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

#include "FileLoaderIAC.h"
#include "Util.h"
#include "Version.h"

//-------------------------------------------------------------------------------
FileLoaderIAC::FileLoaderIAC (QNetworkAccessManager *manager, QWidget *parent)
			: FileLoader (manager)
{
	this->parent = parent;
    step = 0;
	downloadError = false;
	reply_step1 = NULL;
    zygriblog = "a07622b82b18524d2088c9b272bb3feeb0eb1737";
    zygribpwd = "61c9b2b17db77a27841bbeeabff923448b0f6388";
}
//-------------------------------------------------------------------------------
FileLoaderIAC::~FileLoaderIAC () {
	if (reply_step1) {
		reply_step1->deleteLater ();
		reply_step1 = NULL;
	}
}

//-------------------------------------------------------------------------------
void FileLoaderIAC::stop () {
	if (reply_step1) {
		reply_step1->deleteLater ();
		reply_step1 = NULL;
	}
}

//-------------------------------------------------------------------------------
void FileLoaderIAC::getFile (FileModeIAC IACmode)
{
    QString page;

	//----------------------------------------------------------------
    // Etape 1 : Demande la création du fichier IAC
    //----------------------------------------------------------------
	step = 1;
	emit signalIACsendMessage(tr("Make file on server"));
	
	emit signalIACreadProgress(step, 0, 0);
	
	if (IACmode == ANALYSE_FILE) {
		QTextStream(&page) << "/noaa/getNoaaIacFile.php?"
						<< "but=analyse"
						<< "&client=" << Version::getCompleteName()
						;
	}
	else {   // if (IACmode == FORECAST_FILE) {
		QTextStream(&page) << "/noaa/getNoaaIacFile.php?"
						<< "but=forecast"
						<< "&client=" << Version::getCompleteName()
						;
	}
//page="/headers.php";
	QNetworkRequest request = Util::makeNetworkRequest ("http://"+Util::getServerName()+page,-50,80,40,35);
	reply_step1 = networkManager->get (request);
	connect (reply_step1, SIGNAL(downloadProgress (qint64,qint64)), 
					this, SLOT(downloadProgress (qint64,qint64)));
	connect (reply_step1, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
	connect (reply_step1, SIGNAL(finished()),
					this, SLOT(slotFinished ()));
}
//-------------------------------------------------------------------------------
void FileLoaderIAC::slotNetworkError (QNetworkReply::NetworkError /*err*/)
{
	if (! downloadError) {
		if (sender() == reply_step1) {
			emit signalIACloadError (reply_step1->errorString());
			downloadError = true;
		}
	}
}
//-------------------------------------------------------------------------------
void FileLoaderIAC::downloadProgress (qint64 done, qint64 total)
{
	if (downloadError) {
		step = 1000;
		return;
	}
    emit signalIACreadProgress (step, done, total);
}
//-------------------------------------------------------------------------------
void FileLoaderIAC::slotFinished ()
{
	if (!downloadError) {
		//--------------------------------------------------
		// Reçu le contenu du fichier IAC
		//--------------------------------------------------
		step = 1000;
		arrayContent = reply_step1->readAll ();
		if (arrayContent.size() == 0) {
			emit signalIACloadError (tr("Empty file."));
			return;
		}
		if (reply_step1->hasRawHeader ("Pragma")) {
			QString rep = reply_step1->rawHeader("Pragma");
            QStringList lsval = rep.split(":");
            if (lsval.size() >= 2) {
				if (lsval.at(0) == "message") {
					QString m = QUrl::fromPercentEncoding (lsval.at(1).toUtf8());
					QMessageBox::warning (parent, tr("Information"), m);
				}
            }
		}

		fileName = "IACfile";
		if (reply_step1->hasRawHeader ("content-disposition"))
		{
			// Look for header entry:
			//     "content-disposition : attachment; filename=FSXX21_251200_20081125"
			QString hval = reply_step1->rawHeader ("content-disposition");
			// Extract file name from : "attachment; filename=FSXX21_251200_20081125"
			int ind = hval.lastIndexOf('=');
			if (ind > 0)
				fileName = hval.right( hval.size()-ind-1 );
		}

		emit signalIACsendMessage (tr("Finish"));
		emit signalIACdataReceived (&arrayContent, fileName);
	}
}


