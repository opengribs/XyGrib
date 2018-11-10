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
        void slotFinished();
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

        std::vector<QLabel* > ar_lbRunDate;
        std::vector<QLabel* > ar_lbCurrentJob;
        std::vector<QLabel* > ar_lbUpdateTime;
        QHash <QString, QString> ar_statuses;
        QList <QString> ar_statuses_keys;

        QLabel *lbMessage;
        QFrame * createFrameGui(QWidget *parent);

        QString sstStyleSheet =
                "QDialog, QFrame, QScrollBar {color: #dddddd; background-color: #666666;} "
                "QPushButton {background-color: #666666; border-style: outset; border-width: 2px;"
                            "border-color: #777777; padding: 6px;min-width: 6em; color: #ffffff;}"
                "QPushButton:pressed {border-width: 2px; border-style: inset;}"
                ;

};


#endif
