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

You should have received a cob of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include <unistd.h>
#include <time.h>

#include <QDir>
#include <QStringList>

#include <QUrl>
#include <QNetworkProxy>
#include <QNetworkProxyQuery>
#include <QCryptographicHash>
#include <QFileDialog>

#include "Settings.h"
#include "Util.h"
#include "Version.h"


//======================================================================
QHash <QString, QVariant> GLOB_hashSettings;

void Util::setSetting (const QString &key, const QVariant &value)
{
	GLOB_hashSettings.insert (key, value);
	Settings::setUserSetting (key, value);
}
//---------------------------------------------------------------------
QVariant Util::getSetting (const QString &key, const QVariant &defaultValue)
{
	if (GLOB_hashSettings.contains (key) )
	{
		return GLOB_hashSettings.value (key);
	}
	else
	{
		QVariant v = Settings::getUserSetting (key, defaultValue);
		GLOB_hashSettings.insert (key, v);
		return v;
	}
}
//========================================================================
QString Util::getSaveFileName (QWidget *parent, const QString &caption, 
							const QString &dir, const QString &filter)
{
	#ifdef Q_OS_MACX
		if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
		{   // fix Mac OS X 10.9 (mavericks) QFileDialog bug
			int useNative = Util::getSetting("mac_useNativeFileDialog", 999).toInt();
			if (useNative == 999) {
				Util::setSetting("mac_useNativeFileDialog", 0);
				useNative = 0;
			}
			if (useNative == 0) {
				return QFileDialog::getSaveFileName (parent, caption, dir, filter, 
							0, QFileDialog::DontUseNativeDialog);
			}
		}
	#endif
	return QFileDialog::getSaveFileName (parent, caption, dir, filter);
}
//------------------------------------------------------------
QString Util::getOpenFileName (QWidget *parent, const QString &caption, 
							const QString &dir, const QString &filter)
{
	#ifdef Q_OS_MACX
		if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
		{   // fix Mac OS X 10.9 (mavericks) QFileDialog bug
			int useNative = Util::getSetting("mac_useNativeFileDialog", 999).toInt();
			if (useNative == 999) {
				Util::setSetting("mac_useNativeFileDialog", 0);
				useNative = 0;
			}
			if (useNative == 0) {
				return QFileDialog::getOpenFileName (parent, caption, dir, filter, 
							0, QFileDialog::DontUseNativeDialog);
			}
		}
	#endif
	return QFileDialog::getOpenFileName (parent, caption, dir, filter);
}
//------------------------------------------------------------
QString Util::getServerName ()
{
	return "www.zygrib.org";
}
//------------------------------------------------------------
void Util::setApplicationProxy ()
{
	QNetworkProxy proxy;
	
	if (Util::getSetting("httpUseProxy", false).toBool())
	{
		proxy.setType ((QNetworkProxy::ProxyType)
						(Util::getSetting("httpProxyType", QNetworkProxy::HttpProxy).toInt()));
		proxy.setHostName (Util::decode(Util::getSetting("httpProxyHostname", "").toString()));
		proxy.setPort     (Util::getSetting("httpProxyPort", 0).toInt());
		proxy.setUser     (Util::decode(Util::getSetting("httpProxyUsername", "").toString()));
		proxy.setPassword (Util::decode(Util::getSetting("httpProxyUserPassword", "").toString()));
		
		if (proxy.type() == QNetworkProxy::DefaultProxy)
		{
			QNetworkProxyQuery proxq = QNetworkProxyQuery (QUrl("http://"+getServerName()+"/index.php"));
			QList<QNetworkProxy> list = QNetworkProxyFactory::systemProxyForQuery (proxq);
			if (list.size() > 0) {
				QNetworkProxy pr2 = list [0];
				QNetworkProxy::setApplicationProxy (pr2);
			}
			else {
				QNetworkProxy::setApplicationProxy (proxy);
			}
		}
		else {
			QNetworkProxy::setApplicationProxy (proxy);
		}
	}
	else {
		proxy.setType (QNetworkProxy::NoProxy);
		QNetworkProxy::setApplicationProxy (proxy);
	}
}
//-------------------------------------------------------------------------------
QNetworkRequest Util::makeNetworkRequest (QString url,double x0,double y0,double x1,double y1)
{
	QNetworkRequest request;
	QString now = QTime::currentTime().toString("HHmmss");
	if (url.contains('?'))
		url += "&tm="+now;
	else
		url += "?tm="+now;
	if (Util::getSetting ("strictHttpDownload", false).toBool()) {
		QString zl = Util::encode(Util::decode(Util::getSetting("zyGribForumUserName", "").toString()),now);
		QString zp = Util::encode(Util::decode(Util::getSetting("zyGribForumUserPassword", "").toString()),now);
		url += "&zl="+zl+"&zp="+zp+"&md=h";
		request.setRawHeader ("User-Agent","Mozilla/5.0 (Windows NT 5.1; rv:8.0) Gecko/20100101 Firefox/8.0");
 		request.setRawHeader ("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	}
	else
		request.setRawHeader ("User-Agent",qPrintable(Version::getAppName()+"/"+Version::getVersion()) );
	request.setUrl (QUrl(url));
	validArea (request,x0,y0,x1,y1);
	return request;
}
//======================================================================
bool Util::isDirWritable (const QDir &dir)
{
	if (! dir.exists())
		return false;
		
	// try yo write a file
	FILE *fd;
	QString tmpfname = dir.absolutePath() + "/jgj13642hygg54hjgiouhg43.tmp";
	fd = fopen( qPrintable(tmpfname), "w");
	if (fd != NULL
			&& fwrite(&tmpfname,1,1,fd)==1 )
	{
		fclose(fd);
		unlink( qPrintable(tmpfname) );
		return true;
	}
	else
		return false;
}
    
//======================================================================
float Util::convertTemperature (float tempKelvin)
{
    QString unit = Util::getSetting("unitsTemp", QVariant(tr("°C"))).toString();
    if (unit == tr("°K")) {
        return tempKelvin;
    }
    else if (unit == tr("°F")) {
        return 1.8*(tempKelvin-273.15)+32.0;
    }
    else  {
        return tempKelvin-273.15;
    }
}
//-------------------------------------------------------
QString Util::formatTemperature (float tempKelvin, bool withUnit)
{
    QString unit = Util::getSetting("unitsTemp", QVariant(tr("°C"))).toString();
    QString r;
    if (unit == tr("°K")) {
        r.sprintf("%.1f", tempKelvin);
    }
    else if (unit == tr("°F")) {
        r.sprintf("%.1f", 1.8*(tempKelvin-273.15)+32.0);
    }
    else  {   // if (unit == tr("°K"))
        unit = tr("°C");
        r.sprintf("%.1f", tempKelvin-273.15);
    }
	return (withUnit) ? r+""+unit : r;
}
//-------------------------------------------------------
QString Util::formatTemperature_short(float tempKelvin, bool withUnit)
{
    QString unit = Util::getSetting("unitsTemp", tr("°C")).toString();
    QString r;
    if (unit == tr("°K")) {
        r.sprintf("%d", qRound(tempKelvin) );
    }
    else if (unit == tr("°F")) {
        r.sprintf("%d", qRound(1.8*(tempKelvin-273.15)+32.0) );
    }
    else  {   // if (unit == tr("°K"))
        unit = tr("°C");
        r.sprintf("%d", qRound(tempKelvin-273.15) );
    }
	return (withUnit) ? r+""+unit : r;
}
//----------------------------------------------------------------
QString Util::formatSpeed (float meterspersecond, bool withUnit, QString unit)
{
    QString r;
    if (unit == "m/s") {
        r.sprintf("%.1f ", meterspersecond);
        unit = tr("m/s");
    }
    else if (unit == "km/h") {
        r.sprintf("%.1f ", meterspersecond*3.6);
        unit = tr("km/h");
    }
    else  {   // if (unit == tr("kts"))
        r.sprintf("%.1f ", meterspersecond*3.6/1.852);
        unit = tr("kts");
    }
	return (withUnit) ? r+" "+unit : r;
}
//----------------------------------------------------------------
QString Util::formatSpeed_Wind (float meterspersecond, bool withUnit)
{
    QString unit = Util::getSetting("unitsWindSpeed", tr("km/h")).toString();
	return Util::formatSpeed (meterspersecond, withUnit, unit);
}
//----------------------------------------------------------------
QString Util::formatSpeed_Current (float meterspersecond, bool withUnit)
{
    QString unit = Util::getSetting("unitsCurrentSpeed", tr("kts")).toString();
	return Util::formatSpeed (meterspersecond, withUnit, unit);
}
//----------------------------------------------------------------
QString Util::formatDirection (float angle, bool withUnit)
{
    QString unite = tr("°");
    QString r;
	r.sprintf("%d", (int) (angle+0.5));
	return (withUnit) ? r+unite : r;
}
//----------------------------------------------------------------
QString Util::formatDistance (float mille, bool withUnit)
{
    QString unit = Util::getSetting("unitsDistance", tr("km")).toString();
    QString r;
    float d;
    if (unit == tr("km")) {
        unit = "km";
        d= mille*1.852;
    }
    else  {
        unit = "NM";
        d = mille;
    }
    if (d<10)
        r.sprintf("%5.2f", d);
    else if (d<100)
        r.sprintf("%5.1f", d);
    else
        r.sprintf("%5.0f", d);
	return (withUnit) ? r+" "+unit : r;
}
//----------------------------------------------------------------
QString Util::getDataUnit (int dataType)
{
	return getDataUnit (DataCode(dataType,LV_GND_SURF,0));
}
//----------------------------------------------------------------
QByteArray Util::sha1 (const QByteArray &v) 
{
	return QCryptographicHash::hash(v, QCryptographicHash::Sha1).toHex();
}
//----------------------------------------------------------------
QString Util::encode (const QString &v, const QString &k) {
	QByteArray pv = v.toUtf8 ();
	QString ke = k.trimmed();
	if (ke == "") ke="zygrib";
	QByteArray pk = Util::sha1 (ke.toUtf8 ());
	QByteArray re;
	int i, j=0;
	for (i=0; i<pv.size(); i++) {
		re.append (pv[i]^pk[j]);
		j++;
		if (j==pk.size()) j=0;
	}
	return QString(re.toBase64());
}
//----------------------------------------------------------------
QString Util::decode (const QString &v, const QString &k) {
	QByteArray pv = QByteArray::fromBase64 (v.toUtf8());
	QString ke = k.trimmed();
	if (ke == "") ke="zygrib";
	QByteArray pk = Util::sha1 (ke.toUtf8 ());
	QByteArray re;
	int i, j=0;
	for (i=0; i<pv.size(); i++) {
		re.append (pv[i]^pk[j]);
		j++;
		if (j==pk.size()) j=0;
	}
	return QString(re);
}
//----------------------------------------------------------------
QString Util::getDataUnit (const DataCode &dtc)
{
	QString unit;
	switch (dtc.dataType) {
		case GRB_GEOPOT_HGT:
			if (dtc.levelType == LV_ISOTHERM0)
				unit = Util::getSetting("geopotAltitudeUnit", "gpm").toString();
			else
				unit = Util::getSetting("isotherm0Unit", "m").toString();
			if (unit == "dam")
				return tr("dam");
			else if (unit == "ft")
				return tr("ft");
			else   // if (unit == "m")
				return tr("m");
			break;
		case GRB_CLOUD_TOT    : 
		case GRB_HUMID_SPEC   : 
		case GRB_HUMID_REL    : 
			return "%";
			break;
		case GRB_FRZRAIN_CATEG: 
		case GRB_SNOW_CATEG   : 
			return "0/1";
			break;
		case GRB_TEMP         : 
		case GRB_TEMP_POT     : 
		case GRB_TMAX         : 
		case GRB_TMIN         : 
		case GRB_DEWPOINT     : 
		case GRB_PRV_DIFF_TEMPDEW : 
		case GRB_PRV_THETA_E      : 
			return Util::getSetting("unitsTemp", tr("°C")).toString();
			break;
		case GRB_WIND_VX    : 
		case GRB_WIND_VY    : 
		case GRB_WIND_SPEED : 
		case GRB_PRV_WIND_XY2D : 
		case GRB_PRV_WIND_JET  : 
			return Util::getSetting("unitsWindSpeed", tr("km/h")).toString();
			break;
		case GRB_CUR_VX      : 
		case GRB_CUR_VY      : 
		case GRB_PRV_CUR_XY2D    : 
			return Util::getSetting("unitsCurrentSpeed", tr("kts")).toString();
			break;
		case GRB_CAPE 		  : 
		case GRB_CIN 		  : 
			return tr("J/kg");
			break;
		case GRB_SNOW_DEPTH   : 
			unit = Util::getSetting("snowDepthUnit", tr("m")).toString();
			if (unit == tr("m"))
				unit = tr("cm");
			return unit;
			break;
		case GRB_PRECIP_RATE  : 
		case GRB_PRECIP_TOT   : 
			return tr("mm/h");
			break;
		case GRB_PRESSURE_MSL : 
			return tr("hPa");
			break;
		case GRB_WAV_SIG_HT : 
		case GRB_WAV_WND_HT : 
		case GRB_WAV_SWL_HT : 
		case GRB_WAV_MAX_HT : 
			return tr("m");
			break;
		case GRB_WIND_DIR : 
		case GRB_WAV_WND_DIR : 
		case GRB_WAV_SWL_DIR : 
		case GRB_WAV_MAX_DIR : 
		case GRB_WAV_PRIM_DIR : 
		case GRB_WAV_SCDY_DIR : 
			return tr("°");
			break;
		case GRB_WAV_WND_PER : 
		case GRB_WAV_SWL_PER : 
		case GRB_WAV_MAX_PER : 
		case GRB_WAV_PRIM_PER : 
		case GRB_WAV_SCDY_PER : 
			return tr("s");
			break;
		case GRB_WAV_WHITCAP_PROB : 
			return tr("%");
			break;
		default:
			return "?";
	}
	return "?";
}
//----------------------------------------------------------------
double Util::getDataCoef (const DataCode &dtc)
{
	QString unit;
	switch (dtc.dataType) {
		case GRB_GEOPOT_HGT:
			if (dtc.levelType == LV_ISOTHERM0)
				unit = Util::getSetting("isotherm0Unit", "m").toString();
			else
				unit = Util::getSetting("geopotAltitudeUnit", "gpm").toString();
			if (unit == "gpdm" ||  unit == "dam")
				return 0.1;
			else if (unit == "gpft" ||  unit == "ft")
				return 1.0/0.3048;
			else
				return 1.0;
			break;
		default:
			return 1.0;
	}
	return 1.0;
}
//----------------------------------------------------------------
QString Util::formatGeopotAltitude (float meter, bool withUnit)
{
    QString unite = Util::getDataUnit (DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,0));
	double  coef  = Util::getDataCoef (DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,0));
    int d = (int) (meter*coef +0.5);
    QString r;
	r.sprintf("%d", d);
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatRain (float mmh, bool withUnit)
{
    QString unite = Util::getDataUnit (DataCode(GRB_PRECIP_TOT,LV_GND_SURF,0));
    QString r;
	if (mmh<10)
		r.sprintf("%.2f", mmh);
	else
		r.sprintf("%.1f", mmh);
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatPressure (float pasc, bool withUnit, int precision)
{
    QString unite = Util::getDataUnit (DataCode(GRB_PRESSURE_MSL,LV_MSL,0));
    QString r;
	if (pasc != GRIB_NOTDEF) {
		if (precision > 0)
			r.sprintf("%.1f", pasc/100.0);
		else
			r.sprintf("%.0f", pasc/100.0);
	}
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatCAPEsfc (float jkg, bool withUnit)
{
    QString unite = Util::getDataUnit (DataCode(GRB_CAPE,LV_GND_SURF,0));
    QString r;
	r.sprintf("%d", qRound(jkg));
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatWaveHeight (float meter, bool withUnit)
{
    QString unit = Util::getSetting("waveHeightUnit", tr("m")).toString();
    QString r, unite;
    float d;
    if (unit == tr("m")) {
		unite = unit;
		d = meter;
    }
    else {
        unite = "ft";
        d= meter/0.3048;
    }
	if (d < 10)
		r.sprintf("%.1f", d);
	else
		r.sprintf("%.0f", d);
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatWavePeriod (float second, bool withUnit)
{
    QString unit = Util::getSetting("waveHeightPeriod", tr("s")).toString();
    QString r;
	r.sprintf("%.0f", second);
	return (withUnit) ? r+" "+unit : r;
}
//----------------------------------------------------------------
QString Util::formatWaveDirection (float angle, bool withUnit)
{
	return formatDirection (angle, withUnit);
}
//----------------------------------------------------------------
QString Util::formatWhiteCap (float prb, bool withUnit)
{
    QString unite = "%";
    QString r;
	float v = inRange (0.0f, prb, 100.0f);
	if (v < 10)
		r.sprintf("%.1f", v);
	else
		r.sprintf("%.0f", v);
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatSnowDepth (float meter, bool withUnit)
{
    QString unit = Util::getSetting("snowDepthUnit", tr("m")).toString();
    QString r, unite;
    float d;
    if (unit == tr("m")) {
        unite = "cm";
        d= meter*100.0;
        if (d < 10)
			r.sprintf("%.1f", d);
		else
			r.sprintf("%.0f", d);
    }
    else {
        unite = "ft";
        d= meter/0.3048;
		r.sprintf("%.2f", d);
    }
	return (withUnit) ? r+" "+unite : r;
}
//----------------------------------------------------------------
QString Util::formatDegres (float x, bool inf100)     // 123.4 -> 123°24.00'
{
	const char *cdeg = "°";
    QString tunit = Util::getSetting("unitsPosition", "").toString();
    QString unit = (tunit=="") ? tr("dd°mm'ss\"") : tunit;
    
    QString r;
    if (unit == tr("dd°mm,mm'"))
    {
        int deg = (int) fabs(x);
        float min = (fabs(x) - deg)*60.0;
        char sign = (x<0) ? '-' : ' ';
        if (inf100)
        	r.sprintf("%c%02d%s%05.2f'", sign,deg,cdeg, min);
		else
        	r.sprintf("%c%03d%s%05.2f'", sign,deg,cdeg, min);
    }
    else if (unit == tr("dd°mm'ss\""))
    {
        int sec = (int) fabs(x*3600.0);  // total en secondes
        int min = sec / 60;              // nombre entier de minutes
        int deg = min / 60;              // nombre entier de degrés
        min = min % 60;                  // reste en minutes
        sec = sec % 60;                  // reste en secondes
        char sign = (x<0) ? '-' : ' ';
        if (inf100)
	        r.sprintf("%c%02d%s%02d'%02d\"", sign,deg,cdeg, min,sec);
		else
	        r.sprintf("%c%03d%s%02d'%02d\"", sign,deg,cdeg, min,sec);
    }
    else // if (unit == tr("dd,dd°"))
    {
        if (inf100)
    	    r.sprintf("%05.2f%s",x,cdeg);
		else
    	    r.sprintf("%06.2f%s",x,cdeg);
    }
    return r;
}
//---------------------------------------------------------------------
QString Util::formatPosition(float x, float y)  // 123°24.00'W 45°67.89'N
{
    if ( Util::getSetting("orderLatitudeLongitude", true).toBool() )
		return formatLatitude(y)+" "+formatLongitude(x);
	else
		return formatLongitude(x)+" "+formatLatitude(y);
}
//---------------------------------------------------------------------
QString Util::formatLongitude(float x)
{
    QString dir = Util::getSetting("longitudeDirection", "").toString();
	while (x > 360)
		x -= 360;
	while (x < -360)
		x += 360;
    if (dir == "West+") {
    	if (x <= 0)
	    	return formatDegres(-x, false)+"W";
		else
	    	return formatDegres(360-x, false)+"W";
	}
	else if (dir == "East+") {
    	if (x >= 0)
	    	return formatDegres(x, false)+"E";
		else
	    	return formatDegres(360+x, false)+"E";
	}
	else {
		// Mode automatique
		if (x > 0) {
			if (x <= 180)
    			return formatDegres(x, false)+"E";
			else
    			return formatDegres(360-x, false)+"W";
		}
		else {
			if (x >= -180)
	    		return formatDegres(-x, false)+"W";
			else
    			return formatDegres(x+360, false)+"E";
		}
	}
}
//---------------------------------------------------------------------
QString Util::formatLatitude(float y)
{
    QString dir = Util::getSetting("latitudeDirection", "").toString();
    if (dir == "South+")
	    return formatDegres(-y, true)+"S";
	else if (dir == "North+")
	    return formatDegres(y, true)+"N";
	else {
		// Mode automatique
		if (y > 0)
		    return formatDegres(y, true)+"N";
		else	
	    	return formatDegres(-y, true)+"S";
	}
}
//---------------------------------------------------------------------
QString Util::formatPercentValue(float v, bool withUnit)
{
	if (v == GRIB_NOTDEF)
		return withUnit ? "    %%": "   ";
    QString r;
    if (v<0)
        v=0;
    else if (v>100)
        v=100;    
	if (withUnit) {
		if (v < 10)
			r.sprintf("%.1f %%", v);
		else
			r.sprintf("%.0f %%", v);
	}
	else {
		if (v < 10)
			r.sprintf("%.1f", v);
		else
			r.sprintf("%.0f", v);
	}
    return r;
}

//======================================================================
QString Util::formatDayName (QDate dt)
{
	QString  sday;
	if (getSetting("appLanguage", "").toString() == "fr")
	{
		switch (dt.dayOfWeek()) {	// car la locale french ajoute un pt. inutile
			case 1: sday="Lun"; break;
			case 2: sday="Mar"; break;
			case 3: sday="Mer"; break;
			case 4: sday="Jeu"; break;
			case 5: sday="Ven"; break;
			case 6: sday="Sam"; break;
			case 7: sday="Dim"; break;
			default: sday=""; break;
		}
	}
    else {
		QLocale loc;
		sday = loc.toString(dt, "ddd");
	}
	return sday;
}
//------------------------------------------------
QString Util::formatDayName (time_t t)
{
    QDateTime dt = applyTimeZone(t);
	return formatDayName(dt.date());
}
//------------------------------------------------
int  Util::getDayNum (time_t t)
{
    QDateTime dt = applyTimeZone(t);
	return dt.date().dayOfWeek();
}
//------------------------------------------------
QString Util::formatDateLong (time_t t, bool localTime)
{
    QDateTime dt;
	if (localTime)
		dt = applyTimeZone(t);
	else
		dt.setTime_t(t);
	return formatDayName(dt.date())+" "+formatDateShort(t);
}
//---------------------------------------------------------------------
QString Util::formatDateShort (time_t t)
{
    QDateTime dt = applyTimeZone(t);
	if (getSetting("appLanguage", "").toString() == "fr")
	    return dt.toString("dd-MM-yyyy");
	else
	    return dt.toString("yyyy-MM-dd");
}
//---------------------------------------------------------------------
QString Util::formatDateTimeLong (time_t t)
{
	return formatDateLong(t)+" "+formatTime(t);
}
//---------------------------------------------------------------------
QString Util::formatDateTimeShort (time_t t)
{
	return formatDateShort(t)+" "+formatTime(t);
}
//---------------------------------------------------------------------
QString Util::formatTime (time_t t)
{
	QString suffix;
    QDateTime dt = applyTimeZone(t, &suffix);
    return dt.toString("hh:mm ")+suffix;
}
//---------------------------------------------------------------------
QDateTime Util::applyTimeZone (time_t t, QString *suffix)
{
    QDateTime dt;
    dt.setTime_t(t);
    //dt.setTimeSpec(Qt::UTC);
	dt = dt.toUTC();
	
	QString tmzone =  Util::getSetting("timeZone", "UTC").toString();
	if (tmzone == "LOC") {
		dt = dt.toLocalTime();
		if (suffix != NULL)
			*suffix = "LOC";
	}
	else if (tmzone.left(4)=="UTC+" || tmzone.left(4)=="UTC-")
	{    // UTC-12 UTC-11 ... UTC+1 UTC+2 UTC+3 ... UTC+14
		int dec = tmzone.mid(3,-1).toInt();
		if (dec==0 || dec<-12 || dec>14) {
			if (suffix != NULL)
				*suffix = "UTC";
		}
		else {
			dt = dt.addSecs(dec*3600);
			if (suffix != NULL)
				*suffix = tmzone;
		}
	}
	else
	{	// default timezone : UTC
		if (suffix != NULL)
			*suffix = "UTC";
	}
    
    return dt;
}
//-------------------------------------------------------------------------------
void Util::validArea (QNetworkRequest &request,double x0,double y0,double x1,double y1)
{
	int c1 = (int)floor(fabs((17.6*x0+89.23*x1-7.23*y0)*y1));
	int c2 = (int)ceil(fabs((37.12*x0-53.2*x1+7.23)*y0*y1));
	QString formatrange;
	QTextStream(&formatrange)<<c1<<"-"<<c2;
	request.setRawHeader ("Range", qPrintable(formatrange));
}
//============================================================================
double Util::distancePointSegment (double a,double b,     // point
								  double x0, double y0,  // segment start
								  double x1, double y1   // segment end
							)
{
	double dx, dy, t, dist;
	double hx, hy;

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx == 0 && dy == 0) {	//  the segment is a single point
		dx = a - x0;
		dy = b - y0;
		dist = sqrt(dx*dx + dy*dy);
	}
	else
	{
		// Projection on segment line
		t = ((a-x0)*dx + (b-y0)*dy) / (dx*dx + dy*dy);
		if (t < 0) {
			dx = a-x0;
			dy = b-y0;
			hx = x0;
			hy = y0;
		}
		else if (t > 1) {
			dx = a-x1;
			dy = b-y1;
			hx = x1;
			hy = y1;
		}
		else {
			hx = x0 + t*dx;
			hy = y0 + t*dy;
			dx = a-hx;
			dy = b-hy;
		}
		dist = sqrt(dx*dx + dy*dy);
	}
	return dist;
}
//----------------------------------------------------------------
double Util::roundMultiple (double x, double mul) 
{
	int sg;
	if (x < 0) {
		x = -x;
		sg = -1;
	}
	else {
		sg = 1;
	}
	return sg * ((int)floor(x/mul+0.5))*mul;
}
//----------------------------------------------------------------
time_t Util::timestampSetHour (time_t t, int hour, int min, int sec)
{
	struct tm dt;
	memcpy (&dt, gmtime(&t), sizeof(struct tm)); // no gmtime_r in windows mingw
	dt.tm_hour = hour;
	dt.tm_min = min;
	dt.tm_sec = sec;
	return mktime (&dt);
}
//------------------------------------
void Util::dumpchars (unsigned char *buf, size_t offset, size_t len)
{
	for (size_t i=0; i<len; i++) {
		unsigned char c = buf[i+offset];
		printf ("0x%02X %c\n", c, (c>=32?c:' '));
	}
}



