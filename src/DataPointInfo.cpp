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

#include "Util.h"
#include "DataPointInfo.h"

DataPointInfo::DataPointInfo ( 
					GriddedReader *reader, 
					float x, float y, time_t  date )
{
	this->x = x;
	this->y = y;
	this->date = date;
	this->reader = reader;
	
	// Prefered temperature altitude : 2m. If not found try altitude 0m.
	temp = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_TEMP,LV_ABOV_GND,2), x,y,date);
	if (temp != GRIB_NOTDEF) {
		tempAltitude = Altitude (LV_ABOV_GND,2);
	}
	else {
		temp = reader==NULL ? GRIB_NOTDEF
				: reader->getDateInterpolatedValue (DataCode(GRB_TEMP,LV_GND_SURF,0), x,y,date);
		if (temp != GRIB_NOTDEF)
			tempAltitude = Altitude (LV_GND_SURF,0);
		else
			tempAltitude = Altitude (LV_TYPE_NOT_DEFINED,0);
	}
	//-------------------------------------
	tempMin = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_TMIN,LV_ABOV_GND,2), x,y,date);
	tempMax = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_TMAX,LV_ABOV_GND,2), x,y,date);
	rain    = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRECIP_TOT,LV_GND_SURF,0), x,y,date);
	pressureMSL = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE_MSL,LV_MSL,0), x,y,date);
	//----------------------------------------
	// Waves
	//----------------------------------------
	wave_sig_ht = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SIG_HT,LV_GND_SURF,0), x,y,date);
	wave_max_ht = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_MAX_HT,LV_GND_SURF,0), x,y,date);
	wave_swl_ht = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SWL_HT,LV_GND_SURF,0), x,y,date);
	wave_wnd_ht = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_WND_HT,LV_GND_SURF,0), x,y,date);

	wave_wnd_per = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_WND_PER,LV_GND_SURF,0), x,y,date);
	wave_swl_per = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SWL_PER,LV_GND_SURF,0), x,y,date);
	wave_pr_per = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_PRIM_PER,LV_GND_SURF,0), x,y,date);
	wave_scdy_per = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SCDY_PER,LV_GND_SURF,0), x,y,date);
	wave_max_per = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_MAX_PER,LV_GND_SURF,0), x,y,date);

	wave_wnd_dir = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_WND_DIR,LV_GND_SURF,0), x,y,date);
	wave_swl_dir = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SWL_DIR,LV_GND_SURF,0), x,y,date);
	wave_pr_dir = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_PRIM_DIR,LV_GND_SURF,0), x,y,date);
	wave_scdy_dir = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_SCDY_DIR,LV_GND_SURF,0), x,y,date);
	wave_max_dir = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_MAX_DIR,LV_GND_SURF,0), x,y,date);
			
	wave_wcap_prbl = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WAV_WHITCAP_PROB,LV_GND_SURF,0), x,y,date);

	//----------------------------------------
	// Cloud : total cover
	//----------------------------------------
	cloudTotal = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CLOUD_TOT,LV_ATMOS_ALL,0), x,y,date);
	//----------------------------------------
	// Cloud : layers
	//----------------------------------------
	cloudLow = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_LOW_LAYER,0), x,y,date);
	cloudMid = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_MID_LAYER,0), x,y,date);
	cloudHigh = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_HIG_LAYER,0), x,y,date);
			
	hasCloudLayers = (cloudLow!=GRIB_NOTDEF) || (cloudMid!=GRIB_NOTDEF) || (cloudHigh!=GRIB_NOTDEF);

	cloudLowTop = cloudLow<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_LOW_TOP,0), x,y,date);
	cloudMidTop = cloudMid<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_MID_TOP,0), x,y,date);
	cloudHighTop = cloudHigh<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_HIG_TOP,0), x,y,date);
	
	cloudLowBottom = cloudLow<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_LOW_BOTTOM,0), x,y,date);
	cloudMidBottom = cloudMid<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_MID_BOTTOM,0), x,y,date);
	cloudHighBottom = cloudHigh<0.5 || reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_PRESSURE,LV_CLOUD_HIG_BOTTOM,0), x,y,date);
	
	//----------------------------------------
	humidRel = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_HUMID_REL,LV_ABOV_GND,2), x,y,date);
	humidSpec = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_HUMID_SPEC,LV_ABOV_GND,2), x,y,date);
	dewPoint = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_DEWPOINT,LV_ABOV_GND,2), x,y,date);
	isotherm0HGT = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_GEOPOT_HGT,LV_ISOTHERM0,0), x,y,date);
	snowDepth    = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_SNOW_DEPTH,LV_GND_SURF,0), x,y,date);
	snowCateg    = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_SNOW_CATEG,LV_GND_SURF,0), x,y,date);
	frzRainCateg = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_FRZRAIN_CATEG,LV_GND_SURF,0), x,y,date);
	CAPEsfc = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CAPE,LV_GND_SURF,0), x,y,date);
	CINsfc = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CIN,LV_GND_SURF,0), x,y,date);
	GUSTsfc = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WIND_GUST,LV_GND_SURF,0), x,y,date);
	//-----------------------------------------
	// Wind 10m
	//-----------------------------------------
	vx_10m = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WIND_VX,LV_ABOV_GND,10), x,y,date);
	vy_10m = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WIND_VY,LV_ABOV_GND,10), x,y,date);
	if (vx_10m!=GRIB_NOTDEF && vy_10m!=GRIB_NOTDEF) {
		windSpeed_10m = sqrt (vx_10m*vx_10m + vy_10m*vy_10m);
		windDir_10m = - atan2 (-vx_10m, vy_10m) *180.0/M_PI + 180;
		if (windDir_10m < 0)    windDir_10m += 360.0;
		else if (windDir_10m >= 360) windDir_10m -= 360.0;
	}
	else {
		windSpeed_10m = GRIB_NOTDEF;
		windDir_10m = GRIB_NOTDEF;
	}
	//-----------------------------------------
	// Wind surface
	//-----------------------------------------
	vx_gnd = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WIND_VX,LV_GND_SURF,0), x,y,date);
	vy_gnd = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_WIND_VY,LV_GND_SURF,0), x,y,date);
	if (vx_gnd!=GRIB_NOTDEF && vy_gnd!=GRIB_NOTDEF) {
		windSpeed_gnd = sqrt (vx_gnd*vx_gnd + vy_gnd*vy_gnd);
		windDir_gnd = - atan2 (-vx_gnd, vy_gnd) *180.0/M_PI + 180;
		if (windDir_gnd < 0)    windDir_gnd += 360.0;
		else if (windDir_gnd >= 360) windDir_gnd -= 360.0;
	}
	else {
		windSpeed_gnd = GRIB_NOTDEF;
		windDir_gnd = GRIB_NOTDEF;
	}
	//-----------------------------------------
	// Current surface
	//-----------------------------------------
	cx = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CUR_VX,LV_GND_SURF,0), x,y,date);
	cy = reader==NULL ? GRIB_NOTDEF
			: reader->getDateInterpolatedValue (DataCode(GRB_CUR_VY,LV_GND_SURF,0), x,y,date);
	if (cx!=GRIB_NOTDEF && cy!=GRIB_NOTDEF) {
		currentSpeed = sqrt (cx*cx + cy*cy);
		currentDir = - atan2 (-cx, cy) *180.0/M_PI;
		if (currentDir < 0)    currentDir += 360.0;
		else if (currentDir >= 360) currentDir -= 360.0;
	}
	else {
		currentSpeed = GRIB_NOTDEF;
		currentDir = GRIB_NOTDEF;
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
			if (hVx[i]!=GRIB_NOTDEF && hVy[i]!=GRIB_NOTDEF) {
				hWindSpeed[i] = sqrt (hVx[i]*hVx[i] + hVy[i]*hVy[i]);
				hWindDir[i]   = -atan2 (-hVx[i], hVy[i]) *180.0/M_PI + 180;
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
		case GRB_WIND_GUST	  : 
			return GUSTsfc;
		case GRB_SNOW_DEPTH   : 
			return snowDepth;
		case GRB_PRESSURE_MSL : 
			return pressureMSL;
		case GRB_PRECIP_TOT   : 
			return rain;
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
			
		case GRB_TEMP         : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC)
				return hTemp [dtc.getAltitude().index()];
			else
				return temp;
		case GRB_GEOPOT_HGT   :
			if (dtc.levelType == LV_ISOTHERM0)
				return isotherm0HGT;
			else if (dtc.getAltitude().levelType == LV_ISOBARIC)
				return hGeopot [dtc.getAltitude().index()];
		case GRB_HUMID_REL    : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC)
				return hHumidRel [dtc.getAltitude().index()];
			else
				return humidRel;
		case GRB_HUMID_SPEC   : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC)
				return hHumidSpec [dtc.getAltitude().index()];
			else
				return humidSpec;
		case GRB_PRV_THETA_E      : 
			if (dtc.getAltitude().levelType == LV_ISOBARIC)
				return hThetae [dtc.getAltitude().index()];
			else
				return GRIB_NOTDEF;
		//-----------------------------------
		// Waves
		//-----------------------------------
		case GRB_WAV_SIG_HT:
				return wave_sig_ht;
		case GRB_WAV_WND_DIR:
				return wave_wnd_dir;
		case GRB_WAV_WND_HT:
				return wave_wnd_ht;
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
	return *ht != GRIB_NOTDEF || *dir != GRIB_NOTDEF || *per != GRIB_NOTDEF;
}
//--------------------------------------------------------
float DataPointInfo::getWaveData (int type)  const
{	
	return getDataValue (DataCode(type,LV_GND_SURF,0));
}
//--------------------------------------------------------
bool DataPointInfo::hasWaveData (int type)  const
{	
	return getWaveData (type) != GRIB_NOTDEF;
}
//--------------------------------------------------------
bool DataPointInfo::hasWind (const Altitude &alt)  const
{	
	if (alt.levelType == LV_ISOBARIC) {
		int i = alt.index();
		return i>=0 && hVx[i]!=GRIB_NOTDEF && hVy[i]!=GRIB_NOTDEF;
	} 
	else if (alt.levelType == LV_GND_SURF) {
		return windDir_gnd!=GRIB_NOTDEF && windSpeed_gnd!=GRIB_NOTDEF;
	} 
	else
		return windDir_10m!=GRIB_NOTDEF && windSpeed_10m!=GRIB_NOTDEF;
}
//--------------------------------------------------------
bool DataPointInfo::hasCurrent ()  const
{	
	return currentDir!=GRIB_NOTDEF && currentSpeed!=GRIB_NOTDEF;
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
	else if (alt.levelType == LV_GND_SURF) {
		*vx = cx;
		*vy = cy;
	} 
}



