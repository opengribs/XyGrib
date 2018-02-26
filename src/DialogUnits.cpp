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
#include <time.h>

#include "DialogUnits.h"
#include "Util.h"


//-------------------------------------------------------------------------------
DialogUnits::DialogUnits (QWidget *parent) : DialogBoxBase (parent)
{
    setWindowTitle(tr("Units"));
    QFrame *ftmp;
    QLabel *label;
    frameGui = createFrameGui(this);
    
    layout = new QGridLayout(this);
    int lig=0;
    //-------------------------
    lig ++;
    QFont font;
    font.setBold(true);
    label = new QLabel(tr("Units"), this);
    label->setFont(font);
    layout->addWidget( label,    lig,0, 1,-1, Qt::AlignCenter);
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    layout->addWidget( frameGui,  lig,0,   1, 2);
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    btOK     = new QPushButton(tr("Ok"), this);
    btCancel = new QPushButton(tr("Cancel"), this);
    layout->addWidget( btOK,    lig,0);
    layout->addWidget( btCancel, lig,1);
    
    //===============================================================
    // restaure les valeurs enregistrées dans les settings
    QString tunit;
    int ind;

    tunit = Util::getSetting("unitsWindSpeed", "").toString();
    ind = (tunit=="") ? 0 : cbWindSpeedUnit->findData(tunit);
    cbWindSpeedUnit->setCurrentIndex( ind );

    tunit = Util::getSetting("unitsCurrentSpeed", "").toString();
    ind = (tunit=="") ? 0 : cbCurrentSpeedUnit->findData(tunit);
    cbCurrentSpeedUnit->setCurrentIndex( ind );

    tunit = Util::getSetting("unitsTemp",   "").toString();
    ind = (tunit=="") ? 0 : cbTempUnit->findData(tunit);
    cbTempUnit->setCurrentIndex( ind );

    tunit = Util::getSetting("unitsPosition", "").toString();
    ind = (tunit=="") ? 0 : cbPositionUnit->findData(tunit);
    cbPositionUnit->setCurrentIndex( ind );
    
    tunit = Util::getSetting("unitsDistance", "").toString();
    ind = (tunit=="") ? 0 : cbDistanceUnit->findData(tunit);
    cbDistanceUnit->setCurrentIndex( ind );
    
    tunit = Util::getSetting("longitudeDirection", "").toString();
    ind = (tunit=="") ? 0 : cbLongitude->findData(tunit);
    cbLongitude->setCurrentIndex( ind );
    
    tunit = Util::getSetting("latitudeDirection", "").toString();
    ind = (tunit=="") ? 0 : cbLatitude->findData(tunit);
    cbLatitude->setCurrentIndex( ind );
    
    tunit = Util::getSetting("timeZone", "UTC").toString();
    ind = (tunit=="") ? 0 : cbTimeZone->findData(tunit);
    cbTimeZone->setCurrentIndex( ind );
	isTimeZoneChanged = false;    
    
    tunit = Util::getSetting("geopotAltitudeUnit", "").toString();
    ind = (tunit=="") ? 0 : cbAltitude->findData(tunit);
    cbAltitude->setCurrentIndex( ind );
    
    tunit = Util::getSetting("isotherm0Unit", "").toString();
    ind = (tunit=="") ? 0 : cbIsotherme0->findData(tunit);
    cbIsotherme0->setCurrentIndex( ind );

    //===============================================================
    connect(btCancel, SIGNAL(clicked()), this, SLOT(slotBtCancel()));
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
}

//-------------------------------------------------------------------------------
void DialogUnits::slotBtOK()
{
	QComboBox *cb;
	
	cb = cbWindSpeedUnit;
    Util::setSetting("unitsWindSpeed", cb->itemData(cb->currentIndex()).toString());
    cb = cbCurrentSpeedUnit;
    Util::setSetting("unitsCurrentSpeed", cb->itemData(cb->currentIndex()).toString());
    cb = cbTempUnit;
    Util::setSetting("unitsTemp",      cb->itemData(cb->currentIndex()).toString());
    cb = cbPositionUnit;
    Util::setSetting("unitsPosition",  cb->itemData(cb->currentIndex()).toString());
    cb = cbDistanceUnit;
    Util::setSetting("unitsDistance",  cb->itemData(cb->currentIndex()).toString());

    cb = cbAltitude;
    Util::setSetting("geopotAltitudeUnit",  cb->itemData(cb->currentIndex()).toString());
    cb = cbIsotherme0;
    Util::setSetting("isotherm0Unit",  cb->itemData(cb->currentIndex()).toString());

 	cb = cbLongitude;
    Util::setSetting("longitudeDirection", cb->itemData(cb->currentIndex()).toString());
    cb = cbLatitude;
    Util::setSetting("latitudeDirection",  cb->itemData(cb->currentIndex()).toString());

 	cb = cbTimeZone;
    Util::setSetting("timeZone",  cb->itemData(cb->currentIndex()).toString());
	if	(isTimeZoneChanged)
	{
		isTimeZoneChanged = false;
		emit signalTimeZoneChanged();
    }
    accept();
}
//-------------------------------------------------------------------------------
void DialogUnits::slotBtCancel()
{
	isTimeZoneChanged = false;
    reject();
}

//=============================================================================
// GUI
//=============================================================================
QFrame *DialogUnits::createFrameGui(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
    QLabel * label;
    QGridLayout  *lay = new QGridLayout(frm);
    int lig=0;
    int sizemin = 160;
    //-------------------------
    lig ++;
    label = new QLabel(tr("Wind speed :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbWindSpeedUnit = new QComboBox(this);
    cbWindSpeedUnit->addItem( tr("m/s") ,  "m/s");
    cbWindSpeedUnit->addItem( tr("km/h"),  "km/h");
    cbWindSpeedUnit->addItem( tr("kts"), "kts");
    cbWindSpeedUnit->setMinimumWidth (sizemin);
    lay->addWidget( cbWindSpeedUnit, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Current speed :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbCurrentSpeedUnit = new QComboBox(this);
    cbCurrentSpeedUnit->addItem( tr("m/s") ,  "m/s");
    cbCurrentSpeedUnit->addItem( tr("km/h"),  "km/h");
    cbCurrentSpeedUnit->addItem( tr("kts"), "kts");
    cbCurrentSpeedUnit->setMinimumWidth (sizemin);
    lay->addWidget( cbCurrentSpeedUnit, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Temperature :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbTempUnit = new QComboBox(this);
    cbTempUnit->addItem( tr("°C"),  "°C");
    cbTempUnit->addItem( tr("°F"),  "°F");
    cbTempUnit->addItem( tr("°K"),  "°K");
    cbTempUnit->setMinimumWidth (sizemin);
    lay->addWidget( cbTempUnit, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Distances :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbDistanceUnit = new QComboBox(this);
    cbDistanceUnit->addItem( tr("Nautical Mile"), "mille marin");
    cbDistanceUnit->addItem( tr("km"), "km");
    cbDistanceUnit->setMinimumWidth (sizemin);
    lay->addWidget( cbDistanceUnit, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Coordinates :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbPositionUnit = new QComboBox(this);
    cbPositionUnit->addItem( tr("dd°mm'ss\""), "dd°mm'ss\"");
    cbPositionUnit->addItem( tr("dd°mm,mm'"), "dd°mm,mm'");
    cbPositionUnit->addItem( tr("dd,dd°"), "dd,dd°");
    cbPositionUnit->setMinimumWidth (sizemin);
    lay->addWidget( cbPositionUnit, lig,1, Qt::AlignLeft);
    
    //-------------------------
    lig ++;
    label = new QLabel(tr("Longitudes :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbLongitude = new QComboBox(this);
    cbLongitude->addItem( tr("Auto"), "Auto");
    cbLongitude->addItem( tr("East positive"),   "East+");
    cbLongitude->addItem( tr("West positive"), "West+");
    cbLongitude->setMinimumWidth (sizemin);
    lay->addWidget( cbLongitude, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Latitudes :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbLatitude = new QComboBox(this);
    cbLatitude->addItem( tr("Auto"), "Auto");
    cbLatitude->addItem( tr("North positive"), "North+");
    cbLatitude->addItem( tr("South positive"),  "South+");
    cbLatitude->setMinimumWidth (sizemin);
    lay->addWidget( cbLatitude, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Time zone :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbTimeZone = new QComboBox(this);
    cbTimeZone->addItem( tr("UTC time"),    "UTC");
    cbTimeZone->addItem( tr("Local time"), "LOC");
	for (int i=-12; i<=14; i++)
	{	// UTC-12 UTC-11 ... UTC+1 UTC+2 UTC+3 ... UTC+14
		QString stz;
		if (i != 0)
			stz.sprintf("UTC%+d", i);
		else
			stz = "UTC";
    	cbTimeZone->addItem( tr("Fixed time ")+stz, stz);
	}
    cbTimeZone->setMinimumWidth (sizemin);
    lay->addWidget( cbTimeZone, lig,1, Qt::AlignLeft);
    connect(cbTimeZone, SIGNAL(activated(int)), this, SLOT(slotTimeZoneChanged(int)));
    //-------------------------
    lig ++;
    label = new QLabel(tr("Geopotential altitudes:"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbAltitude = new QComboBox(this);
    cbAltitude->addItem( tr("gm (≈ meter)"), "gpm");
    cbAltitude->addItem( tr("gdm (≈ decameter)"), "gpdm");
    cbAltitude->addItem( tr("gft (≈ foot)"), "gpft");
    cbAltitude->setMinimumWidth (sizemin);
    lay->addWidget( cbAltitude, lig,1, Qt::AlignLeft);
    //-------------------------
    lig ++;
    label = new QLabel(tr("Isotherm 0°C :"), frm);
    lay->addWidget( label,    lig,0, Qt::AlignRight);
    cbIsotherme0 = new QComboBox(this);
    cbIsotherme0->addItem( tr("meter (m)"), "m");
    cbIsotherme0->addItem( tr("decameter (dam)"), "dam");
    cbIsotherme0->addItem( tr("foot (ft)"), "ft");
    cbIsotherme0->setMinimumWidth (sizemin);
    lay->addWidget( cbIsotherme0, lig,1, Qt::AlignLeft);
    
    return frm;
}

//---------------------------------------------------
void DialogUnits::slotTimeZoneChanged(int index)
{
	isTimeZoneChanged = true;
	QString val = cbTimeZone->itemData(index).toString();
	if (val == "LOC")
	{
		time_t locnow = QDateTime::currentDateTime().toTime_t();
    	
    	QString saveTimeZone = Util::getSetting("timeZone", "UTC").toString();
		Util::setSetting("timeZone",  "UTC");
		QString tutc = Util::formatDateTimeLong (locnow);
		Util::setSetting("timeZone",  "LOC");
		QString tloc = Util::formatDateTimeLong (locnow);
    	Util::setSetting("timeZone",  saveTimeZone);

        QMessageBox::warning (this,
		tr("Local time"),
		tr("WARNING: You have choose local time.")+"\n"
		  + "\n"
		  + tr("The local time is the time of your computer, it's not")+"\n"
		  + tr("the time of a point on the map.")+"\n"
		  + "\n"
		  + tr("Conversion UTC/Local time depends of the parameters ") +"\n"
		  + tr("of your computer. ")
		  +"\n"
		  + tr("Current hour seen by zygrib with those parameters :")+"\n"
		  + "      " + tutc + tr(" : UTC")+"\n"
		  + "      " + tloc + tr(" : Local")+"\n"
		  + "\n"
		  + tr("If these values are bad, you must update the ")+"\n"
		  + tr("configuration of your computeur (time zone),")+"\n"
		  + tr("or choose display in UTC time zone.")+"\n"
		  + "\n"
		);
	}
//exit(1);
}











