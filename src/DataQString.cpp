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

#include "DataQString.h"

//-------------------------------------------------
QString DataCodeStr::toString (const DataCenterModel &dcm)
{
	switch (dcm) {
		case NOAA_GFS :
			return "NOAA-GFS";
		case NOAA_NCEP_WW3 :
			return "NOAA-NCEP-WW3";
		case METEOBLUE_NMM4 :
			return "METEOBLUE-NMM4";
		case FNMOC_WW3_GLB :
			return "FNMOC-WW3-GLOBAL";
		case FNMOC_WW3_MED :
			return "FNMOC-WW3-MEDIT";
		case NORWAY_METNO :
			return "NORWAY-MET";
		case OTHER_DATA_CENTER :
		default:
			return "";
	}
}
//-------------------------------------------------
QString AltitudeStr::toString (const Altitude &alt)
{
	QString res;
	switch (alt.levelType) {
		case  LV_GND_SURF:
			res = tr("surface");
			break;
		case  LV_ISOTHERM0:
			res = tr("isotherm 0°C");
			break;
		case  LV_ISOBARIC:
		case  LV_CLOUD_LOW_BOTTOM:
		case  LV_CLOUD_MID_BOTTOM:
		case  LV_CLOUD_HIG_BOTTOM:
		case  LV_CLOUD_LOW_TOP:
		case  LV_CLOUD_MID_TOP:
		case  LV_CLOUD_HIG_TOP:
			res = tr("%1 hPa").arg(alt.levelValue);
			break;
		case  LV_MSL :
			res = tr("sea level");
			break;
		case  LV_ABOV_MSL:
			res = tr("%1 m above mean sea level").arg(alt.levelValue);
			break;
		case  LV_ABOV_GND:
			res = tr("%1 m above ground").arg(alt.levelValue);
			break;
		case  LV_SIGMA:
			res = tr("sigma %1").arg((int)(alt.levelValue/10));
			break;
		case  LV_ATMOS_ALL:
			res = tr("atmosphere");
			break;
		case LV_CLOUD_LOW_LAYER :
			res = tr("low cloud layer");
			break;
		case LV_CLOUD_MID_LAYER :
			res = tr("middle cloud layer");
			break;
		case LV_CLOUD_HIG_LAYER :
			res = tr("high cloud layer");
			break;
		default:
			res =  "";
	}
	return res;
}
//-------------------------------------------------
QString AltitudeStr::toStringShort (const Altitude &alt)
{
	QString res;
	switch (alt.levelType) {
		case  LV_GND_SURF:
			res = tr("sfc");
			break;
		case  LV_ISOTHERM0:
			res = tr("iso0°C");
			break;
		case  LV_ISOBARIC:
		case  LV_CLOUD_LOW_BOTTOM:
		case  LV_CLOUD_MID_BOTTOM:
		case  LV_CLOUD_HIG_BOTTOM:
		case  LV_CLOUD_LOW_TOP:
		case  LV_CLOUD_MID_TOP:
		case  LV_CLOUD_HIG_TOP:
			res = tr("%1hpa").arg(alt.levelValue);
			break;
		case  LV_MSL :
			res = tr("MSL");
			break;
		case  LV_ABOV_MSL:
			if (alt.levelValue == 0)
				res = tr("MSL");
			else
				res = tr("%1 m ab.MSL").arg(alt.levelValue);
			break;
		case  LV_ABOV_GND:
			if (alt.levelValue == 0)
				res = tr("sfc");
			else
				res = tr("%1 m").arg(alt.levelValue);
			break;
		case  LV_SIGMA:
			res = tr("sigma%1").arg((int)(alt.levelValue/10));
			break;
		case  LV_ATMOS_ALL:
			res = tr("atm");
			break;
		case LV_CLOUD_LOW_LAYER :
			res = tr("lowCldLay");
			break;
		case LV_CLOUD_MID_LAYER :
			res = tr("midCldLay");
			break;
		case LV_CLOUD_HIG_LAYER :
			res = tr("higCldLay");
			break;
		default:
			//DBG("alt.levelType=%d", alt.levelType);
			res = "";
	}
	return res;
}
//-------------------------------------------------
QString DataCodeStr::toString_name (int dataType)
{
	return toString_name (DataCode(dataType,LV_GND_SURF,0));
}
//-------------------------------------------------
QString DataCodeStr::toString_name (const DataCode &dtc)
{
	switch (dtc.dataType) {
		case GRB_PRESSURE     : return tr("Pressure");
		case GRB_PRESSURE_MSL : return tr("Pressure")+" "+tr("MSL");
		case GRB_GEOPOT_HGT   : return tr("Geopotential");
		case GRB_TEMP         : return tr("Temperature");
		case GRB_TEMP_POT     : return tr("Potential temperature");
		case GRB_TMAX         : return tr("Temperature max");
		case GRB_TMIN         : return tr("Temperature min");
		case GRB_DEWPOINT     : return tr("Dew point");
		case GRB_CUR_VX      : return tr("Current (Vx)");
		case GRB_CUR_VY      : return tr("Current (Vy)");
		case GRB_WIND_VX      : return tr("Wind (Vx)");
		case GRB_WIND_VY      : return tr("Wind (Vy)");
		case GRB_WIND_DIR     : return tr("Wind (direction)");
		case GRB_WIND_SPEED   : return tr("Wind (speed)");
		case GRB_HUMID_SPEC   : return tr("Specific humidity");
		case GRB_HUMID_REL    : return tr("Relative humidity");
		case GRB_PRECIP_RATE  : return tr("Precipitation rate");
		case GRB_PRECIP_TOT   : return tr("Total precipitation");
		case GRB_SNOW_DEPTH   : return tr("Snow (depth)");
		case GRB_CLOUD_TOT    : return tr("Cloud cover");
		case GRB_CLOUD_LOW    : return tr("Cloud cover (low)");
		case GRB_CLOUD_MID    : return tr("Cloud cover (middle)");
		case GRB_CLOUD_HIG    : return tr("Cloud cover (high)");
		case GRB_FRZRAIN_CATEG: return tr("Frozen rain possible");
		case GRB_SNOW_CATEG   : return tr("Snowfall possible");
		case GRB_CAPE 		  : return tr("CAPE");
		case GRB_CIN 		  : return tr("CIN");

		case GRB_PRV_WIND_XY2D    : return tr("Wind");
		case GRB_PRV_WIND_JET     : return tr("Wind (jet stream)");
		case GRB_PRV_DIFF_TEMPDEW : return tr("Gap temperature-dew point");
		case GRB_PRV_THETA_E      : return tr("Theta-e");
		case GRB_WIND_GUST    : return tr("Wind gust");
		case GRB_WIND_GUST_VX : return tr("Wind gust (Vx)");
		case GRB_WIND_GUST_VY : return tr("Wind gust (VY)");
		case GRB_PRV_CUR_DIR    : return tr("Current direction");
		case GRB_PRV_CUR_XY2D   : return tr("Current");
		
		case GRB_WAV_SIG_HT	   : return tr("Significant wave");
		case GRB_WAV_WND_DIR   : return tr("Wind wave direction");
		case GRB_WAV_WND_HT    : return tr("Wind wave height");
		case GRB_WAV_WND_PER   : return tr("Wind wave period");
		case GRB_WAV_SWL_DIR   : return tr("Swell wave direction");
		case GRB_WAV_SWL_HT    : return tr("Swell wave height");
		case GRB_WAV_SWL_PER   : return tr("Swell wave period");
		case GRB_WAV_PRIM_DIR    : return tr("Primary wave direction");
		case GRB_WAV_PRIM_PER    : return tr("Primary wave period");
		case GRB_WAV_SCDY_DIR  : return tr("Secondary wave direction");
		case GRB_WAV_SCDY_PER  : return tr("Secondary wave period");
		case GRB_WAV_MAX_DIR   : return tr("Maximum wave direction");
		case GRB_WAV_MAX_HT    : return tr("Maximum wave height");
		case GRB_WAV_MAX_PER   : return tr("Maximum wave period");
		case GRB_WAV_WHITCAP_PROB  : return tr("Whitecap probability");
		
		case GRB_PRV_WAV_SIG   : return tr("Significant wave");
		case GRB_PRV_WAV_MAX   : return tr("Maximum wave");
		case GRB_PRV_WAV_SWL   : return tr("Swell");
		case GRB_PRV_WAV_WND   : return tr("Wind wave");
		case GRB_PRV_WAV_PRIM   : return tr("Primary wave");
		case GRB_PRV_WAV_SCDY   : return tr("Secondary wave");
		
		default:
			DBG("Unknown data type: %d", dtc.dataType);
			return  "";
	}
}
//-------------------------------------------------
QString DataCodeStr::toString_level (const DataCode &dtc)
{
	return AltitudeStr::toString (dtc.getAltitude());
}
//-------------------------------------------------
QString DataCodeStr::toString (const DataCode &dtc)
{
	QString name = toString_name (dtc);
	QString level = toString_level (dtc);
	if (level != "")
			return name+" (" +level+ ")";
	else
			return name;
}
//-------------------------------------------------
QString DataCodeStr::toString_levelShort (const DataCode &dtc)
{
	QString name = toString_name (dtc);
	QString level = AltitudeStr::toStringShort (dtc.getAltitude());
	if (level != "")
			return name+" (" +level+ ")";
	else
			return name;
}

//-------------------------------------------------
QString AltitudeStr::serialize (const Altitude &alt)
{
	return QString("%1;%2").arg(alt.levelType).arg(alt.levelValue);
}
//-------------------------------------------------
Altitude AltitudeStr::unserialize (const QString &stringCode)
{
	Altitude alt;
	QStringList list = stringCode.split(";");
	if (list.size() == 2) {
		bool ok;
		alt.levelType = list.at(0).toInt (&ok);
		if (!ok) alt.levelType = LV_TYPE_NOT_DEFINED;
		alt.levelValue = list.at(1).toInt (&ok);
		if (!ok) alt.levelType = LV_TYPE_NOT_DEFINED;
	}
	else {
		alt.levelType = LV_TYPE_NOT_DEFINED;
	}
	return alt;
}

//-------------------------------------------------
QString DataCodeStr::serialize (const DataCode &dtc)
{
	return QString("%1;%2;%3").arg(dtc.dataType).arg(dtc.levelType).arg(dtc.levelValue);
}
//-------------------------------------------------
DataCode DataCodeStr::unserialize (const QString &stringCode)
{
	DataCode dtc;
	QStringList list = stringCode.split(";");
	if (list.size() == 3) {
		bool ok;
		dtc.dataType = list.at(0).toInt (&ok);
		if (!ok) dtc.dataType = GRB_TYPE_NOT_DEFINED;
		dtc.levelType = list.at(1).toInt (&ok);
		if (!ok) dtc.dataType = GRB_TYPE_NOT_DEFINED;
		dtc.levelValue = list.at(2).toInt (&ok);
		if (!ok) dtc.dataType = GRB_TYPE_NOT_DEFINED;
	}
	else {
		dtc.dataType = GRB_TYPE_NOT_DEFINED;
	}
	return dtc;
}

