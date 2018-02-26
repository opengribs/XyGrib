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

#include <QMessageBox>

#include "DialogBoxColumn.h"
#include "Util.h"

//==============================================================================
DialogBoxColumn::DialogBoxColumn (QWidget *parent,
			int nbColumn, QString windowTitle, QString title, int nbButtons)
	: DialogBoxBase (parent)
{
    setWindowTitle (windowTitle);
	accepted = false;
	if (nbColumn > MAXCOLS)
		nbColumn = MAXCOLS;
	if (nbColumn <= 1)
		nbColumn = 1;
	this->nbColumn = nbColumn;
	for (int i=0; i<nbColumn; i++) {
		currentLig [i] = 0;
		frameGui [i] = new QFrame (this);
		layGui [i] = new QGridLayout (frameGui [i]);
		layGui [i] ->setVerticalSpacing (0);
	}
	
    QFrame *ftmp;
    QVBoxLayout *layout = new QVBoxLayout (this);
	
    int lig=-1;
	if (title != "") {
		QFont font;
		font.setBold(true);
		QLabel *label = new QLabel(title, this);
		label->setFont(font);
		layout->addWidget (label, Qt::AlignCenter);
		lig ++;
		ftmp = new QFrame (this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget(ftmp);
	}
	//-------------------------
    lig ++;
	if (nbColumn == 1) {
		layout->addWidget (frameGui [0]);
	}
	else {
		ftmp = new QFrame (this); 
		QHBoxLayout *hlay = new QHBoxLayout (ftmp);
		hlay->addWidget (frameGui [0]);
		for (int i=1; i<nbColumn; i++) 
		{
			ftmp = new QFrame (ftmp); 
			ftmp->setFrameShape(QFrame::VLine); 
			hlay->addWidget (ftmp);
			hlay->addWidget (frameGui [1]);
		}
		layout->addWidget (ftmp);
	}
	
    //-------------------------
    lig ++;
    ftmp = new QFrame (this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget(ftmp);
    //-------------------------
	// Buttons in a frame
    lig ++;
	ftmp = new QFrame (this); 
    QHBoxLayout *hlay = new QHBoxLayout (ftmp);
    btOK     = new QPushButton (tr("Ok"), this);
	if (nbButtons > 1)
		btCancel = new QPushButton (tr("Cancel"), this);
	hlay->addStretch (0);
    hlay->addWidget (btOK);
	if (nbButtons > 1)
		hlay->addWidget (btCancel);
	hlay->addStretch (0);
    layout->addWidget (ftmp, Qt::AlignCenter);

    //-----------------------------------------------------------------------
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
	if (nbButtons > 1)
		connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
}

//-------------------------------------------------------------------------------
void DialogBoxColumn::slotBtOK()
{
	accepted = true;
    accept();
}
//-------------------------------------------------------------------------------
void DialogBoxColumn::slotBtCancel()
{
	accepted = false;
    reject();
}

//-----------------------------------------------------------------------------
void DialogBoxColumn::addLabeledWidget (int col, QString label, QWidget *widget)
{
	if (col>=0 && col < nbColumn)
	{
		layGui [col]->addWidget (new QLabel(label), currentLig [col], 0, Qt::AlignRight);
		layGui [col]->addWidget (widget, currentLig [col], 1);
		currentLig [col] ++;
	}
}








