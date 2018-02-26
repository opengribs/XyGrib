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

Download GRIB File on zygrib server

*************************/

#ifndef FILELOADER_MBLUE_H
#define FILELOADER_MBLUE_H

#include <QObject>
#include <QtNetwork>
#include <QBuffer>

#include "FileLoader.h"

class FileLoaderMBLUE : public QObject, FileLoader
{ Q_OBJECT
    public:
        FileLoaderMBLUE (QNetworkAccessManager *manager, QWidget *parent);
        ~FileLoaderMBLUE();
        
        void getMblueFile(
				float x0, float y0, float x1, float y1,
				float resolution, int interval, int days,
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
			);
        void stop();
        
    private:
		QString scriptpath;
		QString scriptname;
		QString scriptstock;
        QByteArray arrayContent;
        QWidget *parent;
        
        QString fileName;
        QString checkSumSHA1;
        int     step;
        int     fileSize;

        QString zygriblog;
        QString zygribpwd;
		
		QNetworkReply *reply_step1;
		QNetworkReply *reply_step2;
		bool downloadError;

    public slots:
        void downloadProgress (qint64 done, qint64 total);
		void slotNetworkError (QNetworkReply::NetworkError);
		void slotFinished_step1 ();
		void slotFinished_step2 ();

    signals:
        void signalGribDataReceived (QByteArray *content, QString);
        void signalGribReadProgress (int step, int done, int total);
        void signalGribSendMessage (QString msg);
        void signalGribStartLoadData ();
        void signalGribLoadError (QString msg);
};


#endif
