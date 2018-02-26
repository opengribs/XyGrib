/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008 - Jacques Zaninetti - http://www.zygrib.org

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

#include <cassert>

#include <QVBoxLayout>

#include "AngleConverterDialog.h"

//-------------------------------------------------------
// POI_Editor: Constructor for edit and create a new POI
//-------------------------------------------------------
AngleConverterDialog::AngleConverterDialog (QWidget *parentWindow)
	: QDialog(parentWindow)
{
	angleConverterWidget = new AngleConverterWidget(this);
	assert(angleConverterWidget);
	btClose = new QPushButton(tr("Close"), this);
	assert(btClose);
	
	QVBoxLayout  *lay = new QVBoxLayout(this);
	assert (lay);
	lay->addWidget (angleConverterWidget);
	lay->addWidget (btClose, 0,Qt::AlignRight);
	
	setLayout(lay);
		
	connect(btClose,  SIGNAL(clicked()), this, SLOT(btCloseClicked()));
	setModal(true);
	show();
}

//---------------------------------------
AngleConverterDialog::~AngleConverterDialog()
{
	//printf("~AngleConverterDialog\n");
	delete angleConverterWidget;
}
//---------------------------------------
void AngleConverterDialog::reject()
{
	btCloseClicked();
}
//---------------------------------------
void AngleConverterDialog::btCloseClicked()
{
	delete this;
}

