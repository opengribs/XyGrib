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

#ifndef DIALOGSELECTMETAR_H
#define DIALOGSELECTMETAR_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QDesktopWidget>

#include "DialogBoxBase.h"
#include "Metar.h"

class DialogSelectMetar : public DialogBoxBase
{ Q_OBJECT
    public:
        DialogSelectMetar (QWidget *parent=NULL);
    
    public slots:
        void slotBtOK();
		
	signals:
		void metarListChanged ();
		
    private:
        QFrame *frameGui;
        QGridLayout *layout;        
        QPushButton *btOK;
		
		QList <Airport> allAirports;
		QMap <QString, QString> mapCountries;
		QMap <QPair <QString,QString>, QString> mapStates;
		
		QTreeWidget *treeWidget;
		
		void make_metar_tree ();

        QFrame * createFrameGui(QWidget *parent);
};


#endif
