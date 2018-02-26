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

#ifndef DIALOGLOAD_IAC_H
#define DIALOGLOAD_IAC_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>

#include "DialogBoxBase.h"
#include "FileLoaderIAC.h"

class DialogLoadIAC : public DialogBoxBase
{ Q_OBJECT
    public:
        DialogLoadIAC (QNetworkAccessManager *manager, QWidget *parent);
        ~DialogLoadIAC ();
		
		static QString getFile (QNetworkAccessManager *manager, QWidget *parent);


    public slots:
        void slotBtDownload();
        void slotBtCancel();
        
        void slotLoaderDataReceived(QByteArray *content, QString fileName);
        void slotLoaderReadProgress(int step, int done, int total);
        void slotLoaderFileError(QString error);
        void slotLoaderMessage(QString msg);
        void slotLoaderStartLoadData();
		void slotFinished (int result);

    private:
        FileLoaderIAC	*IACloader;
		QNetworkAccessManager *networkManager;
		QString  savedFileName;
		
        bool     loadInProgress;
        QTime    timeLoad;
        
        QPushButton *btAnalyse;
        QPushButton *btForecast;
        QPushButton *btCancel;

        QProgressBar *progressBar;
        QLabel       *labelMsg;
        
        QFrame *createFrameButtonsZone(QWidget *parent);
                
        void    closeEvent (QCloseEvent *event);
};


#endif
