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

#include "Util.h"
#include "DataPointInfo.h"

#include <cmath>

static float polar_dir(float vx, float vy)
{
	float dir = -std::atan2(-vx, vy) *180.0f/M_PI + 180.f;
	if (dir < 0.f)
		return dir +360.f;
	if (dir >= 360.f)
		return dir -360.f;
	return dir;
}

DataPointInfo::DataPointInfo (GriddedReader *reader,
                    double x, double y, time_t  date )
{
	this->x = x;
	this->y = y;
	this->date = date;
	this->reader = reader;
	
	// Prefered temperature altitude : 2m. If not found try altitude 0m.
	temp = getValue(DataCode(GRB_TEMP,LV_ABOV_GND,2));
    if (GribDataIsDef(temp)) {
		tempAltitude = Altitude (LV_ABOV_GND,2);
	}
	else {
		temp = getValue(DataCode(GRB_TEMP,LV_GND_SURF,0));
        if (GribDataIsDef(temp))
            tempAltitude = Altitude (LV_GND_SURF,0);
		else
			tempAltitude = Altitude (LV_TYPE_NOT_DEFINED,0);
	}
	//-------------------------------------
	tempMin = getValue(DataCode(GRB_TMIN,LV_ABOV_GND,2));
	tempMax = getValue(DataCode(GRB_TMAX,LV_ABOV_GND,2));
	rain    = getValue(DataCode(GRB_PRECIP_TOT,LV_GND_SURF,0));
    if ( ! GribDataIsDef(rain )) // try for precipitation rate
        rain = getValue(DataCode(GRB_PRECIP_RATE,LV_GND_SURF,0));

    pressureMSL = getValue(DataCode(GRB_PRESSURE_MSL,LV_MSL,0));
	//----------------------------------------
	// Waves
	//----------------------------------------
	wave_sig_ht = getValue(DataCode(GRB_WAV_SIG_HT,LV_GND_SURF,0));
	wave_max_ht = getValue(DataCode(GRB_WAV_MAX_HT,LV_GND_SURF,0));
	wave_swl_ht = getValue(DataCode(GRB_WAV_SWL_HT,LV_GND_SURF,0));
	wave_wnd_ht = getValue(DataCode(GRB_WAV_WND_HT,LV_GND_SURF,0));

	wave_per     = getValue(DataCode(GRB_WAV_PER,LV_GND_SURF,0));
	wave_wnd_per = getValue(DataCode(GRB_WAV_WND_PER,LV_GND_SURF,0));
	wave_swl_per = getValue(DataCode(GRB_WAV_SWL_PER,LV_GND_SURF,0));
	wave_pr_per = getValue(DataCode(GRB_WAV_PRIM_PER,LV_GND_SURF,0));
	wave_scdy_per = getValue(DataCode(GRB_WAV_SCDY_PER,LV_GND_SURF,0));
	wave_max_per = getValue(DataCode(GRB_WAV_MAX_PER,LV_GND_SURF,0));

	wave_dir     = getValue(DataCode(GRB_WAV_DIR,LV_GND_SURF,0));
	wave_wnd_dir = getValue(DataCode(GRB_WAV_WND_DIR,LV_GND_SURF,0));
	wave_swl_dir = getValue(DataCode(GRB_WAV_SWL_DIR,LV_GND_SURF,0));
	wave_pr_dir = getValue(DataCode(GRB_WAV_PRIM_DIR,LV_GND_SURF,0));
	wave_scdy_dir = getValue(DataCode(GRB_WAV_SCDY_DIR,LV_GND_SURF,0));
	wave_max_dir = getValue(DataCode(GRB_WAV_MAX_DIR,LV_GND_SURF,0));
			
	wave_wcap_prbl = getValue(DataCode(GRB_WAV_WHITCAP_PROB,LV_GND_SURF,0));

	//----------------------------------------
	// Cloud : total cover
	//----------------------------------------
	// wgrib2 normalisation can return values < 0 or > 100 %
	cloudTotal = getPercentValue(DataCode(GRB_CLOUD_TOT,LV_ATMOS_ALL,0));

	//----------------------------------------
	// Cloud : layers
	//----------------------------------------
	cloudLow =  getPercentValue(DataCode(GRB_CLOUD_TOT,LV_CLOUD_LOW_LAYER,0));

	cloudMid =  getPercentValue(DataCode(GRB_CLOUD_TOT,LV_CLOUD_MID_LAYER,0));
	cloudHigh = getPercentValue(DataCode(GRB_CLOUD_TOT,LV_CLOUD_HIG_LAYER,0));
			
    hasCloudLayers = GribDataIsDef(cloudLow) || GribDataIsDef(cloudMid) || GribDataIsDef(cloudHigh);

    cloudLowTop = cloudLow<0.5f ? GRIB_NOTDEF
			: getPercentValue(DataCode(GRB_PRESSURE,LV_CLOUD_LOW_TOP,0));
    cloudMidTop = cloudMid<0.5f ? GRIB_NOTDEF
			: getPercentValue(DataCode(GRB_PRESSURE,LV_CLOUD_MID_TOP,0));
    cloudHighTop = cloudHigh<0.5f ? GRIB_NOTDEF
			: getPercentValue(DataCode(GRB_PRESSURE,LV_CLOUD_HIG_TOP,0));
	
    cloudLowBottom = cloudLow<0.5f ? GRIB_NOTDEF
			: getPercentValue(DataCode(GRB_PRESSURE,LV_CLOUD_LOW_BOTTOM,0));
    cloudMidBottom = cloudMid<0.5f ? GRIB_NOTDEF
            : getPercentValue (DataCode(GRB_PRESSURE,LV_CLOUD_MID_BOTTOM,0));
    cloudHighBottom = cloudHigh<0.5f ? GRIB_NOTDEF
			: getPercentValue (DataCode(GRB_PRESSURE,LV_CLOUD_HIG_BOTTOM,0));
	
	//----------------------------------------
	humidRel = getValue(DataCode(GRB_HUMID_REL,LV_ABOV_GND,2));
	humidSpec = getValue(DataCode(GRB_HUMID_SPEC,LV_ABOV_GND,2));
	dewPoint = getValue(DataCode(GRB_DEWPOINT,LV_ABOV_GND,2));
	isotherm0HGT = getValue(DataCode(GRB_GEOPOT_HGT,LV_ISOTHERM0,0));
	waterTemp    = getValue(DataCode(GRB_WTMP,LV_GND_SURF,0));
	snowDepth    = getValue(DataCode(GRB_SNOW_DEPTH,LV_GND_SURF,0));
	snowCateg    = getValue(DataCode(GRB_SNOW_CATEG,LV_GND_SURF,0));
	frzRainCateg = getValue(DataCode(GRB_FRZRAIN_CATEG,LV_GND_SURF,0));
	CAPEsfc = getValue(DataCode(GRB_CAPE,LV_GND_SURF,0));
    CINsfc = getValue(DataCode(GRB_CIN,LV_GND_SURF,0));
    // added by david
    compReflect = getValue(DataCode(GRB_COMP_REFL,LV_ATMOS_ALL,0));
    GUSTsfc = getValue(DataCode(GRB_WIND_GUST,LV_GND_SURF,0));
    // added by david to enable gust fields both sfc and at 10m assuming that only one
    // option exists in grib file
    if (! GribDataIsDef( GUSTsfc )) // gust at sfc not found - try at 10m
        GUSTsfc = getValue(DataCode(GRB_WIND_GUST,LV_ABOV_GND,10));
    //-----------------------------------------
	// Wind 10m
	//-----------------------------------------
	vx_10m = getValue(DataCode(GRB_WIND_VX,LV_ABOV_GND,10));
	vy_10m = getValue(DataCode(GRB_WIND_VY,LV_ABOV_GND,10));
    if (GribDataIsDef(vx_10m) && GribDataIsDef(vy_10m)) {
		windSpeed_10m = std::sqrt (vx_10m*vx_10m + vy_10m*vy_10m);
		windDir_10m = polar_dir(vx_10m, vy_10m);
	}
	else {
		windSpeed_10m = getValue(DataCode(GRB_WIND_SPEED,LV_ABOV_GND,10));
		windDir_10m = getValue(DataCode(GRB_WIND_DIR,LV_ABOV_GND,10));
		if (GribDataIsDef(windSpeed_10m) && GribDataIsDef(windDir_10m)) {
			double ang = windDir_10m/180.0*M_PI;
			double si= windSpeed_10m*sin(ang),  co= windSpeed_10m*cos(ang);
			vx_10m = -si;
			vy_10m = -co;
		}
	}
	//-----------------------------------------
	// Wind surface
	//-----------------------------------------
	vx_gnd = getValue(DataCode(GRB_WIND_VX,LV_GND_SURF,0));
	vy_gnd = getValue(DataCode(GRB_WIND_VY,LV_GND_SURF,0));
    if (GribDataIsDef(vx_gnd) && GribDataIsDef(vy_gnd)) {
		windSpeed_gnd = std::sqrt (vx_gnd*vx_gnd + vy_gnd*vy_gnd);
		windDir_gnd = polar_dir(vx_gnd, vy_gnd);
	}
	else {
		windSpeed_gnd = getValue(DataCode(GRB_WIND_SPEED,LV_GND_SURF,0));
		windDir_gnd = getValue(DataCode(GRB_WIND_DIR,LV_GND_SURF,0));
		if (GribDataIsDef(windSpeed_gnd) && GribDataIsDef(windDir_gnd)) {
			double ang = windDir_gnd/180.0*M_PI;
			double si= windSpeed_gnd*sin(ang),  co= windSpeed_gnd*cos(ang);
			vx_gnd = -si;
			vy_gnd = -co;
		}
	}
	//-----------------------------------------
    // Current
	//-----------------------------------------
    cx = getValue(DataCode(GRB_CUR_VX,LV_GND_SURF,0));
    if (!GribDataIsDef(cx)) // if not at surface try at -1m
        cx = getValue(DataCode(GRB_CUR_VX,LV_BLW_SURF,1));
    if (!GribDataIsDef(cx)) // if not at surface try at -2m
        cx = getValue(DataCode(GRB_CUR_VX,LV_BLW_SURF,2));
    if (!GribDataIsDef(cx)) // if not at surface try at -3m
        cx = getValue(DataCode(GRB_CUR_VX,LV_BLW_SURF,3));

    cy = getValue(DataCode(GRB_CUR_VY,LV_GND_SURF,0));
    if (!GribDataIsDef(cy)) // if not at surface try at -1m
        cy = getValue(DataCode(GRB_CUR_VY,LV_BLW_SURF,1));
    if (!GribDataIsDef(cy)) // if not at surface try at -2m
        cy = getValue(DataCode(GRB_CUR_VY,LV_BLW_SURF,2));
    if (!GribDataIsDef(cy)) // if not at surface try at -3m
        cy = getValue(DataCode(GRB_CUR_VY,LV_BLW_SURF,3));

    if (GribDataIsDef(cx) && GribDataIsDef(cy)) {
		currentSpeed = std::sqrt (cx*cx + cy*cy);
		// unlike wind current direction is to X not from X
		currentDir = polar_dir(-cx, -cy);
	}
	else {
		currentSpeed = getValue(DataCode(GRB_CUR_SPEED,LV_GND_SURF,0));
		currentDir = getValue(DataCode(GRB_CUR_DIR,LV_GND_SURF,0));;
		if (GribDataIsDef(currentSpeed) && GribDataIsDef(currentDir)) {
			double ang = currentDir/180.0*M_PI;
			double si= currentSpeed*sin(ang),  co= currentSpeed*cos(ang);
			cx = -si;
			cy = -co;
		}
		if (GribDataIsDef(currentDir)) {
			// XXX I convert UV this way but what about other?
			currentDir -= 180.;
			if (currentDir < 0.)
				currentDir += 360.;
			if (currentDir >= 360.)
				currentDir -=360.;
		}
	}
	//-------------------------------------------------------------
	// Data in altitude
	//-----------------------------------------
	if (reader && reader->hasAltitudeData())
	{
		double RH, T;
		int P, i;
		for (i=0; i<GEOPOTsize; i++)
		{
			P = GEOPOThgt(i);	// 925 850 700 600 500 400 300 200
			T = reader->getDateInterpolatedValue (DataCode(GRB_TEMP,LV_ISOBARIC,P), x,y,date);
			RH = reader->getDateInterpolatedValue (DataCode(GRB_HUMID_REL,LV_ISOBARIC,P), x,y,date);
			hThetae [i] = Therm::thetaEfromHR (T, P, RH);
			hTemp [i] = T;
			hGeopot [i] = reader->getDateInterpolatedValue (DataCode(GRB_GEOPOT_HGT,LV_ISOBARIC,P), x,y,date);
			hHumidRel [i]  = reader->getDateInterpolatedValue (DataCode(GRB_HUMID_REL,LV_ISOBARIC,P), x,y,date);
			hHumidSpec [i] = reader->getDateInterpolatedValue (DataCode(GRB_HUMID_SPEC,LV_ISOBARIC,P), x,y,date);

			hVx [i] = reader->getDateInterpolatedValue (DataCode(GRB_WIND_VX,LV_ISOBARIC,P), x,y,date);
			hVy [i] = reader->getDateInterpolatedValue (DataCode(GRB_WIND_VY,LV_ISOBARIC,P), x,y,date);
            if (GribDataIsDef(hVx[i]) && GribDataIsDef(hVy[i])) {
				hWindSpeed[i] = std::sqrt (hVx[i]*hVx[i] + hVy[i]*hVy[i]);
				hWindDir[i]   = -std::atan2 (-hVx[i], hVy[i]) *180.0/M_PI + 180;
				if (hWindDir[i] < 0)    hWindDir[i] += 360.0;
				if (hWindDir[i] >= 360) hWindDir[i] -= 360.0;
			}
			else {
				hWindSpeed[i] = GRIB_NOTDEF;
				hWindDir[i] = GRIB_NOTDEF;
			}
		}
		hasAltitudeData = true;
	}
	else {
		hasAltitudeData = false;
		for (int i=0; i<GEOPOTsize; i++) {
			hThetae [i] = GRIB_NOTDEF;
			hTemp [i] = GRIB_NOTDEF;
			hGeopot [i] = GRIB_NOTDEF;
			hHumidRel [i]  = GRIB_NOTDEF;
			hHumidSpec [i] = GRIB_NOTDEF;
			hVx [i] = GRIB_NOTDEF;
			hVy [i] = GRIB_NOTDEF;
			hWindSpeed[i] = GRIB_NOTDEF;
			hWindDir[i] = GRIB_NOTDEF;
		}
	}
	
}
//------------------------------------------------------------------
float DataPointInfo::getDataValue (const DataCode &dtc) const
{
	switch (dtc.dataType) {
		case GRB_CAPE 		  : 
			return CAPEsfc;
        case GRB_CIN 		  :
            return CINsfc;
        // added by david
        case GRB_COMP_REFL 		  :
            return compReflect;
        case GRB_WIND_GUST	  :
			return GUSTsfc;
		case GRB_SNOW_DEPTH   : 
			return snowDepth;
		case GRB_PRESSURE_MSL : 
			return pressureMSL;
		case GRB_PRECIP_TOT   : 
        case GRB_PRECIP_RATE  :
            return rain;
		case GRB_CLOUD_TOT    : 
			return cloudTotal;
		case GRB_SNOW_CATEG   : 
			return snowCateg;
		case GRB_FRZRAIN_CATEG: 
			return frzRainCateg;
		case GRB_TMAX         : 
			return tempMax;
		case GRB_TMIN         : 
			return tempMin;
		case GRB_DEWPOINT     : 
			return dewPoint;
		case GRB_PRV_DIFF_TEMPDEW : 
			return 
				hasDewPoint()&&hasTemp() ? temp-dewPoint : GRIB_NOTDEF;
			
		case GRB_WTMP         :
			return waterTemp;
		case GRB_TEMP         : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC) {
				int idx = dtc.getAltitude().index();
				assert (idx >= 0);
				return hTemp [idx];
			}
			else
				return temp;
		case GRB_GEOPOT_HGT   :
			if (dtc.levelType == LV_ISOTHERM0)
				return isotherm0HGT;
			else if (dtc.getAltitude().levelType == LV_ISOBARIC) {
				int idx = dtc.getAltitude().index();
				assert (idx >= 0);
				return hGeopot [idx];
			}
			return GRIB_NOTDEF;
		case GRB_HUMID_REL    : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC) {
				int idx = dtc.getAltitude().index();
				assert (idx >= 0);
				return hHumidRel [idx];
			}
			else
				return humidRel;
		case GRB_HUMID_SPEC   : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC) {
				int idx = dtc.getAltitude().index();
				assert (idx >= 0);
				return hHumidSpec [idx];
			}
			else
				return humidSpec;
		case GRB_PRV_THETA_E      : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC) {
				int idx = dtc.getAltitude().index();
				assert (idx >= 0);
				return hThetae [idx];
			}
			else
				return GRIB_NOTDEF;
		//-----------------------------------
		// Waves
		//-----------------------------------
		case GRB_WAV_SIG_HT:
				return wave_sig_ht;
		case GRB_WAV_DIR:
				return wave_dir;
		case GRB_WAV_WND_DIR:
				return wave_wnd_dir;
		case GRB_WAV_WND_HT:
				return wave_wnd_ht;
		case GRB_WAV_PER:
				return wave_per;
		case GRB_WAV_WND_PER:
				return wave_wnd_per;
		case GRB_WAV_SWL_DIR:
				return wave_swl_dir;
		case GRB_WAV_SWL_HT:
				return wave_swl_ht;
		case GRB_WAV_SWL_PER:
				return wave_swl_per;
		case GRB_WAV_PRIM_DIR:
				return wave_pr_dir;
		case GRB_WAV_PRIM_PER:
				return wave_pr_per;
		case GRB_WAV_SCDY_DIR:
				return wave_scdy_dir;
		case GRB_WAV_SCDY_PER:
				return wave_scdy_per;
		case GRB_WAV_WHITCAP_PROB:
				return wave_wcap_prbl;
		case GRB_WAV_MAX_DIR:
				return wave_max_dir;
		case GRB_WAV_MAX_PER:
				return wave_max_per;
		case GRB_WAV_MAX_HT:
				return wave_max_ht;
		//-----------------------------------
		case GRB_CUR_VX      : 
		case GRB_CUR_VY      : 
		case GRB_PRV_CUR_XY2D    : 
		case GRB_PRV_CUR_DIR     : 
		case GRB_WIND_VX      : 
		case GRB_WIND_VY      : 
		case GRB_PRV_WIND_XY2D    : 
		case GRB_PRV_WIND_DIR     : 
		default:
			return GRIB_NOTDEF;
	}
}
//--------------------------------------------------------
bool DataPointInfo::getWaveValues (int prvtype, 
							float *ht, float *per, float *dir)  const
{	
	switch (prvtype) {
		case GRB_PRV_WAV_SIG:
			*ht  = getWaveData (GRB_WAV_SIG_HT);
			*dir = getWaveData (GRB_WAV_DIR);
			*per = getWaveData (GRB_WAV_PER);
			break;
		case GRB_PRV_WAV_MAX:
			*ht  = getWaveData (GRB_WAV_MAX_HT);
			*dir = getWaveData (GRB_WAV_MAX_DIR);
			*per = getWaveData (GRB_WAV_MAX_PER);
			break;
		case GRB_PRV_WAV_WND:
			*ht  = getWaveData (GRB_WAV_WND_HT);
			*dir = getWaveData (GRB_WAV_WND_DIR);
			*per = getWaveData (GRB_WAV_WND_PER);
			break;
		case GRB_PRV_WAV_SWL:
			*ht  = getWaveData (GRB_WAV_SWL_HT);
			*dir = getWaveData (GRB_WAV_SWL_DIR);
			*per = getWaveData (GRB_WAV_SWL_PER);
			break;
		case GRB_PRV_WAV_PRIM:
			*ht  = GRIB_NOTDEF;
			*dir = getWaveData (GRB_WAV_PRIM_DIR);
			*per = getWaveData (GRB_WAV_PRIM_PER);
			break;
		case GRB_PRV_WAV_SCDY:
			*ht  = GRIB_NOTDEF;
			*dir = getWaveData (GRB_WAV_SCDY_DIR);
			*per = getWaveData (GRB_WAV_SCDY_PER);
			break;
		default:
			*ht  = GRIB_NOTDEF;
			*dir = GRIB_NOTDEF;
			*per = GRIB_NOTDEF;
			return false;
	}
    return GribDataIsDef(*ht) || GribDataIsDef(*dir) || GribDataIsDef(*per);
}
//--------------------------------------------------------
void DataPointInfo::getWaveWxWy (int prvtype, float *wx, float *wy)  const
{
	float ht;
	float per;
	float dir;

	*wx = GRIB_NOTDEF;
	*wy = GRIB_NOTDEF;

	if (!getWaveValues (prvtype, &ht, &per, &dir))
		return;
	if (!GribDataIsDef(dir) || !GribDataIsDef(per))
		return;

	double ang = dir/180.0*M_PI;
	double si= per*sin(ang),  co= per*cos(ang);
	*wx = -si;
	*wy = -co;
}
//--------------------------------------------------------
float DataPointInfo::getWaveData (int type)  const
{	
	return getDataValue (DataCode(type,LV_GND_SURF,0));
}
//--------------------------------------------------------
bool DataPointInfo::hasWaveData (int type)  const
{	
    return GribDataIsDef(getWaveData (type));
}
//--------------------------------------------------------
bool DataPointInfo::hasWind (const Altitude &alt)  const
{	
	if (alt.levelType == LV_ISOBARIC) {
		int i = alt.index();
        return i>=0 && GribDataIsDef(hVx[i]) && GribDataIsDef(hVy[i]);
	} 
	else if (alt.levelType == LV_GND_SURF) {
        return GribDataIsDef(windDir_gnd) && GribDataIsDef(windSpeed_gnd);
	} 
	else
        return GribDataIsDef(windDir_10m) && GribDataIsDef(windSpeed_10m);
}
//--------------------------------------------------------
bool DataPointInfo::hasCurrent ()  const
{	
    return GribDataIsDef(currentDir) && GribDataIsDef(currentSpeed);
}
//--------------------------------------------------------
bool DataPointInfo::getWindValues 
			(const Altitude &alt, float *speed, float*dir) const
{	
	if (! hasWind (alt)) {
		*speed = GRIB_NOTDEF;
		*dir = GRIB_NOTDEF;
		return false;
	}
	if (alt.levelType == LV_ISOBARIC) {
		int i = alt.index();
		assert(i != -1);
		*speed = hWindSpeed [i];
		*dir = hWindDir [i];
	} 
	else if (alt.levelType == LV_GND_SURF) {
		*speed = windSpeed_gnd;
		*dir = windDir_gnd;
	} 
	else {
		*speed = windSpeed_10m;
		*dir = windDir_10m;
	}
	return true;
}
//--------------------------------------------------------
bool DataPointInfo::getCurrentValues 
			(float *speed, float*dir) const
{	
	if (! hasCurrent ()) {
		*speed = GRIB_NOTDEF;
		*dir = GRIB_NOTDEF;
		return false;
	}
	
		*speed = currentSpeed;
		*dir = currentDir;
	
	return true;
}
//--------------------------------------------------------
void  DataPointInfo::getWindVxVy (const Altitude &alt, float *vx, float*vy) const
{	
	if (! hasWind (alt)) {
		*vx = GRIB_NOTDEF;
		*vy = GRIB_NOTDEF;
		return;
	}
	if (alt.levelType == LV_ISOBARIC) {
		int i = alt.index();
		assert(i != -1);

		*vx = hVx [i];
		*vy = hVy [i];
	} 
	else if (alt.levelType == LV_GND_SURF) {
		*vx = vx_gnd;
		*vy = vy_gnd;
	} 
	else {
		*vx = vx_10m;
		*vy = vy_10m;
	}
}
//--------------------------------------------------------
void  DataPointInfo::getCurrentCxCy (const Altitude &alt, float *vx, float*vy) const
{	
	if (! hasCurrent ()) {
		*vx = GRIB_NOTDEF;
		*vy = GRIB_NOTDEF;
		return;
	}
    if (alt.levelType == LV_GND_SURF) {
		*vx = cx;
		*vy = cy;
	} 
}



