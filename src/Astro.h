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

#ifndef ASTRO_H
#define ASTRO_H

#include "Util.h"

#include <libnova/julian_day.h>
#include <libnova/solar.h>
#include <libnova/lunar.h>

//------------------------------
class Almanac
{
	public:
		time_t sunRise, sunTransit, sunSet;
		time_t moonRise, moonTransit, moonSet;
		double moonPhase;
		double moonDisk;
		
		time_t date;
		double lat, lon;
};
 
//------------------------------
class Astro
{
	public:
		static Almanac getSunMoonAlmanac (time_t t, double lat, double lon);
		
		static time_t timestampSetHour (int h, int m, int s);
	
};


#endif
