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

#ifndef METEOTABLEOPTIONSDIALOG_H
#define METEOTABLEOPTIONSDIALOG_H

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
 
#include "DialogBoxBase.h"
#include "ui_MeteotableOptionsDialog.h"
#include "DataDefines.h"
#include "Settings.h"

//-----------------------------------------------
class MeteotableOptionItem 
{
	public :
		MeteotableOptionItem (
				QString  name, DataCode dtc, int pos, bool visible, int interncode)
					{ this->name = name; this->dtc = dtc; this->pos = pos;  
					  this->visible = visible; 
					  this->interncode = interncode; }
		QString  name;
		DataCode dtc;
		bool visible;
		int  pos;
		int  interncode;
		
		static bool lessThan_byPos (MeteotableOptionItem* &a,
								    MeteotableOptionItem* &b)
					{ return a->pos < b->pos; }
		static bool lessThan_byName (MeteotableOptionItem* &a,
									 MeteotableOptionItem* &b)
					{ return a->name < b->name; }
};

//-----------------------------------------------
class DialogMeteotableOptions
		: public  DialogBoxBase,
		  private Ui_MeteotableOptionsDialog
{ Q_OBJECT
    public:
        DialogMeteotableOptions ();
        ~DialogMeteotableOptions ();
    
    public slots:
        void slotBtOK ();
        void slotBtCancel ();
        void slotChangeVisibleItems ();
	
    private:
		QList <MeteotableOptionItem *> listAllOptionItems;
		void  updateListWidgetsItems ();
		int   nbOptionItems (bool visibility);
		MeteotableOptionItem * getOptionItemByCode (int interncode);
		
		// setting (bool) : data visible or not in meteotable
        QString getSettingName_vis(uint gribCode) {
			return QString("MTableData_vis_%1").arg(gribCode,8,16,QLatin1Char('0'));
        }
		// setting (int) : vertical position in meteotable
        QString getSettingName_pos(uint gribCode) {
			return QString("MTableData_pos_%1").arg(gribCode,8,16,QLatin1Char('0'));
        }
        
        void initInterface ();
        void createDataLists ();
        //void addItemInDataList (QString name, int code, bool isVisible, int position);
        
		int  currentItemCode;
		void addData (QString title, 
					uchar grbtype, uchar leveltype, uint level,
					bool defaultvis, int defaultpos );
};


#endif
