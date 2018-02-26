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

#ifndef POI_EDITOR_H
#define POI_EDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>

#include "Projection.h"
#include "POI.h"
#include "Util.h"

#include "ui_PoiEditorDialog.h"

class POI_Editor :
		public  QDialog,
		private Ui_PoiEditorDialog
{
Q_OBJECT
    public:
    
    	// Constructor for edit an existing POI
        POI_Editor(POI *poi, QWidget *parent);
        
        // Constructor for edit and create a new POI
        POI_Editor(uint code, double lon, double lat,
        			Projection *proj, QWidget *ownerMeteotable, QWidget *parentWindow);
        
        ~POI_Editor();
    
    private:
    	POI   *poi;
    	bool  modeCreation;
		
		void closeEvent(QCloseEvent *) {delete this;};
		void updateInterface();
	
	private slots:
		void reject();
		
		void btOkClicked();
		void btCancelClicked();
		void btDeleteClicked();

};


#endif
