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

/*************************

Download IAC File on zygrib server

*************************/

#ifndef FILELOADER_IAC_H
#define FILELOADER_IAC_H

#include <QObject>
#include <QtNetwork>
#include <QBuffer>
        
#include "FileLoader.h"

class FileLoaderIAC : public QObject, FileLoader
{ Q_OBJECT
    public:
		enum FileModeIAC
			{
				ANALYSE_FILE,
			  	FORECAST_FILE
			};
    
        FileLoaderIAC (QNetworkAccessManager *manager, QWidget *parent);
        ~FileLoaderIAC();
        
        void getFile(FileModeIAC IACmode);
        void stop();
        
    private:
        QByteArray arrayContent;
        QWidget *parent;
        QString fileName;
        int     fileSize;
        int     step;
        QString zygriblog;
        QString zygribpwd;
		
		QNetworkReply *reply_step1;
		bool downloadError;

    public slots:
        void downloadProgress (qint64 done, qint64 total);
		void slotNetworkError (QNetworkReply::NetworkError);
		void slotFinished ();
		
    signals:
        void signalIACdataReceived(QByteArray *content, QString);
        void signalIACreadProgress(int step, int done, int total);
        void signalIACsendMessage(QString msg);
        void signalIACstartLoadData();
        void signalIACloadError(QString msg);
};


#endif
