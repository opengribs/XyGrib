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

#ifndef DIALOGSERVERSTATUS_H
#define DIALOGSERVERSTATUS_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QtNetwork>
#include <QBuffer>

#include "DialogBoxBase.h"

class DialogServerStatus : public  DialogBoxBase
{ Q_OBJECT
    public:
        DialogServerStatus (QNetworkAccessManager *manager, 
							QWidget *parent=NULL);
        ~DialogServerStatus ();
    
    public slots:
        void slotBtOK();
        void slotBtCancel();
        void downloadProgress_step1 (qint64 done, qint64 total);
		void slotNetworkError (QNetworkReply::NetworkError);
    
    private:
		QHash <QString,QString> readData (const QByteArray &data);
		QString getData (const QHash <QString,QString> &data, const QString &key);
        
		QString getNoaaRunDate (QStringList lsbuf);
        QString getNoaaRunHour (QStringList lsbuf);
        QString getServerCurrentJob (QStringList lsbuf);
        QString getClient (QStringList lsbuf);
        
        QFrame *frameGui;
        QGridLayout *layout;
        QPushButton *btOK;
		
		QString errorMessage;
		QNetworkReply *reply_step1;
		bool downloadError;
        
        QTime   timeLoad;
        QLabel *lbResponseStatus;
        QLabel *lbRunDate;
        QLabel *lbCurrentJob;
        QLabel *lbGfsUpdateTime;
		
        QLabel *lbFnmocWW3_RunDate;
        QLabel *lbFnmocWW3_UpdateTime;
        QLabel *lbFnmocWW3_CurrentJob;
		
        QLabel *lbFnmocWW3_Med_RunDate;
        QLabel *lbFnmocWW3_Med_UpdateTime;
        QLabel *lbFnmocWW3_Med_CurrentJob;
		
        QLabel *lbMblueNMM4RunDate;
        QLabel *lbMblueNMM4UpdateTime;
        QLabel *lbMblueNMM4CurrentJob;
        
        QLabel *lbMessage;
        QFrame * createFrameGui(QWidget *parent);
};


#endif
