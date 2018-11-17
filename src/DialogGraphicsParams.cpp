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

#include <QMessageBox>
#include <cmath>
#include <cassert>

#include <QPainter>
#include <QColorDialog>
#include <QFontDialog>

#include "DialogGraphicsParams.h"
#include "Util.h"

//===========================================================================
// DialogGraphicsParams
//===========================================================================
DialogChooseLang::DialogChooseLang (QWidget *parent, const QString& defaultlang)
		: DialogBoxColumn ( parent, 1, 
                            "Welcome to XyGrib",
						    "Choose your language",   
						   1 )
{
    this->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
	btGroup = new QButtonGroup (this);
	assert (btGroup);
	// A. Degwerth [Cassidian]: added Arabic language selection
	bt_ar = addLanguage ("العربية",   Util::pathImg("drapeau_ar.png"));
	bt_cz = addLanguage ("Čeština",   Util::pathImg("drapeau_cz.png"));
	bt_de = addLanguage ("Deutsch",   Util::pathImg("drapeau_de.png"));
	bt_en = addLanguage ("English",   Util::pathImg("drapeau_en.png"));
	bt_es = addLanguage ("Español",   Util::pathImg("drapeau_es.png"));
	bt_fi = addLanguage ("Finnish",   Util::pathImg("drapeau_fi.png"));
    bt_fr = addLanguage ("Français",  Util::pathImg("drapeau_fr.png"));
    bt_he = addLanguage ("עברית",  Util::pathImg("drapeau_he.png"));
    bt_it = addLanguage ("Italiano",  Util::pathImg("drapeau_it.png"));
	bt_nl = addLanguage ("Dutch",     Util::pathImg("drapeau_nl.png"));
	bt_pt = addLanguage ("Português (Brasil)", Util::pathImg("drapeau_pt.png"));
	bt_ru = addLanguage ("Русско",    Util::pathImg("drapeau_ru.png"));
	bt_gr = addLanguage ("Ελληνικά",  Util::pathImg("drapeau_gr.png"));
	
	if (defaultlang == "de")
		bt_de->setChecked (true);
	else if (defaultlang == "en")
		bt_en->setChecked (true);
	else if (defaultlang == "es")
		bt_es->setChecked (true);
	else if (defaultlang == "fi")
		bt_fi->setChecked (true);
    else if (defaultlang == "fr")
        bt_fr->setChecked (true);
    else if (defaultlang == "he")
        bt_he->setChecked (true);
    else if (defaultlang == "it")
		bt_it->setChecked (true);
	else if (defaultlang == "nl")
		bt_nl->setChecked (true);
	else if (defaultlang == "pt")
		bt_pt->setChecked (true);
	else if (defaultlang == "ru")
		bt_ru->setChecked (true);
	else if (defaultlang == "cz")
		bt_ru->setChecked (true);
	// A. Degwerth [Cassidian]: added Arabic language selection
	else if (defaultlang == "ar")
		bt_ar->setChecked (true);
	else if (defaultlang == "gr")
		bt_gr->setChecked (true);
	else
		bt_en->setChecked (true);
}	
//--------------------------------------------------------------------------------	
QString DialogChooseLang::getLanguage ()
{
	if (bt_de->isChecked())
		return "de";
	else if (bt_en->isChecked())
		return "en";
	else if (bt_es->isChecked())
		return "es";
	else if (bt_fi->isChecked())
		return "fi";
    else if (bt_fr->isChecked())
        return "fr";
    else if (bt_he->isChecked())
        return "he";
    else if (bt_it->isChecked())
		return "it";
	else if (bt_nl->isChecked())
		return "nl";
	else if (bt_pt->isChecked())
		return "pt";
	else if (bt_ru->isChecked())
		return "ru";
	else if (bt_cz->isChecked())
		return "cz";
	// A. Degwerth [Cassidian]: added Arabic language selection
	else if (bt_ar->isChecked())
		return "ar";
	else if (bt_gr->isChecked())
		return "gr";
	else
		return "en";
}
//--------------------------------------------------------------------------------	
QRadioButton * DialogChooseLang::addLanguage (const QString& name, const QString& iconfile)
{
	QRadioButton *bt = new QRadioButton (name, this);
	assert (bt);
	bt->setIcon (QIcon(iconfile));
	
	btGroup->addButton (bt);
	addLabeledWidget (0, "", bt);
	return bt;
}
	
	
//===========================================================================
// DialogGraphicsParams
//===========================================================================
DialogGraphicsParams::DialogGraphicsParams (QWidget *parent) : DialogBoxBase (parent)
{
	setupUi(this);
	initInterface();
    
    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
}

//-------------------------------------------------------------------------------
void DialogGraphicsParams::slotBtOK()
{
	QHash <QString, QVariant> h;
	h.insert("seaColor", inputSeaColor->getColor());
	h.insert("landColor", inputLandColor->getColor());
	h.insert("backgroundColor", inputBgColor->getColor());
	
	h.insert("seaBordersLineWidth", inputSeaBordersLine->getLineWidth());
	h.insert("seaBordersLineColor", inputSeaBordersLine->getLineColor());
	h.insert("boundariesLineWidth", inputBoundariesLine->getLineWidth());
	h.insert("boundariesLineColor", inputBoundariesLine->getLineColor());
	h.insert("riversLineWidth",   inputRiversLine->getLineWidth());
	h.insert("riversLineColor",   inputRiversLine->getLineColor());
	h.insert("isobarsLineWidth",  inputIsobarsLine->getLineWidth());
	h.insert("isobarsLineColor",  inputIsobarsLine->getLineColor());
	h.insert("isotherms0LineWidth",  inputIsotherms0Line->getLineWidth());
	h.insert("isotherms0LineColor",  inputIsotherms0Line->getLineColor());
	h.insert("isotherms_LineWidth",  inputIsotherms_Line->getLineWidth());
	h.insert("isotherms_LineColor",  inputIsotherms_Line->getLineColor());
	h.insert("linesThetaE_LineWidth",  inputThetaE_Line->getLineWidth());
	h.insert("linesThetaE_LineColor",  inputThetaE_Line->getLineColor());
	h.insert("geopotentials_LineWidth",  inputGeopotentials_Line->getLineWidth());
	h.insert("geopotentials_LineColor",  inputGeopotentials_Line->getLineColor());
	
	h.insert("cloudsColorMode", inputCloudsColorMode->itemData(inputCloudsColorMode->currentIndex()).toString());
	Util::setSettings(h);
    accept();
}
//-------------------------------------------------------------------------------
void DialogGraphicsParams::slotBtCancel()
{
    reject();
}

//=============================================================================
// GUI
//=============================================================================
void DialogGraphicsParams::initInterface()
{
	inputBgColor->setColor(Util::getSetting("backgroundColor", QColor(0,0,45)).value<QColor>());
	inputBgColor->setDefaultColor(QColor(0,0,45));

    inputSeaColor->setColor(Util::getSetting("seaColor", QColor(67,193,183)).value<QColor>());
    inputSeaColor->setDefaultColor(QColor(67,193,183));
	
	inputLandColor->setColor(Util::getSetting("landColor", QColor(200,200,120)).value<QColor>());
	inputLandColor->setDefaultColor(QColor(200,200,120));
	
	inputSeaBordersLine->setValues (
				Util::getSetting("seaBordersLineWidth", 1.8).toDouble(),
				Util::getSetting("seaBordersLineColor", QColor(40,45,30)).value<QColor>(),
				1.6,  QColor(40,45,30) );
	inputBoundariesLine->setValues (
				Util::getSetting("boundariesLineWidth", 1.4).toDouble(),
				Util::getSetting("boundariesLineColor", QColor(40,40,40)).value<QColor>(),
				1.4,  QColor(40,40,40) );
	inputRiversLine->setValues (
				Util::getSetting("riversLineWidth", 1.0).toDouble(),
				Util::getSetting("riversLineColor", QColor(50,50,150)).value<QColor>(),
				1.0,  QColor(50,50,150) );
	inputIsobarsLine->setValues (
				Util::getSetting("isobarsLineWidth", 2.0).toDouble(),
				Util::getSetting("isobarsLineColor", QColor(80,80,80)).value<QColor>(),
				2.0,  QColor(80,80,80) );
	inputIsotherms0Line->setValues (
				Util::getSetting("isotherms0LineWidth", 1.6).toDouble(),
				Util::getSetting("isotherms0LineColor", QColor(200,120,100)).value<QColor>(),
				1.6,  QColor(200,120,100) );
	inputIsotherms_Line->setValues (
				Util::getSetting("isotherms_LineWidth", 1.6).toDouble(),
				Util::getSetting("isotherms_LineColor", QColor(40,40,150)).value<QColor>(),
				1.6,  QColor(40,40,150) );
	inputThetaE_Line->setValues (
				Util::getSetting("linesThetaE_LineWidth", 1.6).toDouble(),
				Util::getSetting("linesThetaE_LineColor", QColor(40,140,40)).value<QColor>(),
				1.6,  QColor(40,140,40) );
	inputGeopotentials_Line->setValues (
				Util::getSetting("geopotentials_LineWidth", 1.6).toDouble(),
				Util::getSetting("geopotentials_LineColor", QColor(0, 140,0)).value<QColor>(),
				0.3,  QColor(0,140,0) );

	inputCloudsColorMode->addItem (tr("100 % -> white"), "white");
	inputCloudsColorMode->addItem (tr("100 % -> dark"), "black");
	inputCloudsColorMode->setCurrentIndex (
			inputCloudsColorMode->findData (
					Util::getSetting("cloudsColorMode", "white").toString() ));
}











