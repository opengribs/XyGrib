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

#include <QTimer>

#include "Astro.h"
#include "Settings.h"
#include "DataQString.h"
#include "MeteoTableWidget.h"

//-------------------------------------------------------------------------------
MeteoTableWidget::MeteoTableWidget 
			(GriddedPlotter *plotter, 
			 double lon, double lat, QString locationName, QWidget *parent)
	: QWidget(parent)
{
	this->plotter = plotter;
	this->parent = parent;
	this->lon = lon;
	this->lat = lat;
	this->locationName = locationName;

	layout = new QGridLayout (this);
	assert (layout);
	layout->setContentsMargins (0,0,0,0);
	layout->setSpacing (0);
	headerWidget = new QWidget ();
	assert (headerWidget);
	headerLayout = new QGridLayout (headerWidget);
	assert (headerLayout);
	headerLayout->setContentsMargins (0,0,0,0);
	headerLayout->setSpacing (0);
	
	createTable();
}
//----------------------------------------------------------------
void MeteoTableWidget::addCell_title_dataline (QString txt, bool bold,
				int lig,int col)
{
	QColor bgcolor(200,200,255);
	TableCell *cell2 = new TableCell (headerWidget, txt, bold, bgcolor,
										Qt::AlignRight|Qt::AlignHCenter);	
	assert (cell2);
	cell2->setBorders (lig, col);
	headerLayout->addWidget (cell2, lig,col, 1,1);
}
//------------------------------------------------------------------------
QWidget * MeteoTableWidget::getDataHeaders ()
{
	return headerWidget;
}
//------------------------------------------------------------------------
void MeteoTableWidget::showEvent (QShowEvent */*event*/)
{
	QRect r;
	// Adjust header height to row height
	for (int j=0; j<layout->rowCount(); j++) 
	{
		QLayoutItem *it1 = layout->itemAtPosition (j,1);
		if (it1 && it1->widget()) {
			QLayoutItem *it2 = headerLayout->itemAtPosition (j,0);
			if (it2 && it2->widget()) 
			{
 				it2->widget()->setMinimumHeight (it1->widget()->height());
			}
		}
	}
}

//-------------------------------------------------------------------------------
MeteoTableWidget::~MeteoTableWidget()
{
	Util::cleanVectorPointers (lspinfos);
	qDeleteAll (listVisibleData);
}
//----------------------------------------------------------------
void MeteoTableWidget::addCell_SunMoonAlmanac (time_t t, double lat, double lon, 
				QGridLayout *layout,int lig,int col, int rowspan,int colspan)
{
	TableCell *cell = new TableCell (this);	
	assert (cell);
	cell->setBorders (lig, col);
	
	Almanac alm = Astro::getSunMoonAlmanac (t, lat, lon);

	QString rs,st;
	QString txt;
	rs = alm.sunRise>0 ? Util::formatTime (alm.sunRise) : "----";
	st = alm.sunSet>0 ? Util::formatTime (alm.sunSet) : "----";
	txt = "⇑ "+rs+" ⇓ "+st+"\n";
	rs = alm.moonRise>0 ? Util::formatTime (alm.moonRise) : "----";
	st = alm.moonSet>0 ? Util::formatTime (alm.moonSet) : "----";
	txt += "⇑ "+rs+" ⇓ "+st;
	txt += "  "+QString("%1%").arg( (int)(100*alm.moonDisk+0.5));
	cell->label->setText (txt);
	layout->setAlignment (Qt::AlignLeft);
	layout->addWidget (cell, lig,col, rowspan,colspan );
}

//-------------------------------------------------------------------------------
void MeteoTableWidget::createTable() 
{
	reader = plotter->getReader();
	if (!reader)
		return;
	
	std::set<time_t> sdates = reader->getListDates();
	std::set<time_t>::iterator iter;
	std::set<time_t>::iterator iter2;
	int lig, col, colspan;
	QString dstr;
	bool showSunMoonAlmanac = Util::getSetting("MTABLE_showSunMoonAlmanac", true).toBool();
	//-----------------------------------------------
	// Titre 1 : une colonne par jour, regroupant plusieurs horaires
	//-----------------------------------------------
	col = 0;
	lig = 0;
	addCell_title_dataline ("", true, lig,col);
	col ++;
	QString actuel = "";
	for (iter=sdates.begin(); iter!=sdates.end(); iter++)
	{
		time_t daterecord = *iter;
		dstr = Util::formatDateLong (daterecord);
		if (dstr != actuel)
		{
			colspan = 0;
			actuel = dstr;
			iter2 = iter;
			do
			{
				colspan ++;
				iter2 ++;
				dstr = Util::formatDateLong(*iter2);
			} while (actuel==dstr);
			addCell_title (actuel, true, layout, lig,col, 1,colspan);
			col += colspan;
		}
	}
			
	//-----------------------------------------------
	// Titre 1 bis: sun moon almanac if needed
	//-----------------------------------------------
	if (showSunMoonAlmanac)
	{
		col = 0;
		lig ++;
		addCell_title_dataline (tr("Sun")+"\n"+tr("Moon"), true, lig,col);
		col ++;
		QString actuel = "";
		for (iter=sdates.begin(); iter!=sdates.end(); iter++)
		{
			time_t daterecord = *iter;
			dstr = Util::formatDateLong (daterecord);
			if (dstr != actuel)
			{
				colspan = 0;
				actuel = dstr;
				iter2 = iter;
				do
				{
					colspan ++;
					iter2 ++;
					dstr = Util::formatDateLong(*iter2);
				} while (actuel==dstr);
				addCell_SunMoonAlmanac (daterecord, lat, lon, layout, lig,col, 1,colspan);
				col += colspan;
			}
		}
	}	
	//-----------------------------------------------
	// Titre 2 : une colonne par date+horaires
	//-----------------------------------------------
	DataPointInfo *pinfo;
	time_t dateproche = reader->getClosestDateFromNow ();
	col = 0;
	lig ++;
	addCell_title_dataline ("", true, lig,col);
	col ++;
	for (iter=sdates.begin(); iter!=sdates.end(); iter++)
	{
		time_t date = *iter;
		addCell_title(Util::formatTime(date), false, layout, lig,col, 1,1,
						dateproche==date);
		col ++;

		// Grib data for this point and this date
		pinfo = new DataPointInfo (reader, lon,lat, date);
		lspinfos.push_back (pinfo);
		lsdates.push_back (date);
	}
	//-----------------------------------------------
	// Contenus
	//-----------------------------------------------
	lig ++;
	createListVisibleGribData();
	QList <MTGribData *>::iterator it;
	for (it=listVisibleData.begin(); it!=listVisibleData.end(); it++) {
		MTGribData *gr = *it;
		int dataType   = gr->dtc.dataType;
		int levelType  = gr->dtc.levelType;
		int levelValue = gr->dtc.levelValue;
		
		if (dataType==GRB_PRV_WIND_XY2D) {
			Altitude alt (levelType, levelValue);
			addLine_Wind (alt, lig++);
		}
		else if (dataType==GRB_PRV_CUR_XY2D) {
			Altitude alt (levelType, levelValue);
			addLine_Current (alt, lig++);
		}
		else if (dataType==GRB_PRESSURE_MSL && levelType==LV_MSL && levelValue==0){
			Altitude alt (levelType, levelValue);
			addLine_Pressure (alt, lig++);
		}
		else if (dataType==GRB_CLOUD_TOT && levelType==LV_ATMOS_ALL && levelValue==0){
			addLine_CloudCover (lig++);
		}
		else if (dataType==GRB_CLOUD_TOT && levelType==LV_ATMOS_ENT && levelValue==0){
			addLine_CloudCover (lig++);
		}
		else if (dataType==GRB_PRECIP_TOT && levelType==LV_GND_SURF && levelValue==0){
			addLine_Rain (lig++);
		}
		else if (dataType==GRB_PRV_THETA_E) {
			Altitude alt (levelType, levelValue);
			addLine_Temperature (alt, GRB_PRV_THETA_E, lig++);
		}
		else if (dataType==GRB_TEMP && levelType!=LV_ATMOS_ALL) {
			Altitude alt (levelType, levelValue);
			addLine_Temperature (alt, GRB_TEMP, lig++);
		}
		else if (dataType==GRB_TMIN) {
			Altitude alt (levelType, levelValue);
			addLine_Temperature (alt, GRB_TMIN, lig++);
		}
		else if (dataType==GRB_TMAX) {
			Altitude alt (levelType, levelValue);
			addLine_Temperature (alt, GRB_TMAX, lig++);
		}
		else if (dataType==GRB_TEMP_POT) {
			Altitude alt (levelType, levelValue);
			addLine_Temperature (alt, GRB_TEMP_POT, lig++);
		}
		else if (dataType==GRB_HUMID_REL) {
			Altitude alt (levelType, levelValue);
			addLine_HumidRel (alt, lig++);
		}
		else if (dataType==GRB_GEOPOT_HGT && levelType!=LV_ISOTHERM0) {
			Altitude alt (levelType, levelValue);
			addLine_GeopotentialAltitude (alt, lig++);
		}
		else if (dataType==GRB_GEOPOT_HGT  && levelType==LV_ISOTHERM0 && levelValue==0){
			addLine_Isotherm0Height (lig++);
		}
		else if (dataType==GRB_DEWPOINT && levelType==LV_ABOV_GND && levelValue==2){
			Altitude alt (levelType, levelValue);
			addLine_DewPoint (alt, lig++);
		}
		else if (dataType==GRB_PRV_DIFF_TEMPDEW && levelType==LV_ABOV_GND && levelValue==2){
			Altitude alt (levelType, levelValue);
			addLine_DeltaTemperature (alt, GRB_PRV_DIFF_TEMPDEW, lig++);
		}
		else if (dataType==GRB_SNOW_CATEG && levelType==LV_GND_SURF && levelValue==0){
			addLine_Categorical (GRB_SNOW_CATEG, lig++);
		}
		else if (dataType==GRB_FRZRAIN_CATEG && levelType==LV_GND_SURF && levelValue==0){
			addLine_Categorical (GRB_FRZRAIN_CATEG, lig++);
		}
		else if (dataType==GRB_SNOW_DEPTH && levelType==LV_GND_SURF && levelValue==0){
			addLine_SnowDepth (lig++);
		}
		else if (dataType==GRB_CAPE && levelType==LV_GND_SURF && levelValue==0){
			addLine_CAPEsfc (lig++);
		}
		else if (dataType==GRB_CIN && levelType==LV_GND_SURF && levelValue==0){
			addLine_CINsfc (lig++);
		}
		else if (dataType==GRB_TEMP && levelType==LV_ATMOS_ALL && levelValue==0){
			addLine_SkewT (lig++);
		}
		else if (dataType==GRB_WIND_GUST && levelType==LV_GND_SURF && levelValue==0){
			addLine_GUSTsfc (lig++);
		}
		//----------------------------------------------
		// Waves
		//----------------------------------------------
		else if (dataType==GRB_WAV_SIG_HT
				|| dataType==GRB_WAV_WND_HT
				|| dataType==GRB_WAV_SWL_HT
				|| dataType==GRB_WAV_MAX_HT
		){
			addLine_WaveHeight (dataType, lig++);
		}
		else if (dataType==GRB_PRV_WAV_MAX
				|| dataType==GRB_PRV_WAV_WND
				|| dataType==GRB_PRV_WAV_SWL
				|| dataType==GRB_PRV_WAV_PRIM
				|| dataType==GRB_PRV_WAV_SCDY
		){
			addLine_WaveCompleteCell (dataType, lig++);
		}
		else if (dataType==GRB_WAV_WHITCAP_PROB) {
			addLine_WaveWhitecap (dataType, lig++);
		}
		
		//----------------------------------------------
		// Unknown data
		//----------------------------------------------
		else {
			DBG ("unknown data: %d %d %d", dataType,levelType,levelValue);
			DBGQS (DataCodeStr::toString(gr->dtc) );
		}
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_WaveWhitecap (int type, int lig)
{
	std::vector <time_t>::iterator it; 
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline  (tr("Whitecap (prob)"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		double v = pinfo->getWaveData (type);
		if (v != GRIB_NOTDEF) {
			txt = Util::formatPercentValue (v);
			bgColor = QColor(plotter->getWhiteCapColor (v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_WaveCompleteCell (int prvtype, int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (DataCodeStr::toString(prvtype), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		float ht, per, dir;
		pinfo->getWaveValues (prvtype, &ht, &per, &dir);
		if (ht != GRIB_NOTDEF) {
			txt = Util::formatWaveHeight (ht);
			bgColor = QColor(plotter->getWaveHeightColor (ht, true));
		}
		else {
			bgColor = Qt::white;
		}
		txt += "\n";
		if (dir != GRIB_NOTDEF) {
			txt += Util::formatWaveDirection (dir, true);
		}
		txt += "\n";
		if (per != GRIB_NOTDEF) {
			txt += Util::formatWavePeriod (per, true);
		}
		addCell_content (txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_WaveHeight (int type, int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (DataCodeStr::toString(type), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		double v = pinfo->getWaveData (type);
		if (v != GRIB_NOTDEF) {
			txt = Util::formatWaveHeight (v);
			bgColor = QColor(plotter->getWaveHeightColor (v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}

//-----------------------------------------------------------------
bool lessThanMTGribData(const MTGribData *a, const MTGribData *b)
{
	return (a->pos < b->pos);
}

//-----------------------------------------------------------------
void MeteoTableWidget::createListVisibleGribData ()
{
	qDeleteAll (listVisibleData);
	
	QStringList listKeys = Settings::getAllKeys();
	QStringList::const_iterator cstit;
	bool foundMTableData = false;
	for (cstit = listKeys.constBegin(); cstit != listKeys.constEnd(); cstit++) {
		QString key = *cstit;
		if (key.startsWith("MTableData_vis_"))
		{
			foundMTableData = true;
			if (Util::getSetting(key, false).toBool()) {
				// key like MTableData_vis_0002690b
				QString gribCodeStr = key;
				gribCodeStr = gribCodeStr.replace("MTableData_vis_","");	// extracts gribcode
				QString keyPos = QString("MTableData_pos_")+gribCodeStr;
				int pos = Util::getSetting (keyPos, 1000).toInt();

				MTGribData *group = new MTGribData(gribCodeStr, pos);
				listVisibleData.append (group);
			}
		}
	}
	if (!foundMTableData) {
		// No MTableData_... in settings : initialize first list (arbitrary)
		int pos = 0;
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_PRV_WIND_XY2D,LV_ABOV_GND,10).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_CLOUD_TOT,LV_ATMOS_ALL,0).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_PRECIP_TOT,LV_GND_SURF,0).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_TEMP,LV_ABOV_GND,2).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_PRV_DIFF_TEMPDEW,LV_ABOV_GND,2).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
    				DataCode(GRB_GEOPOT_HGT,LV_ISOTHERM0,0).toInt32(), pos++) );
    	listVisibleData.append( new MTGribData (
					DataCode(GRB_PRESSURE_MSL,LV_MSL,0).toInt32(), pos++) );
	}
	// Sort visible data by position
	qSort (listVisibleData.begin(), listVisibleData.end(), lessThanMTGribData);
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Isotherm0Height(int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Isotherm 0°C"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasIsotherm0HGT()) {
			txt = Util::formatGeopotAltitude (pinfo->isotherm0HGT);
			bgColor = QColor(plotter->getAltitudeColor
						(pinfo->isotherm0HGT, Altitude(LV_ISOTHERM0,0), true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_GeopotentialAltitude(const Altitude &alt, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Geopotential altitude") +" ("+AltitudeStr::toStringShort(alt)+")", 
				  true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++) {
		DataPointInfo * pinfo = *iter;
		txt = "";
		float v = pinfo->getDataValue (DataCode(GRB_GEOPOT_HGT,alt));
		if (v != GRIB_NOTDEF) {
			txt = Util::formatGeopotAltitude (v);
			bgColor = QColor(plotter->getAltitudeColor(v, alt, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Pressure(const Altitude &alt, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Pressure") +" ("+AltitudeStr::toStringShort(alt)+")", 
				  true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++) {
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasPressureMSL()) {
			txt = Util::formatPressure (pinfo->pressureMSL);
			bgColor = QColor(plotter->getPressureColor(pinfo->pressureMSL, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Wind (const Altitude &alt, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	std::vector <time_t>::iterator it; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Wind")+" ("+AltitudeStr::toStringShort(alt)+")", 
					layout, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++) {
		DataPointInfo * pf = *iter;
		float v, dir;
		txt = "";
		bgColor = Qt::white;
		if (pf->getWindValues (alt, &v, &dir)) {
			if (dir != GRIB_NOTDEF) {
				QString tmp;
				tmp.sprintf("%.0f", dir);
				txt += tmp + tr(" °") + "\n";
				txt += Util::formatSpeed_Wind(v);
				if ( Util::getSetting("MTABLE_showWindBeauforts", true).toBool() ) {
					tmp.sprintf("%2d", Util::msToBeaufort(v));
					txt += "\n";
					txt += tmp + tr(" Bf");
				}
				bgColor = QColor(plotter->getWindColor(v, true));
			}
		}
		float vx, vy;
		pf->getWindVxVy (alt, &vx, &vy);   
		addCell_content (txt, layout,lig,col, 1,1, bgColor, MTABLE_WIND_CELL,vx,vy);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Current (const Altitude &alt, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	std::vector <time_t>::iterator it; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Current")+" ("+AltitudeStr::toStringShort(alt)+")", 
					layout, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++) {
		DataPointInfo * pf = *iter;
		float v, dir;
		txt = "";
		bgColor = Qt::white;
		if (pf->getCurrentValues (&v, &dir)) {
			if (dir != GRIB_NOTDEF) {
				QString tmp;
				tmp.sprintf("%.0f", dir);
				txt += tmp + tr(" °") + "\n";
				txt += Util::formatSpeed_Current(v);
				bgColor = QColor(plotter->getCurrentColor(v, true));
			}
		}
		float cx, cy;
		pf->getCurrentCxCy (alt, &cx, &cy);   
		addCell_content (txt, layout,lig,col, 1,1, bgColor, MTABLE_CURRENT_CELL,cx,cy);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_GUSTsfc(int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Wind gust"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++) {
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasGUSTsfc()) {
			double v = pinfo->GUSTsfc;
			txt = Util::formatSpeed_Wind(v);
			bgColor = QColor(plotter->getWindColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_HumidRel (const Altitude &alt, int lig)
{
	std::vector <time_t>::iterator it; 
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	//addCell_title(tr("Relative humidity"), true, layout, lig,col);
	addCell_title_dataline  (tr("Relative humidity")+" ("+AltitudeStr::toStringShort(alt)+")", 
					true, lig,col);
	col ++;
	for (it=lsdates.begin(); it!=lsdates.end(); it++, col++)
	{
		time_t date = *it;
		txt = "";
		double v = 0;
		v = reader->getDateInterpolatedValue (DataCode(GRB_HUMID_REL,alt), lon,lat, date);
		if (v != GRIB_NOTDEF) {
			txt = Util::formatPercentValue(v);
			bgColor = QColor(plotter->getHumidColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Temperature(const Altitude &alt, uchar type, int lig)
{
	std::vector <time_t>::iterator it; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	double v;
	QString title;
	switch (type) {
		case GRB_PRV_THETA_E:
			title = tr("Theta-e"); break;
		case GRB_TEMP:
			title = tr("Temperature"); break;
		case GRB_TMIN:
			title = tr("Temp. min"); break;
		case GRB_TMAX:
			title = tr("Temp. max"); break;
		case GRB_TEMP_POT:
			title = tr("Temp. pot"); break;
	}
	title += " ("+AltitudeStr::toStringShort(alt)+")";
	addCell_title_dataline (title, true, lig,col);
	col ++;
	for (it=lsdates.begin(); it!=lsdates.end(); it++, col++) {
		time_t date = *it;
		if (type == GRB_PRV_THETA_E) {
			int P = alt.levelValue;	// 925 850 700 600 500 400 300 200
			double RH = reader->getDateInterpolatedValue (DataCode(GRB_HUMID_REL,alt), lon,lat,date);
			double T = reader->getDateInterpolatedValue (DataCode(GRB_TEMP,alt), lon,lat,date);
			v = Therm::thetaEfromHR (T, P, RH);
		}
		else
			v = reader->getDateInterpolatedValue (DataCode(type,alt), lon,lat, date);
		txt = "";
		if (v != GRIB_NOTDEF) {
			txt = Util::formatTemperature(v);
			bgColor = QColor(plotter->getTemperatureColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_DeltaTemperature(const Altitude &alt, uchar type, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	double v;
	switch (type) {
		case GRB_PRV_DIFF_TEMPDEW:
		default:
			addCell_title_dataline (tr("Gap temp-dew point")+" ("+AltitudeStr::toStringShort(alt)+")", 
							true, lig,col);
			break;
	}
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		switch (type) {
			case GRB_PRV_DIFF_TEMPDEW:
			default:
				v = fabs(pinfo->temp - pinfo->dewPoint);
				break;
		}
		txt = "";
		if (pinfo->temp != GRIB_NOTDEF && pinfo->dewPoint != GRIB_NOTDEF) {
			txt = Util::formatTemperature(v + 273.15);
			bgColor = QColor(plotter->getDeltaTemperaturesColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_DewPoint(const Altitude &alt, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Dew point")+" ("+AltitudeStr::toStringShort(alt)+")", 
				  true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasDewPoint()) {
			double v = pinfo->dewPoint;
			txt = Util::formatTemperature(v);
			bgColor = QColor(plotter->getTemperatureColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_CAPEsfc (int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("CAPE (surface)"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasCAPEsfc()) {
			double v = pinfo->CAPEsfc;
			txt.sprintf("%d ", qRound(v));
			txt += tr("J/kg");
			bgColor = QColor(plotter->getCAPEColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content (txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_CINsfc (int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("CIN (surface)"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasCINsfc()) {
			double v = pinfo->CINsfc;
			txt.sprintf("%d ", qRound(v));
			txt += tr("J/kg");
			bgColor = QColor(plotter->getCINColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content (txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Rain(int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Precipitation"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		if (pinfo->hasRain()) {
			double v = pinfo->rain;
			txt.sprintf("%.2f ", v);
			txt += tr("mm/h");
			bgColor = QColor(plotter->getRainColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content (txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_CloudCover (int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	addCell_title_dataline (tr("Cloud cover"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		txt = "";
		double v = 0;
		if (pinfo->hasCloudTotal()) {
			v = pinfo->cloudTotal;
			txt = Util::formatPercentValue(v);
			bgColor = QColor(plotter->getCloudColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor, MTABLE_CLOUD_CELL, v);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_Categorical (uchar type, int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	double v = -1000;
	switch (type) {
		case GRB_FRZRAIN_CATEG:
			addCell_title_dataline (tr("Frozen rain possible"), true, lig,col);
			break;
		case GRB_SNOW_CATEG:
			addCell_title_dataline (tr("Snowfall possible"), true, lig,col);
			break;
	}
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		switch (type) {
			case GRB_FRZRAIN_CATEG:
				v = pinfo->frzRainCateg;
				break;
			case GRB_SNOW_CATEG:
				v = pinfo->snowCateg;
				break;
		}
		if (v != GRIB_NOTDEF) {
			txt = Util::formatCategoricalData (v);
			bgColor = QColor(plotter->getSnowDepthColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_SnowDepth (int lig)
{
	std::vector <DataPointInfo *>::iterator iter; 
	QColor    bgColor = Qt::white;
	QString   txt;
	int col = 0;
	double v = -1000;
	addCell_title_dataline (tr("Snow"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo * pinfo = *iter;
		v = pinfo->snowDepth;
		txt = "";
		if (v >= 0) {
			txt.sprintf("%.2f", v);
			txt = Util::formatSnowDepth(v);
			bgColor = QColor(plotter->getSnowDepthColor(v, true));
		}
		else
			bgColor = Qt::white;
		addCell_content(txt, layout,lig,col, 1,1, bgColor);
	}
}
//-----------------------------------------------------------------
void MeteoTableWidget::addLine_SkewT (int lig)
{
	std::vector <DataPointInfo *>::iterator iter;
	int col = 0;
	addCell_title_dataline (tr("SkewT-LogP"), true, lig,col);
	col ++;
	for (iter=lspinfos.begin(); iter!=lspinfos.end(); iter++, col++)
	{
		DataPointInfo *pinfo = *iter;
		addCell_SkewT (layout,lig,col, pinfo->date);
	}
}

//===================================================================
void MeteoTableWidget::addCell_SkewT (
				QGridLayout *layout,int lig,int col,
				time_t date
				)
{
	TableCell *cell = new TableCell_SkewT (this, lon, lat, locationName, date, reader);	
	assert (cell);
	cell->setBorders (lig, col);
	layout->addWidget (cell, lig,col, 1,1, Qt::AlignCenter);
}
//----------------------------------------------------------------
void MeteoTableWidget::addCell_content (
				QString txt,
				QGridLayout *layout,int lig,int col,
				int    rowspan,
				int    colspan,
				QColor bgcolor,
				int    cellType,
				double  vx,
				double  vy
				)
{
	TableCell *cell;
 	if (cellType == MTABLE_WIND_CELL) {
	 	cell = new TableCell_Wind (vx, vy, (lat<0), plotter,
	 				this, txt, false, bgcolor);
 	}
 	else if (cellType == MTABLE_CURRENT_CELL) {
	 	cell = new TableCell_Current (vx, vy, (lat<0), plotter,
	 				this, txt, false, bgcolor);
 	}
 	else if (cellType == MTABLE_CLOUD_CELL) {
	 	cell = new TableCell_Clouds (vx, plotter,
	 				this, txt, false, bgcolor);
 	}
 	else {
	 	cell = new TableCell (this, txt, false, bgcolor);
	}
	assert (cell);
	cell->setBorders (lig, col);
	layout->addWidget (cell, lig,col, rowspan,colspan );
}
//----------------------------------------------------------------
void MeteoTableWidget::addCell_title (QString txt, bool bold,
				QGridLayout *layout,int lig,int col, int rowspan,int colspan,
				bool isNowDate)
{
	QColor bgcolor(200,200,255);
	if (isNowDate) {
		bgcolor = QColor(250,250,100);
	}
	TableCell *cell = new TableCell(this, txt, bold, bgcolor);	
	assert (cell);
	cell->setBorders (lig, col);
	layout->addWidget (cell, lig,col, rowspan,colspan );
}
//===================================================================
// TableCell : case seule
//===================================================================
TableCell::TableCell(QWidget *parent, QString txt, bool bold,
						QColor bgcolor,
						Qt::Alignment alignment
					)
	: QWidget(parent)
{
	this->bgcolor = bgcolor;
	this->bordercolor = QColor(100,100,100);
	this->borders = TableCell::none;
	
	label = new QLabel (txt, this);
//	label->setAlignment (alignment);
	if (bold) {
		QFont font;
		font.setBold(true);
		label->setFont(font);
	}
	setContrastedTextColor(this->bgcolor);
	
	QGridLayout *layout = new QGridLayout (this);
	int mh=2, mv=4;
	layout->setContentsMargins (mh,mv,mh,mv);
//	layout->addWidget(label, 0,0, Qt::AlignHCenter|Qt::AlignBottom);
	layout->addWidget(label, 0,0, alignment);
}
//----------------------------------------------------------
void    TableCell::setContrastedTextColor(QColor bgcolor)
{
	QColor fgcolor = DataColors::getContrastedColor (bgcolor);
	QPalette p;
	p.setBrush(QPalette::Active, QPalette::WindowText, fgcolor);
	p.setBrush(QPalette::Inactive, QPalette::WindowText, fgcolor);
	label->setPalette(p);
}
//---------------------------------------------------------
void TableCell::setBorders (int lig, int col)
{
	if (lig==0 && col==0)
		this->setBorders (TableCell::all);
	else if (lig==0)
		this->setBorders (TableCell::south+TableCell::east+TableCell::north);
	else if (col==0)
		this->setBorders (TableCell::west+TableCell::south+TableCell::east);
	else
		this->setBorders (TableCell::south+TableCell::east);
}
//---------------------------------------------------------
void TableCell::paintEvent(QPaintEvent * /*event*/)
{
    QPainter pnt(this);
	
	pnt.fillRect(0,0,width(),height(), QBrush(bgcolor));
	
	QPen pen(bordercolor);
	pen.setWidth(1);
	pnt.setPen(pen);
	
	if (borders & TableCell::north)
		pnt.drawLine(0,0, width()-1,0);
	if (borders & TableCell::south)
		pnt.drawLine(0,height()-1, width()-1,height()-1);
	
	if (borders & TableCell::west)
		pnt.drawLine(0,0, 0,height()-1);
	if (borders & TableCell::east)
		pnt.drawLine(width()-1,0, width()-1,height()-1);
	
}


//===================================================================
// TableCell_Wind : case seule spécialisée pour le vent (flêche+barbules)
//===================================================================
TableCell_Wind::TableCell_Wind (double vx, double vy, bool south,
        			GriddedPlotter *plotter,
        			QWidget *parent, QString txt, bool bold,
        			QColor bgcolor )
	: TableCell(parent, txt, bold, bgcolor)
{
	this->vx = vx;
	this->vy = vy;
	this->south = south;
	this->plotter = plotter;

	windArrowsColor = QColor(40,40,40);
	showWindArrows = Util::getSetting("MTABLE_showWindArrows", true).toBool();

	if (showWindArrows && vx!=GRIB_NOTDEF && vy!=GRIB_NOTDEF)
		setMinimumHeight(label->minimumSizeHint().height()+50);
}	
//---------------------------------------------------------
void TableCell_Wind::paintEvent(QPaintEvent * e)
{
	TableCell::paintEvent(e);
    QPainter pnt(this);
	pnt.setRenderHint(QPainter::Antialiasing, true);

	if (showWindArrows && vx != GRIB_NOTDEF && vx != GRIB_NOTDEF)
	{
    	plotter->drawWindArrowWithBarbs(
    			pnt, width()/2, 25, vx, vy, south, windArrowsColor);
	}
}


//===================================================================
// TableCell_Current : case seule spécialisée pour le courant (flêche)
//===================================================================
TableCell_Current::TableCell_Current (double cx, double cy, bool south,
        			GriddedPlotter *plotter,
        			QWidget *parent, QString txt, bool bold,
        			QColor bgcolor )
	: TableCell(parent, txt, bold, bgcolor)
{
	this->cx = cx;
	this->cy = cy;
	this->south = south;
	this->plotter = plotter;

	currentArrowsColor = QColor(40,40,40);
	showCurrentArrows = Util::getSetting("MTABLE_showCurrentArrows", true).toBool();

	if (showCurrentArrows && cx!=GRIB_NOTDEF && cy!=GRIB_NOTDEF)
		setMinimumHeight(label->minimumSizeHint().height()+50);
}	
//---------------------------------------------------------
void TableCell_Current::paintEvent(QPaintEvent * e)
{
	TableCell::paintEvent(e);
    QPainter pnt(this);
	pnt.setRenderHint(QPainter::Antialiasing, true);

	if (showCurrentArrows && cx != GRIB_NOTDEF && cx != GRIB_NOTDEF)
	{
    	plotter->drawCurrentArrow (
    			pnt, width()/2, 25, cx, cy, south, currentArrowsColor);
	}
}


//===================================================================
// TableCell_Clouds : case seule spécialisée pour la nébulosité
//===================================================================
TableCell_Clouds::TableCell_Clouds (
					double   val,
        			GriddedPlotter *plotter,
        			QWidget  *parent, QString txt, bool bold,
        			QColor bgcolor )
	: TableCell(parent, txt, bold, bgcolor)
{
	this->plotter = plotter;
	this->val = val;

	// Color = seaColor + bgColor
	QColor seaColor (50,50,200, 255);
	QImage img (1,1, QImage::Format_ARGB32_Premultiplied);
	QPainter pnt (&img);
	
	plotter->setCloudsColorMode("MTABLE_cloudsColorMode");
	QColor cloudColor = QColor::fromRgba(plotter->getCloudColor(val, true));
	plotter->setCloudsColorMode("cloudsColorMode");
		
	pnt.fillRect (0,0, 1,1, seaColor );
	pnt.fillRect (0,0, 1,1, cloudColor);
	this->bgcolor = QColor(img.pixel(0,0));
	
	setContrastedTextColor(this->bgcolor);
}	
//---------------------------------------------------------
void TableCell_Clouds::paintEvent(QPaintEvent * e)
{
	TableCell::paintEvent(e);
    QPainter pnt(this);
	pnt.setRenderHint(QPainter::Antialiasing, true);

}



//===================================================================
// TableCell_SkewT
//===================================================================
TableCell_SkewT::TableCell_SkewT (QWidget *parent,
					double lon, double lat,  QString locationName,
					time_t date,
					GriddedReader *reader)
	: TableCell (parent)
{
	this->lon = lon;
	this->lat = lat;
	this->locationName = locationName;
	this->date = date;
	this->reader = reader;
	MiniSkewT miniskewt;
	miniskewt.initFromGriddedReader (reader, lon, lat, date);
	pixmap = miniskewt.createPixmap ();
	setFixedSize (miniskewt.size());
}
//---------------------------------------------------------
void TableCell_SkewT::paintEvent (QPaintEvent * e)
{
	TableCell::paintEvent (e);
    QPainter pnt (this);
	pnt.setRenderHint(QPainter::Antialiasing, true);
	pnt.drawPixmap (0,0, pixmap);
}
//---------------------------------------------------------
void TableCell_SkewT::enterEvent (QEvent *)
{
	setCursor (Qt::PointingHandCursor);
}
//---------------------------------------------------------
void TableCell_SkewT::leaveEvent (QEvent *)
{
	unsetCursor ();
}
//---------------------------------------------------------
void TableCell_SkewT::mouseReleaseEvent (QMouseEvent *e)
{
	if (this->rect().contains (e->pos())) {
		SkewT *skewt = new SkewT ();
		skewt->initFromGriddedReader (reader, lon, lat, date);
		
		SkewTWindow *sdial = new SkewTWindow (skewt);
		
		QString position = Util::formatPosition(lon, lat);
		if (locationName == "") {
			skewt->setLocation (position);
			sdial->setWindowTitle ("SkewT - "+position);
		}
		else {
			skewt->setLocation (position+" ("+locationName+")");
			sdial->setWindowTitle ("SkewT - "+locationName);
		}
		sdial->show ();
	}
}

	
	
	
	

