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

#include <QMouseEvent>

#include "BoardPanel.h"
#include "Font.h"
#include "DataQString.h"

//=====================================================================
void BoardPanel::showPosition (double x, double y)
{
	if (cellPosition->isVisible()) {
		lbLon.setText (Util::formatLongitude(x));
		lbLat.setText (Util::formatLatitude (y));
	}
}
//-----------------------------------------------------
void BoardPanel::clearPosition ()
{
	if (cellPosition->isVisible()) {
		lbLon.setText ("");
		lbLat.setText ("");
	}
}
//-----------------------------------------------------
void BoardPanel::showDataPointInfo (
						const DataPointInfo &pf, const Altitude &windAlt)
{
    QString s;
	if (pf.isOk())
		showPosition (pf.x, pf.y);
	else
		clearPosition ();
	if (cellWind->isVisible()) {
		if (windAlt.levelType != LV_TYPE_NOT_DEFINED) {
			lbWindTitle.setText (tr("Wind")+"\n"+ AltitudeStr::toStringShort (windAlt));
		}
		else {
			lbWindTitle.setText (tr("Wind")+"\n");
		}
		lbWindDir.setText("");
		lbWindSpeed.setText("");
		lbWindBf.setText("");
		float v, dir;
		if (pf.getWindValues (windAlt, &v, &dir)) {
			lbWindDir.setText (s.sprintf("%.0f ", dir)+tr("°"));
			lbWindBf.setText  (s.sprintf("%2d ", Util::msToBeaufort(v))+tr("Bf"));
			lbWindSpeed.setText (Util::formatSpeed_Wind(v) );
		}
		else {
			lbWindDir.setText ("");
			lbWindBf.setText  ("");
			lbWindSpeed.setText ("");
		}
	}
	//------------------------------------------------------------
	if (cellCurrent->isVisible()) {
		float v, dir;
		if (pf.getCurrentValues (&v, &dir)) {
			lbCurrentDir.setText (s.sprintf("%.0f ", dir)+tr("°"));
			lbCurrentSpeed.setText (Util::formatSpeed_Current(v) );
		}
		else {
			lbCurrentDir.setText ("");
			lbCurrentSpeed.setText ("");
		}
	}
	//------------------------------------------------------------
	if (cellPressure->isVisible()) {
		if (pf.hasPressureMSL())
			lbPres.setText (Util::formatPressure (pf.pressureMSL));
		else
			lbPres.setText("");
	}
	if (cellGust->isVisible()) {
		if (pf.hasGUSTsfc())
			lbGUSTsfc.setText (Util::formatSpeed_Wind (pf.GUSTsfc));
		else
			lbGUSTsfc.setText("");
	}
	if (cellRain->isVisible()) {
		if (pf.hasRain())
			lbRain.setText (Util::formatRain (pf.rain));
		else
			lbRain.setText("");
	}
	if (cellTemp->isVisible()) {
		if (pf.tempAltitude.levelType != LV_TYPE_NOT_DEFINED) {
			lbTempTitle.setText (tr("Temperature")+" ("+AltitudeStr::toStringShort (pf.tempAltitude)+")");
			lastDefinedTempAltitude = pf.tempAltitude;
		}
		else {
			if (lastDefinedTempAltitude.levelType != LV_TYPE_NOT_DEFINED)
				lbTempTitle.setText (tr("Temperature")+" ("+AltitudeStr::toStringShort (lastDefinedTempAltitude)+")");
			else
				lbTempTitle.setText (tr("Temperature"));
		}
		if (pf.hasTemp())
			lbTemp.setText (Util::formatTemperature(pf.temp));
		else
			lbTemp.setText("");
	}
	if (cellTempMinMax->isVisible()) {
		if (pf.hasTempMin())
			lbTempMin.setText(Util::formatTemperature(pf.tempMin));
		else
			lbTempMin.setText("");
		if (pf.hasTempMax())
			lbTempMax.setText(Util::formatTemperature(pf.tempMax));
		else
			lbTempMax.setText("");
	}
	if (cellClouds->isVisible()) {
		if (pf.hasCloudTotal())
			lbCloud.setText(Util::formatPercentValue(pf.cloudTotal));
		else
			lbCloud.setText("");
	}
	if (cellHumidRel->isVisible()) {
		if (pf.hasHumidRel())
			lbHumid.setText(Util::formatPercentValue(pf.humidRel));
		else
			lbHumid.setText("");
	}
	if (cellDewPoint->isVisible()) {
		if (pf.hasDewPoint())
			lbDewPoint.setText(Util::formatTemperature(pf.dewPoint));
		else
			lbDewPoint.setText("");
	}
	if (cellIsotherm0->isVisible()) {
		if (pf.hasIsotherm0HGT())
			lbIsotherm0HGT.setText(Util::formatGeopotAltitude(pf.isotherm0HGT));
		else
			lbIsotherm0HGT.setText("");
	}
	if (cellSnowDepth->isVisible()) {
		if (pf.hasSnowDepth())
			lbSnowDepth.setText(Util::formatSnowDepth(pf.snowDepth));
		else
			lbSnowDepth.setText("");
	}
	if (cellCAPECIN->isVisible()) {
		if (pf.hasCAPEsfc())
			lbCAPEsfc.setText (Util::formatCAPEsfc (pf.CAPEsfc, false));
		else
			lbCAPEsfc.setText("");
		if (pf.hasCINsfc())
			lbCINsfc.setText (Util::formatCAPEsfc (pf.CINsfc, false));
		else
			lbCINsfc.setText("");
	}
	//------------------------------------------------
	// Data in altitude
	//------------------------------------------------
	if (cellAltitude->isVisible()) {
		if (pf.hasAltitudeData) {
			QString sv[GEOPOTsize];
			QString shpa, sgeopot, stemp, shumidrel, sthetae;
			for (int i=0; i<GEOPOTsize; i++)
			{
				int P = GEOPOThgt(i);	// 925 850 700 600 500 400 300 200
				shpa.sprintf("%3d", P);
				if (pf.hGeopot[i] != GRIB_NOTDEF) {
					sgeopot = QString("%1")
							.arg(Util::formatGeopotAltitude (pf.hGeopot[i],true), 9);
				}
				else
					sgeopot = QString("%1").arg(" ", 9);
				
				if (pf.hTemp[i] != GRIB_NOTDEF) {
					stemp = QString("%1")
							.arg(Util::formatTemperature (pf.hTemp[i],true), 7);
				}
				else
					stemp = QString("%1").arg(" ", 7);
				
				if (pf.hHumidRel[i] != GRIB_NOTDEF) {
					shumidrel = QString("%1%")
							.arg(Util::formatPercentValue (pf.hHumidRel[i],false), 3);
				}
				else
					shumidrel = QString("%1").arg(" ", 4);
				
				if (pf.hThetae[i] != GRIB_NOTDEF) {
					sthetae = QString("%1")
							.arg(Util::formatTemperature (pf.hThetae[i],true), 7);
				}
				else
					sthetae = QString("%1").arg(" ", 7);
				
				sv[i] = QString("%1 %2 %3 %4 %5")
									.arg(shpa).arg(sgeopot).arg(stemp).arg(shumidrel).arg(sthetae);

				if (i>0 && pf.hThetae[i]!=GRIB_NOTDEF && pf.hThetae[i-1]!=GRIB_NOTDEF) {
					QString delta;
					delta.sprintf(" (%+.1f)", pf.hThetae[i]-pf.hThetae[i-1]);
					sv[i] += delta;
				}
			}
			lbAltTitle.setText("hPa   altitude   temp  rh    θe");
			lb925.setText (sv[0]);
			lb850.setText (sv[1]);
			lb700.setText (sv[2]);
			lb600.setText (sv[3]);
			lb500.setText (sv[4]);
			lb400.setText (sv[5]);
			lb300.setText (sv[6]);
			lb200.setText (sv[7]);
		}
		else {
			lbAltTitle.setText("");
			lb925.setText("");
			lb850.setText("");
			lb700.setText("");
			lb600.setText("");
			lb500.setText("");
			lb400.setText("");
			lb300.setText("");
			lb200.setText("");
		}
	}
	//------------------------------------------------
	// Waves
	//------------------------------------------------
	if (cellWaves->isVisible()) {
		lbWaves_sig.setText (formatWave(tr("sig  :","Tr: VERY SHORT TAG, same size as 'swell' translation: significant"),pf.wave_sig_ht, GRIB_NOTDEF, GRIB_NOTDEF));
		lbWaves_max.setText (formatWave(tr("max  :","Tr: VERY SHORT TAG, same size as 'swell' translation: maximum"),pf.wave_max_ht, pf.wave_max_dir, pf.wave_max_per));
		lbWaves_swl.setText (formatWave(tr("swell:","Tr: VERY SHORT TAG: swell"),pf.wave_swl_ht, pf.wave_swl_dir, pf.wave_swl_per));
		lbWaves_wnd.setText (formatWave(tr("wind :","Tr: VERY SHORT TAG, same size as 'swell' translation: wind"),pf.wave_wnd_ht, pf.wave_wnd_dir, pf.wave_wnd_per));
		
		lbWaves_pri.setText (formatWave(tr("prim :","Tr: VERY SHORT TAG, same size as 'swell' translation: primary"),GRIB_NOTDEF, pf.wave_pr_dir, pf.wave_pr_per));
		lbWaves_sec.setText (formatWave(tr("scdy :","Tr: VERY SHORT TAG, same size as 'swell' translation: secondary"),GRIB_NOTDEF, pf.wave_scdy_dir, pf.wave_scdy_per));
		
		QString s;
		if (pf.wave_wcap_prbl != GRIB_NOTDEF) {
			s = tr("whitecap (prob)","Tr: SHORT TAG")+QString(": %1").arg(Util::formatWhiteCap (pf.wave_wcap_prbl,true));
		}
		else
			s = tr("whitecap (prob)","Tr: SHORT TAG")+": ";
		lbWaves_wcap.setText (s);
	}

}
//===================================================================================
QString BoardPanel::formatWave (QString title, float ht, float dir, float per)
{
	QString r = title;
	
	QString s;
	if (ht != GRIB_NOTDEF)
		s.sprintf(" %5.2fm", ht);
	else
		s.sprintf("       ");
	r += s;
	if (dir != GRIB_NOTDEF)
		s.sprintf(" %+4.0f°", dir);
	else
		s.sprintf("      ");
	r += s;
	if (per != GRIB_NOTDEF)
		s.sprintf(" %3.0fs", per);
	else
		s.sprintf("     ");
	r += s;

	return r;
}


//===================================================================================
//===================================================================================
BoardPanel::BoardPanel (QWidget *parent)
    : QDockWidget (parent)
{
	this->setObjectName ("BoardPanel");
	this->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->setWindowTitle (tr("Weather data"));
	lastDefinedTempAltitude = Altitude (LV_TYPE_NOT_DEFINED,0);
	
	updateLabelsSizes();
    	// "0" to have correct widget height on construct
        lbPres.setText("0");
        lbWindDir.setText("0");
        lbWindSpeed.setText("0");
        lbWindBf.setText("0");
        lbRain.setText("0");
        lbTemp.setText("0");
        lbTempMin.setText("0");
        lbTempMax.setText("0");
        lbCloud.setText("0");
        lbHumid.setText("0");
        lbDewPoint.setText("0");
        lbIsotherm0HGT.setText("0");
        lbSnowDepth.setText("0");
        lbCAPEsfc.setText("0");
        lbCINsfc.setText("0");
        lbGUSTsfc.setText("0");
        lbCurrentDir.setText("0");
        lbCurrentSpeed.setText("0");
        lbAltTitle.setText("0");
        lb925.setText("0");
        lb850.setText("0");
        lb700.setText("0");
        lb600.setText("0");
        lb500.setText("0");
        lb400.setText("0");
        lb300.setText("0");
        lb200.setText("0");

	createInterface ();
	createMenuPopup ();
	mainFrame->adjustSize();

    	// Clear "0"
        lbPres.setText("");
        lbWindDir.setText("");
        lbWindSpeed.setText("");
        lbWindBf.setText("");
        lbRain.setText("");
        lbTemp.setText("");
        lbTempMin.setText("");
        lbTempMax.setText("");
        lbCloud.setText("");
        lbHumid.setText("");
        lbDewPoint.setText("");
        lbIsotherm0HGT.setText("");
        lbSnowDepth.setText("");
        lbCAPEsfc.setText("");
        lbCINsfc.setText("");
        lbGUSTsfc.setText("");
        lbCurrentDir.setText("");
        lbCurrentSpeed.setText("");
        lbAltTitle.setText("");
        lb925.setText("");
        lb850.setText("");
        lb700.setText("");
        lb600.setText("");
        lb500.setText("");
        lb400.setText("");
        lb300.setText("");
        lb200.setText("");
	
}
//---------------------------------------------------------------
void BoardPanel::updateLabelsSizes()
{
    QFontMetrics fmt(qApp->font());
    int style = QFrame::Panel | QFrame::Sunken;

    lbLon.setMinimumWidth( fmt.width("X888X88X00XX-") );
    lbLon.setFrameStyle(style);
    lbLat.setMinimumWidth( fmt.width("X888X88X00XX-") );
    lbLat.setFrameStyle(style);

    lbWindDir.setMinimumWidth( fmt.width("888XX-.") );
    lbWindDir.setFrameStyle(style);
    lbWindBf.setMinimumWidth( fmt.width("8888Bf-") );
    lbWindBf.setFrameStyle(style);
	
    lbWindSpeed.setMinimumWidth( fmt.width("888XXXXXkm/h-") );
    lbWindSpeed.setFrameStyle(style);
	
    lbGUSTsfc.setFrameStyle(style);
    lbGUSTsfc.setMinimumWidth( fmt.width("888XXXXXkm/h-") );
	
    lbCurrentDir.setMinimumWidth( fmt.width("888XX-.") );
    lbCurrentDir.setFrameStyle(style);
    lbCurrentSpeed.setMinimumWidth( fmt.width("888XXXXXX-.") );
    lbCurrentSpeed.setFrameStyle(style);

    QString strlong = "888888888888";
    lbPres.setMinimumWidth( fmt.width(strlong) );
    lbPres.setFrameStyle(style);
    lbRain.setMinimumWidth( fmt.width(strlong) );
    lbRain.setFrameStyle(style);
    lbTemp.setMinimumWidth( fmt.width(strlong) );
    lbTemp.setFrameStyle(style);
    lbTempMin.setMinimumWidth( fmt.width(strlong) );
    lbTempMin.setFrameStyle(style);
    lbTempMax.setMinimumWidth( fmt.width(strlong) );
    lbTempMax.setFrameStyle(style);
    lbCloud.setMinimumWidth( fmt.width(strlong) );
    lbCloud.setFrameStyle(style);
    lbHumid.setMinimumWidth( fmt.width(strlong) );
    lbHumid.setFrameStyle(style);
    lbDewPoint.setMinimumWidth( fmt.width(strlong) );
    lbDewPoint.setFrameStyle(style);
    lbIsotherm0HGT.setMinimumWidth( fmt.width(strlong) );
    lbIsotherm0HGT.setFrameStyle(style);
    lbSnowDepth.setFrameStyle(style);
    lbSnowDepth.setMinimumWidth( fmt.width(strlong) );
    lbSnowCateg.setFrameStyle(style);
    lbSnowCateg.setMinimumWidth( fmt.width(strlong) );

    strlong = "888888";
    lbCAPEsfc.setFrameStyle(style);
    lbCAPEsfc.setMinimumWidth( fmt.width(strlong) );
    lbCINsfc.setFrameStyle(style);
    lbCINsfc.setMinimumWidth( fmt.width(strlong) );
	
	// Altitude : hPa Geopot Temp Theta-e
//    strlong = " 850 hPa ";
//	lbWindTitle.setMinimumWidth( fmt.width(strlong) );
	
	QFont fontAlt = Font::getFont(FONT_AltitudeLabel);
	QFont fontAltTitle = Font::getFont(FONT_AltitudeLabelTitle);

	// Font size: 80 % of general font size
	int sz = (int)(0.8*(qApp->font().pointSize()+0.5));
	fontAlt.setPointSize (sz);
	fontAltTitle.setPointSize (sz);
	//DBG("%d %d", qApp->font().pointSize(),sz);	
	
	lbAltTitle.setFont (fontAltTitle);
	lb925.setFont (fontAlt);
	lb850.setFont (fontAlt);
	lb700.setFont (fontAlt);
	lb600.setFont (fontAlt);
	lb500.setFont (fontAlt);
	lb400.setFont (fontAlt);
	lb300.setFont (fontAlt);
	lb200.setFont (fontAlt);
	
    QFontMetrics fmtAlt (fontAlt);
    QFontMetrics fmtAltTitle (fontAltTitle);
    
	strlong = "hPa  altitude   temp     θe";
	lbAltTitle.setMinimumWidth (fmtAltTitle.width(strlong));
	
    strlong = "999 99999 gpft -99.9°C 100% 99.9°C (-99.9)";
	int wmin = fmtAlt.width(strlong);
	lb925.setMinimumWidth (wmin);
	lb850.setMinimumWidth (wmin);
	lb700.setMinimumWidth (wmin);
	lb600.setMinimumWidth (wmin);
	lb500.setMinimumWidth (wmin);
	lb400.setMinimumWidth (wmin);
	lb300.setMinimumWidth (wmin);
	lb200.setMinimumWidth (wmin);
	//---------------------------------------
	lbWaves_sig.setFont (fontAlt);
	lbWaves_max.setFont (fontAlt);
	lbWaves_swl.setFont (fontAlt);
	lbWaves_wnd.setFont (fontAlt);
	lbWaves_pri.setFont (fontAlt);
	lbWaves_sec.setFont (fontAlt);
// 	lbWaves_pek.setFont (fontAlt);
	lbWaves_wcap.setFont (fontAlt);
 }

//---------------------------------------------------------------
void BoardPanel::addWidget (QWidget *widget)
{
	mainFrameLay->addWidget (widget);
}
//=================================================================
BoardPanelCell::BoardPanelCell (QWidget *parent, QLayout *lay)
	: QFrame (parent)
{
	setFrameStyle (QFrame::Panel | QFrame::Sunken);
	setVisible (true);
    lay->addWidget (this);
}

//=================================================================
QHBoxLayout * BoardPanel::newQHBoxLayout () {
	int hsp = 1;	// horizontal spacing
	int vsp = 1;	// vertical spacing
    QHBoxLayout *lay = new QHBoxLayout ();
    assert (lay);
	lay->setContentsMargins (hsp,vsp,hsp,vsp);
	lay->setSpacing (4);
	return lay;
}
//---------------------------------------------------------------
QVBoxLayout * BoardPanel::newQVBoxLayout () {
	int hsp = 1;	// horizontal spacing
	int vsp = 0;	// vertical spacing
    QVBoxLayout *lay = new QVBoxLayout ();
    assert (lay);
	lay->setContentsMargins (hsp,vsp,hsp,vsp);
	lay->setSpacing (0);
	return lay;
}


//--------------------------------------------------------------
void BoardPanel::mouseReleaseEvent (QMouseEvent *e)
{
    switch (e->button()) {

        case Qt::RightButton :
            // Affiche un menu popup
			menuPopup->exec(QCursor::pos());
            break;
        case Qt::LeftButton :
        case Qt::MidButton :
            break;
        default :
            break;
    }
}
//=================================================================
void BoardPanel::createMenuPopup ()
{
	menuPopup = new QMenu (this);
	assert (menuPopup);
	addAction (tr("Location"), "boardPanelPosition", cellPosition);
	addAction (tr("Wind"),     "boardPanelWind", cellWind);
	addAction (tr("Wind gust"),     "boardPanelGust", cellGust);
	addAction (tr("Current"),     "boardPanelCurrent", cellCurrent);
	addAction (tr("Pressure"), "boardPanelPressure", cellPressure);
	addAction (tr("Temperature"),"boardPanelTemp", cellTemp);
	addAction (tr("Dew point"), "boardPanelDewPoint", cellDewPoint);
	addAction (tr("Temperature")+" "+tr("Min")+" "+tr("Max"),"boardTempMinMax", cellTempMinMax);
	addAction (tr("Precipitation"), "boardPanelRain", cellRain);
	addAction (tr("Cloud cover"),  "boardPanelClouds", cellClouds);
	addAction (tr("Relative humidity"), "boardPanelHumidity", cellHumidRel);
	addAction (tr("Isotherm 0°C"), "boardPanelIsotherm0", cellIsotherm0);
	addAction (tr("Snow (depth)"), "boardPanelSnowDepth", cellSnowDepth);
	addAction (tr("CAPE CIN"), "boardPanelCAPE", cellCAPECIN);
	addAction (tr("Altitude"), "boardPanelAltitudeData", cellAltitude);
	addAction (tr("Waves"), "boardPanelWaves", cellWaves);
}
//-------------------------------------------------------
void BoardPanel::addAction (QString title, QString settingName, 
									 BoardPanelCell *cell)
{
	QAction *ac = new QAction (title, menuPopup);
	assert (ac);
	ac->setCheckable  (true);
	menuPopup->addAction (ac);
	connect (ac, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
	hashActionSettingName.insert    (ac, settingName);
	hashActionBoardPanelCell.insert (ac, cell);
	bool vis = Util::getSetting (settingName, true).toBool();
	cell->setVisible (vis);
	ac->setChecked (vis);
}
//-------------------------------------------------------
void BoardPanel::actionTriggered (bool b)
{
	QAction *ac = (QAction *) sender();
	QString settingName = hashActionSettingName.value (ac);
	BoardPanelCell *cell = hashActionBoardPanelCell.value (ac);
	Util::setSetting (settingName, b);
	cell->setVisible (b);
 //DBGQS(settingName);
 mainFrame->adjustSize();
}
//=================================================================
void BoardPanel::createInterface ()
{
    QHBoxLayout *hlay;
	//--------------------------
    mainFrame = new QWidget (this);
    assert (mainFrame);
    mainFrameLay = new QVBoxLayout();
    assert (mainFrameLay);
	mainFrameLay->setSpacing (0);
	mainFrameLay->setContentsMargins (0,0,0,0);
	mainFrame->setLayout (mainFrameLay);
	
	// put the main frame in a scrollarea
//	mainFrameLay->setSizeConstraint (QLayout::SetMinimumSize);
	scrollarea = new QScrollArea ();
	scrollarea->setFrameStyle(QFrame::NoFrame);
	this->setWidget (scrollarea);
	scrollarea->setWidget (mainFrame);
	
	cellPosition = new BoardPanelCell (mainFrame, mainFrameLay);
	cellWind = new BoardPanelCell (mainFrame, mainFrameLay);
	cellGust = new BoardPanelCell (mainFrame, mainFrameLay);
	cellCurrent = new BoardPanelCell (mainFrame, mainFrameLay);
	cellPressure = new BoardPanelCell (mainFrame, mainFrameLay);
	cellTemp = new BoardPanelCell (mainFrame, mainFrameLay);
	cellDewPoint = new BoardPanelCell (mainFrame, mainFrameLay);
	cellTempMinMax = new BoardPanelCell (mainFrame, mainFrameLay);
	cellRain = new BoardPanelCell (mainFrame, mainFrameLay);
	cellClouds = new BoardPanelCell (mainFrame, mainFrameLay);
	cellHumidRel = new BoardPanelCell (mainFrame, mainFrameLay);
	cellIsotherm0 = new BoardPanelCell (mainFrame, mainFrameLay);
	cellSnowDepth = new BoardPanelCell (mainFrame, mainFrameLay);
	cellCAPECIN = new BoardPanelCell (mainFrame, mainFrameLay);
	cellAltitude = new BoardPanelCell (mainFrame, mainFrameLay);
	cellWaves = new BoardPanelCell (mainFrame, mainFrameLay);
		
    hlay = newQHBoxLayout ();
		hlay->addStretch (100);
        hlay->addWidget (&lbLat, 0, Qt::AlignRight);
        hlay->addWidget (&lbLon, 0, Qt::AlignRight);
        cellPosition->setLayout (hlay);
		
    hlay = newQHBoxLayout ();
		hlay->addStretch (100);
		lbWindTitle.setText (tr("Wind")+"\n"+"10 m");
        hlay->addWidget(&lbWindTitle, 0, Qt::AlignRight);
        hlay->addWidget(&lbWindDir, 0, Qt::AlignRight);
        hlay->addWidget(&lbWindBf, 0, Qt::AlignRight);
        hlay->addWidget(&lbWindSpeed, 0, Qt::AlignRight);
        cellWind->setLayout(hlay);

	hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Wind gust")), 10, Qt::AlignRight);
        hlay->addWidget(&lbGUSTsfc, 0, Qt::AlignRight);
        cellGust->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Current")), 10, Qt::AlignRight);
        hlay->addWidget(&lbCurrentDir, 0, Qt::AlignRight);
        hlay->addWidget(&lbCurrentSpeed, 0, Qt::AlignRight);
        cellCurrent->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Pressure")+" (MSL)"), 10, Qt::AlignRight);
        hlay->addWidget(&lbPres, 0, Qt::AlignRight);
        cellPressure->setLayout(hlay);
		
    hlay = newQHBoxLayout ();
		lbTempTitle.setText (tr("Temperature"));
        hlay->addWidget(&lbTempTitle, 10, Qt::AlignRight);
        hlay->addWidget(&lbTemp, 0, Qt::AlignRight);
        cellTemp->setLayout(hlay);

	hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Dew point")+" (2 m)"), 10, Qt::AlignRight);
        hlay->addWidget(&lbDewPoint, 0, Qt::AlignRight);
        cellDewPoint->setLayout(hlay);
		
    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Min")), 0, Qt::AlignRight);
        hlay->addWidget(&lbTempMin, 0, Qt::AlignRight);
        hlay->addWidget(new QLabel(tr("Max")), 0, Qt::AlignRight);
        hlay->addWidget(&lbTempMax, 0, Qt::AlignRight);
        cellTempMinMax->setLayout(hlay);
		
    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Precipitation")), 10, Qt::AlignRight);
        hlay->addWidget(&lbRain, 0, Qt::AlignRight);
        cellRain->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Cloud cover")), 10, Qt::AlignRight);
        hlay->addWidget(&lbCloud, 0, Qt::AlignRight);
        cellClouds->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Relative humidity")+" (2 m)"), 10, Qt::AlignRight);
        hlay->addWidget(&lbHumid, 0, Qt::AlignRight);
        cellHumidRel->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Isotherm 0°C")), 10, Qt::AlignRight);
        hlay->addWidget(&lbIsotherm0HGT, 0, Qt::AlignRight);
        cellIsotherm0->setLayout(hlay);

    hlay = newQHBoxLayout ();
        hlay->addWidget(new QLabel(tr("Snow depth")), 10, Qt::AlignRight);
        hlay->addWidget(&lbSnowDepth, 0, Qt::AlignRight);
        cellSnowDepth->setLayout(hlay);

    hlay = newQHBoxLayout ();
		hlay->addStretch (100);
        hlay->addWidget(new QLabel(tr("CAPE")), 10, Qt::AlignRight);
        hlay->addWidget(&lbCAPEsfc, 0, Qt::AlignRight);
        hlay->addWidget(new QLabel(tr("CIN")), 10, Qt::AlignRight);
        hlay->addWidget(&lbCINsfc, 0, Qt::AlignRight);
        hlay->addWidget(new QLabel(tr("J/Kg")), 10, Qt::AlignRight);
        cellCAPECIN->setLayout(hlay);

    //--------------------------
    // Altitude data
    //--------------------------
	QVBoxLayout *vlay = newQVBoxLayout ();
		vlay->addWidget (&lbAltTitle);
		vlay->addWidget (&lb200);
		vlay->addWidget (&lb300);
		vlay->addWidget (&lb400);
		vlay->addWidget (&lb500);
		vlay->addWidget (&lb600);
		vlay->addWidget (&lb700);
		vlay->addWidget (&lb850);
		vlay->addWidget (&lb925);
        cellAltitude->setLayout (vlay);

    //--------------------------
    // Waves data
    //--------------------------
	vlay = newQVBoxLayout ();
		vlay->addWidget (&lbWaves_sig);
		vlay->addWidget (&lbWaves_max);
		vlay->addWidget (&lbWaves_swl);
		vlay->addWidget (&lbWaves_wnd);
		
		vlay->addWidget (&lbWaves_pri);
		vlay->addWidget (&lbWaves_sec);
// 		vlay->addWidget (&lbWaves_pek);
		
		vlay->addWidget (&lbWaves_wcap);

		cellWaves->setLayout (vlay);
	
	//-------------------------------------
	// End with a stretch spacer
	mainFrameLay->addStretch (100);
}





