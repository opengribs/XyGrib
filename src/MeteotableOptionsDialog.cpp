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
#include <cmath>
#include <cassert>

#include <QPainter>
#include <QColorDialog>
#include <QFontDialog>

#include "MeteotableOptionsDialog.h"
#include "DataQString.h"
#include "Util.h"

//===========================================================================
// DialogMeteotableOptions
//===========================================================================
DialogMeteotableOptions::DialogMeteotableOptions()
{
	setupUi(this);
	initInterface();
    
    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    
    connect(btAddVisibleItem,    SIGNAL(clicked()), this, SLOT(slotChangeVisibleItems()));
    connect(btRemoveVisibleItem, SIGNAL(clicked()), this, SLOT(slotChangeVisibleItems()));
}
//-------------------------------------------------------------------------
DialogMeteotableOptions::~DialogMeteotableOptions()
{
	for (int i = 0; i < listAllOptionItems.size(); ++i) {
		delete listAllOptionItems.at(i);
	}
}

//-------------------------------------------------------------------------------
void DialogMeteotableOptions::slotBtOK()
{
	// copy visible items order from widgets
	for (int i=0; i<listHiddenData->count(); i++) {
		QListWidgetItem *widgitem = listHiddenData->item(i);
		MeteotableOptionItem *item = getOptionItemByCode (widgitem->type());
		item->visible = false;
		item->pos = i;
	}
	for (int i=0; i<listVisibleData->count(); i++) {
		QListWidgetItem *widgitem = listVisibleData->item(i);
		MeteotableOptionItem *item = getOptionItemByCode (widgitem->type());
		item->visible = true;
		item->pos = i;
	}
	//----------------------------
	// remove old items from ini file
	QStringList listKeys = Settings::getAllKeys();
	QStringList::const_iterator cstit;
	for (cstit = listKeys.constBegin(); cstit != listKeys.constEnd(); cstit++) {
		QString key = *cstit;
		if (key.startsWith("MTableData_vis_")
			|| key.startsWith("MTableData_pos_")
		) {
			Settings::removeUserSetting (key);
		}
	}
	//----------------------------
	// write settings
	for (int i = 0; i < listAllOptionItems.size(); ++i) {
		MeteotableOptionItem *item = listAllOptionItems.at(i);
		uint grbcode = item->dtc.toInt32 ();
		Util::setSetting (getSettingName_vis(grbcode), item->visible);
		Util::setSetting (getSettingName_pos(grbcode), item->pos);
	}
	Util::setSetting("MTABLE_cloudsColorMode", inputCloudsColorMode->itemData(inputCloudsColorMode->currentIndex()).toString());
	Util::setSetting("MTABLE_showWindArrows", cbShowWindArrows->isChecked());
	Util::setSetting("MTABLE_showWindBeauforts", cbShowBeauforts->isChecked());
	Util::setSetting("MTABLE_showSunMoonAlmanac", cbSunMoonAlmanac->isChecked());
    
    accept();
}
//-------------------------------------------------------------------------------
void DialogMeteotableOptions::slotBtCancel()
{
    reject();
}
//-----------------------------------------------------------------------
void DialogMeteotableOptions::initInterface()
{
	currentItemCode = 1001;		// private type for QlistWidgetItem are > 1000
	createDataLists();
	
	inputCloudsColorMode->addItem (tr("100 % -> white"), "white");
	inputCloudsColorMode->addItem (tr("100 % -> dark"), "black");
	inputCloudsColorMode->setCurrentIndex (
			inputCloudsColorMode->findData (
					Util::getSetting("MTABLE_cloudsColorMode", "black").toString() ));

	cbShowWindArrows->setChecked (
				Util::getSetting("MTABLE_showWindArrows", true).toBool() );
	cbShowBeauforts->setChecked (
				Util::getSetting("MTABLE_showWindBeauforts", true).toBool() );
	cbSunMoonAlmanac->setChecked (
				Util::getSetting("MTABLE_showSunMoonAlmanac", true).toBool() );
}
//=============================================================================
void DialogMeteotableOptions::slotChangeVisibleItems()
{
	// copy visible items order from widget
	for (int i=0; i<listVisibleData->count(); i++)
	{
		QListWidgetItem *widgitem = listVisibleData->item(i);
		MeteotableOptionItem *item = getOptionItemByCode (widgitem->type());
		item->pos = i;
	}

	// change visibility of selected items
	QListWidget *listWidgetSource;
	if (sender() == btAddVisibleItem) {
		listWidgetSource = listHiddenData;
	}
	else {
		listWidgetSource = listVisibleData;
	}
	for (int i=0; i<listWidgetSource->count(); i++)
	{
		QListWidgetItem *widgitem = listWidgetSource->item(i);
		if (widgitem->isSelected()) 
		{
			// put all selected items at the end of the other list
			MeteotableOptionItem *item = getOptionItemByCode (widgitem->type());
			if (item) {
				item->visible = ! item->visible;    // reverse visibility
				item->pos = nbOptionItems(item->visible)-1;  // to the end of his category
			}
		}
	}
	// update positions after movings (numerate from 0)
	qSort (listAllOptionItems.begin(), listAllOptionItems.end(), 
								MeteotableOptionItem::lessThan_byPos);
	int posVis = 0;
	int posNotVis = 0;
	for (int i = 0; i < listAllOptionItems.size(); ++i) {
		MeteotableOptionItem *item = listAllOptionItems.at(i);
		if (item->visible) {
			item->pos = posVis++;
		}
		else {
			item->pos = posNotVis++;
		}
	}
	
	updateListWidgetsItems ();
}
//-----------------------------------------------------------------
int DialogMeteotableOptions::nbOptionItems (bool visibility) 
{
	int nb = 0;
	for (int i = 0; i < listAllOptionItems.size(); ++i)
		if (listAllOptionItems.at(i)->visible == visibility)
			nb ++;
	return nb;
}
//-----------------------------------------------------------------
MeteotableOptionItem * DialogMeteotableOptions::getOptionItemByCode (int interncode)
{
	MeteotableOptionItem *p = 0;
	for (int i = 0; !p && i<listAllOptionItems.size(); ++i)
		if (listAllOptionItems.at(i)->interncode == interncode)
			p = listAllOptionItems.at(i);
	return p;
}

//=============================================================================
// GUI
//=============================================================================
void DialogMeteotableOptions::addData
		( QString title, uchar grbtype, uchar leveltype, uint level,
		  bool defaultvis, int defaultpos )
{
	uint grbcode = DataCode (grbtype, leveltype, level).toInt32();
	bool visible = Util::getSetting (getSettingName_vis(grbcode), defaultvis).toBool();
	int  pos = Util::getSetting (getSettingName_pos(grbcode), defaultpos).toInt();
		
	MeteotableOptionItem *item =
		new MeteotableOptionItem (title, DataCode(grbtype,leveltype,level), 
							   pos, visible, currentItemCode);
						
	currentItemCode ++;
	listAllOptionItems.append (item);
}

//-------------------------------------------------------------------------
void DialogMeteotableOptions::updateListWidgetsItems ()
{
	// Create 2 temporary separate lists
	QList <MeteotableOptionItem *> listVisibleItems;
	QList <MeteotableOptionItem *> listHiddenItems;
	
	for (int i = 0; i < listAllOptionItems.size(); ++i) {
		MeteotableOptionItem *item = listAllOptionItems.at(i);
		if (item->visible)
			listVisibleItems.append (item);
		else
			listHiddenItems.append (item);
	}
	
	// sort visible data by position
	qSort (listVisibleItems.begin(), listVisibleItems.end(), 
								MeteotableOptionItem::lessThan_byPos);
	// sort hidden data by name
	qSort (listHiddenItems.begin(), listHiddenItems.end(), 
								MeteotableOptionItem::lessThan_byName);
	// add ordered items in widget lists
	listVisibleData->clear();
	listHiddenData->clear();
	
	for (int i = 0; i < listVisibleItems.size(); ++i) {
		MeteotableOptionItem *opt = listVisibleItems.at(i);
		QListWidgetItem *item = new QListWidgetItem (opt->name, 0, opt->interncode);
		listVisibleData->addItem (item);
	}
	for (int i = 0; i < listHiddenItems.size(); ++i) {
		MeteotableOptionItem *opt = listHiddenItems.at(i);
		QListWidgetItem *item = new QListWidgetItem (opt->name, 0, opt->interncode);
		listHiddenData->addItem (item);
	}
}

//-------------------------------------------------------------------------
void DialogMeteotableOptions::createDataLists()
{
	int  defaultpos = 0;
    //----------------------------------
	bool defaultvis = true;
    //----------------------------------
    addData (tr("Wind (10 m)"), GRB_PRV_WIND_XY2D,LV_ABOV_GND,10, defaultvis,defaultpos++);
    addData (tr("Wind gust"), GRB_WIND_GUST,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Current"), GRB_PRV_CUR_XY2D,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Cloud cover"),  GRB_CLOUD_TOT,LV_ATMOS_ALL,0, defaultvis,defaultpos++);
    addData (tr("Total precipitation"), GRB_PRECIP_TOT,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Temperature (2 m)"), 		GRB_TEMP,LV_ABOV_GND,2, defaultvis,defaultpos++);
    addData (tr("Gap temperature-dew point (2 m)"), GRB_PRV_DIFF_TEMPDEW,LV_ABOV_GND,2, defaultvis,defaultpos++);
    addData (tr("Isotherm 0°C"),     GRB_GEOPOT_HGT,LV_ISOTHERM0,0, defaultvis,defaultpos++);
    addData (tr("Mean sea level pressure"), GRB_PRESSURE_MSL,LV_MSL,0, defaultvis,defaultpos++);
    //----------------------------------
    defaultvis = false;
    //----------------------------------
    addData (tr("Dew point (2 m)"),    GRB_DEWPOINT,LV_ABOV_GND,2, defaultvis,defaultpos++);
    addData (tr("Relative humidity (2 m)"), GRB_HUMID_REL,LV_ABOV_GND,2, defaultvis,defaultpos++);
    //addData (tr("Potential temperature (sigma 995)"), GRB_TEMP_POT,LV_SIGMA,9950, defaultvis,defaultpos++);
    addData (tr("Temperature min (2 m)"),  GRB_TMIN,LV_ABOV_GND,2, defaultvis,defaultpos++);
    addData (tr("Temperature max (2 m)"),  GRB_TMAX,LV_ABOV_GND,2, defaultvis,defaultpos++);
    addData (tr("Snowfall possible"),         GRB_SNOW_CATEG,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Snow (depth)"),      GRB_SNOW_DEPTH,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Frozen rain possible"), GRB_FRZRAIN_CATEG,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("CAPE (surface)"),         GRB_CAPE,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("CIN (surface)"),         GRB_CIN,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("SkewT-LogP"), 	GRB_TEMP,LV_ATMOS_ALL,0, defaultvis,defaultpos++);
    //----------------------------------
    addData (tr("Wind")+" ("+tr("surface")+")", GRB_PRV_WIND_XY2D,LV_GND_SURF,0, defaultvis,defaultpos++);
    addData (tr("Wind (925 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,925, defaultvis,defaultpos++);
    addData (tr("Wind (850 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,850, defaultvis,defaultpos++);
    addData (tr("Wind (700 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,700, defaultvis,defaultpos++);
    addData (tr("Wind (600 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,600, defaultvis,defaultpos++);
    addData (tr("Wind (500 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,500, defaultvis,defaultpos++);
    addData (tr("Wind (400 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,400, defaultvis,defaultpos++);
    addData (tr("Wind (300 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,300, defaultvis,defaultpos++);
    addData (tr("Wind (200 hPa)"), GRB_PRV_WIND_XY2D,LV_ISOBARIC,200, defaultvis,defaultpos++);
    addData (tr("Temperature (925 hPa)"), GRB_TEMP,LV_ISOBARIC,925, defaultvis,defaultpos++);
    addData (tr("Temperature (850 hPa)"), GRB_TEMP,LV_ISOBARIC,850, defaultvis,defaultpos++);
    addData (tr("Temperature (700 hPa)"), GRB_TEMP,LV_ISOBARIC,700, defaultvis,defaultpos++);
    addData (tr("Temperature (600 hPa)"), GRB_TEMP,LV_ISOBARIC,600, defaultvis,defaultpos++);
    addData (tr("Temperature (500 hPa)"), GRB_TEMP,LV_ISOBARIC,500, defaultvis,defaultpos++);
    addData (tr("Temperature (400 hPa)"), GRB_TEMP,LV_ISOBARIC,400, defaultvis,defaultpos++);
    addData (tr("Temperature (300 hPa)"), GRB_TEMP,LV_ISOBARIC,300, defaultvis,defaultpos++);
    addData (tr("Temperature (200 hPa)"), GRB_TEMP,LV_ISOBARIC,200, defaultvis,defaultpos++);
    addData (tr("Theta-e (925 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,925, defaultvis,defaultpos++);
    addData (tr("Theta-e (850 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,850, defaultvis,defaultpos++);
    addData (tr("Theta-e (700 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,700, defaultvis,defaultpos++);
    addData (tr("Theta-e (600 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,600, defaultvis,defaultpos++);
    addData (tr("Theta-e (500 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,500, defaultvis,defaultpos++);
    addData (tr("Theta-e (400 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,400, defaultvis,defaultpos++);
    addData (tr("Theta-e (300 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,300, defaultvis,defaultpos++);
    addData (tr("Theta-e (200 hPa)"), GRB_PRV_THETA_E,LV_ISOBARIC,200, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (925 hPa)", GRB_HUMID_REL,LV_ISOBARIC,925, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (850 hPa)", GRB_HUMID_REL,LV_ISOBARIC,850, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (700 hPa)", GRB_HUMID_REL,LV_ISOBARIC,700, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (600 hPa)", GRB_HUMID_REL,LV_ISOBARIC,600, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (500 hPa)", GRB_HUMID_REL,LV_ISOBARIC,500, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (400 hPa)", GRB_HUMID_REL,LV_ISOBARIC,400, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (300 hPa)", GRB_HUMID_REL,LV_ISOBARIC,300, defaultvis,defaultpos++);
    addData (tr("Relative humidity")+" (200 hPa)", GRB_HUMID_REL,LV_ISOBARIC,200, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (925 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,925, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (850 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,850, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (700 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,700, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (600 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,600, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (500 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,500, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (400 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,400, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (300 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,300, defaultvis,defaultpos++);
    addData (tr("Geopotential altitude")+" (200 hPa)", GRB_GEOPOT_HGT,LV_ISOBARIC,200, defaultvis,defaultpos++);
    //----------------------------------
	int type;
	type = GRB_WAV_SIG_HT; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_PRV_WAV_MAX; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_PRV_WAV_WND; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_PRV_WAV_SWL; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_PRV_WAV_PRIM; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_PRV_WAV_SCDY; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
	type = GRB_WAV_WHITCAP_PROB; addData (tr("Waves")+": "+DataCodeStr::toString(type), type,LV_GND_SURF,0, defaultvis,defaultpos++);
    //----------------------------------

	updateListWidgetsItems ();
}











