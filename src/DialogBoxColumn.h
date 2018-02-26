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

#ifndef DIALOGBOXCOLUMNS_H
#define DIALOGBOXCOLUMNS_H

#include <QDialog>
#include <QFrame>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "DialogBoxBase.h"

class DialogBoxColumn  : public DialogBoxBase
{ Q_OBJECT
	public :
		static const int MAXCOLS = 4;
		
		DialogBoxColumn ( QWidget *parent, 
						  int nbColumn, 
						  QString windowTitle, QString title, 
						  int nbButtons=2 );
				
		bool isAccepted () {return accepted;}
		
	protected :
		void addLabeledWidget (int column, QString label, QWidget *widget); 
    
	private slots:
        void slotBtOK();
        void slotBtCancel();

	private :
		bool accepted;
		int nbColumn;
		int currentLig [MAXCOLS];
		QFrame 		*frameGui [MAXCOLS];
		QGridLayout *layGui   [MAXCOLS];
		
        QPushButton *btOK;
        QPushButton *btCancel;
};




#endif
