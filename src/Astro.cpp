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

#include "Astro.h"


//----------------------------------------------------------------
Almanac Astro::getSunMoonAlmanac (time_t t, double lat, double lon)
{
	struct ln_rst_time rst;
	struct ln_lnlat_posn observer;
	double JD;
	Almanac alm;
	alm.date = t;
	alm.lat = lat;
	alm.lon = lon;
	observer.lat = lat; 
	observer.lng = lon;  
	t = Util::timestampSetHour (t, 0,0,0);		// the day begins at midnight
	JD = ln_get_julian_from_timet (&t);  
	//----------------------------------------
	if (ln_get_solar_rst (JD, &observer, &rst) == 1)  {
			alm.sunRise = alm.sunTransit = alm.sunSet = 0;  // Sun is circumpolar 
	}
	else {
			ln_get_timet_from_julian (rst.rise,    &alm.sunRise);
			ln_get_timet_from_julian (rst.transit, &alm.sunTransit);
			ln_get_timet_from_julian (rst.set,     &alm.sunSet);
	}
	//----------------------------------------
	if (ln_get_lunar_rst (JD, &observer, &rst) == 1)  {
			alm.moonRise = alm.moonTransit = alm.moonSet = 0;  // Sun is circumpolar 
	}
	else {
			ln_get_timet_from_julian (rst.rise,    &alm.moonRise);
			ln_get_timet_from_julian (rst.transit, &alm.moonTransit);
			ln_get_timet_from_julian (rst.set,     &alm.moonSet);
	}
	alm.moonPhase = ln_get_lunar_phase (JD);
	alm.moonDisk = ln_get_lunar_disk (JD);
	return alm;
}




