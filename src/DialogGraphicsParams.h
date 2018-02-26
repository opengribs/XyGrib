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

#ifndef DIALOGGRAPHICSPARAMS_H
#define DIALOGGRAPHICSPARAMS_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include "ui_GraphicsParamsDialog.h"
#include "DialogBoxColumn.h"

//-------------------------------------------
class DialogChooseLang : public DialogBoxColumn 
{ Q_OBJECT
	public:
		DialogChooseLang (QWidget *parent, QString defaultlang);
		QString getLanguage ();
		
	private:
		QRadioButton * addLanguage (QString name, QString iconfile);
		QButtonGroup *btGroup;
		
		QRadioButton *bt_de;
		QRadioButton *bt_en;
		QRadioButton *bt_es;
		QRadioButton *bt_fi;
		QRadioButton *bt_fr;
		QRadioButton *bt_it;
		QRadioButton *bt_nl;
		QRadioButton *bt_pt;
		QRadioButton *bt_ru;
		QRadioButton *bt_cz;
		QRadioButton *bt_gr;
		// A. Degwerth [Cassidian]: added RadioButton for Arabic language
		QRadioButton *bt_ar;
};


//==============================================================
class DialogGraphicsParams
		: public  DialogBoxBase,
		  private Ui_GraphicsParamsDialog
{ Q_OBJECT
    public:
        DialogGraphicsParams (QWidget *parent=NULL);
    
    public slots:
        void slotBtOK ();
        void slotBtCancel ();
    
    
    private:
        
        
        void initInterface();
};


#endif
