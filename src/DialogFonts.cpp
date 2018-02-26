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
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <QFontDialog>

#include "DialogFonts.h"
#include "Util.h"

//==============================================================================
//==============================================================================
FontSelector::FontSelector (FontCode code, QString txtlabel, QWidget *parent)
	: QWidget(parent)
{
	this->code = code;
	this->font = Font::getFont(code);


	QLabel *label = new QLabel(txtlabel+ " :");

	QString txt = font.family()+ QString(" %1"). arg(font.pointSize());
	
	btfont = new QPushButton (txt);
	btfont->setFont (font);

	btdefault = new QPushButton (tr("Reset"));
	btdefault->setFont (Font::getDefaultFont (code));

	connect(btfont, SIGNAL(clicked()), this, SLOT(btfontClicked()));
	connect(btdefault, SIGNAL(clicked()), this, SLOT(btdefaultClicked()));
	
	QGridLayout * lay = new QGridLayout(this);
	lay->setContentsMargins (0,2,0,2);
	lay->addWidget( label, 0,0, Qt::AlignRight);
	lay->addWidget( btfont, 0,1, Qt::AlignLeft);
	lay->addWidget( btdefault, 0,2, Qt::AlignCenter);
}
//-----------------------------------------------------
void FontSelector::btdefaultClicked()
{
	font = Font::getDefaultFont (code);
	QString txt = font.family()+ QString(" %1"). arg(font.pointSize());
	btfont->setFont (font);
	btfont->setText (txt);
}
//-----------------------------------------------------
void FontSelector::btfontClicked()
{
	bool ok;
	QFont newfont = QFontDialog::getFont (&ok, font, this, tr("Choose a font"));
	if (ok) {
		font = newfont;
		QString txt = font.family()+ QString(" %1"). arg(font.pointSize());
		btfont->setFont (font);
		btfont->setText (txt);
	}
}
 
//==============================================================================
//==============================================================================
DialogFonts::DialogFonts (QWidget *parent) : DialogBoxBase (parent)
{
    setWindowTitle(tr("Fonts"));
    QFrame *ftmp;
    QLabel *label;
    frameGui = createFrameGui(this);
    
    layout = new QGridLayout(this);
    int lig=0;
    //-------------------------
    lig ++;
    QFont font;
    font.setBold(true);
    label = new QLabel(tr("Choice of fonts"), this);
    label->setFont(font);
    layout->addWidget( label,    lig,0, 1,-1, Qt::AlignCenter);
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);

    lig ++;
    layout->addWidget( frameGui,    lig,0, 1,-1, Qt::AlignCenter);
	
	
	//-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    lig ++;
    btOK     = new QPushButton(tr("Ok"), this);
    btCancel = new QPushButton(tr("Cancel"), this);
    layout->addWidget( btOK,    lig,0);
    layout->addWidget( btCancel, lig,1);
    

    //===============================================================
    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
}

//-------------------------------------------------------------------------------
void DialogFonts::slotBtOK()
{
    accept();
}
//-------------------------------------------------------------------------------
void DialogFonts::slotBtCancel()
{
    reject();
}
//-------------------------------------------------------------------
QFont DialogFonts::getFontItem (FontCode code) {
	FontSelector *fsel =  hashFontSelectors.value (code);
	return fsel->getFont();
}

//=============================================================================
// GUI
//=============================================================================
QFrame *DialogFonts::createFrameGui(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
	int lig, col;
	FontSelector *fontsel;
	FontCode code;

	QGridLayout  *lay = new QGridLayout(frm);
	lay->setContentsMargins (0,0,0,0);
	
	//-------------------------
	// Colonne 1
	//-------------------------
    lig = 1;
	col = 0;
	code = FONT_Default;
    fontsel = new FontSelector(code, tr("Main font"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_ComboBox;
	fontsel = new FontSelector(code, tr("Dates list"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MenuBar;
    fontsel = new FontSelector(code, tr("Menus"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_StatusBar;
    fontsel = new FontSelector(code, tr("Status bar"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_GRIB_Temp;
    fontsel = new FontSelector(code, tr("Temperature"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_IsolineLabel;
    fontsel = new FontSelector(code, tr("Isobars"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_GRIB_PressHL;
    fontsel = new FontSelector(code, tr("Pressure H/L"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	
	//-------------------------
	// Colonne 2
	//-------------------------
    lig = 1;
	col = 1;
	code = FONT_MapCity_1;
    fontsel = new FontSelector(code, tr("Cities (level 1)"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MapCity_2;
    fontsel = new FontSelector(code, tr("Cities (level 2)"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MapCity_3;
    fontsel = new FontSelector(code, tr("Cities (level 3)"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MapCity_4;
    fontsel = new FontSelector(code, tr("Cities (level 4)"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MapCity_5;
    fontsel = new FontSelector(code, tr("Cities (level 5)"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;
	code = FONT_MapCountry;
    fontsel = new FontSelector(code, tr("Countries names"), frm);
    lay->addWidget( fontsel,    lig,col, Qt::AlignRight);
	hashFontSelectors.insert (code, fontsel);
    lig ++;

	//-------------------------

    return frm;
}









