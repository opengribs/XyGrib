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

#include <stdio.h>

#include "Orthodromie.h"
//#include <QtCore/QDebug>

// added by Tim Holtschneider, 05.2010
// define parameters for calculation
#define ZY_R_EARTH	6378.0
#define	ZY_MILE		1.852
    
//------------------------------------------------------------------------------
Orthodromie::Orthodromie(double x0,double y0, double x1,double y1)
{
    setPoints(x0,y0, x1,y1);
}
//------------------------------------------------------------------------------
void Orthodromie::setPoints(double x0,double y0, double x1,double y1)
{
    lon0 = x0 *M_PI/180.0;
    lat0 = y0 *M_PI/180.0;
    lon1 = x1 *M_PI/180.0;
    lat1 = y1 *M_PI/180.0;
    initOrthodromie();
}
//------------------------------------------------------------------------------
void Orthodromie::initOrthodromie()
{
    sinStartLat = sin(lat0);
    cosStartLat = cos(lat0);
    sinEndLat = sin(lat1);
    cosEndLat = cos(lat1);
    double deltaLng = reduceLng(lon1 - lon0);
    cosDeltaLng = cos(deltaLng);
    sinDeltaLng = sin(deltaLng);

    double cosang = sinStartLat*sinEndLat + cosStartLat*cosEndLat*cosDeltaLng;
    cosang = (cosang < -1.0) ? -1.0 : (cosang > 1.0) ? 1.0 : cosang;
	    
	distanceNM = ZY_R_EARTH/ZY_MILE * acos(cosang);

	azimut = reduceAzimut(
                 atan2(sin(lon1-lon0)*cos(lat1),
                       cos(lat0)*sin(lat1)-sin(lat0)*cos(lat1)*cos(lon1-lon0)));
    sinAzimut = sin(azimut);
    cosAzimut = cos(azimut);
    azimutDeg = 180.0/M_PI*azimut;
}
//------------------------------------------------------------------------------
//added by Tim Holtschneider, 05.2010
// calculate end point for distance and azimuth
void Orthodromie::getCoordsForDist( double  sLon, double  sLat, double dDist, double dAzmth,
								    double *eLon, double *eLat ) {	
	// conversion to radians
	double sLonRad, sLatRad, dAzmthRad, dDistRad;
	sLonRad   = sLon	* M_PI/180.0;
	sLatRad	  = sLat	* M_PI/180.0;
	dAzmthRad = dAzmth	* M_PI/180.0;
//	dDistRad  = dDist	* M_PI/180. * ZY_MILE / ZY_R_EARTH;
	dDistRad  = dDist	* ZY_MILE / ZY_R_EARTH;
	
	*eLat = asin( sin(sLatRad)  * cos(dDistRad) + cos(sLatRad)
				* sin(dDistRad) * cos(dAzmthRad) );
	*eLon = sLonRad + atan2( sin(dAzmthRad) * sin(dDistRad) * cos(sLatRad),
							 cos(dDistRad)  - sin(sLatRad)  * sin(*eLat) );
	
	// convert back to degrees
	*eLat = *eLat * 180./M_PI;
	*eLon = *eLon * 180./M_PI;
}
//------------------------------------------------------------------------------
void Orthodromie::setStartPoint (double x,double y)
{
    lat0 = y *M_PI/180.0;
    lon0 = x *M_PI/180.0;
    initOrthodromie();
}
//------------------------------------------------------------------------------
void Orthodromie::setEndPoint   (double x,double y)
{
    lat1 = y *M_PI/180.0;
    lon1 = x *M_PI/180.0;
    initOrthodromie();
}
    
//------------------------------------------------------------------------------
void Orthodromie::getMidPoint(double *x, double *y)
{
    double Bx = cosEndLat*cosDeltaLng;
    double By = cosEndLat*sinDeltaLng;
    double Bz = cosStartLat+Bx;
    *y = (double) (atan2( sin(lat0)+sin(lat1),  sqrt(Bz*Bz+By*By)));
    *x = (double) (lon0 + atan2(By, cos(lat0)+Bx));

//    printf("%f %f\n", *x, *y);
}

//------------------------------------------------------------------------------
//// Reduce an angle to (-PI/2, PI/2), for latitudes.
double Orthodromie::reduceLat(double lat)  // old name: fng(x)
{
  return lat - M_PI * floor((lat + M_PI/2.0) / M_PI);
}
//// Reduce and angle to (-PI, PI), for longitudes.
double Orthodromie::reduceLng(double lng)  // old name: fnl(x)
{
  return lng - 2.0*M_PI * floor((lng + M_PI ) / (2.0*M_PI));
}
//// Reduce an angle to (0, 2*PI), for direction and azimuth.
double Orthodromie::reduceAzimut(double azimut)
{
  return azimut - 2.0*M_PI * floor(azimut / (2.0*M_PI));
}
