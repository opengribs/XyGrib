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

#include <cstdlib>
#include "GriddedRecord.h"
#include "Util.h"

//------------------------------------------------------------
GriddedRecord::GriddedRecord ()
{
	dataCenterModel = OTHER_DATA_CENTER;
	duplicated = false;
	entireWorldInLongitude = false;
}

//=====================================================================
// Interpolation using a regular rectangular grid
//=====================================================================
data_t  GriddedRecord::getInterpolatedValueUsingRegularGrid (
				double lon, double lat,
				bool interpolateValues) const
{
    double val;
    double const eps = 1e-4;
    double pi, pj;     // coord. in grid unit
    // 00 10      point is in a square
    // 01 11
    int i0, j0, i1, j1;
    bool zero = false;

    if (!isOk() || getDeltaX()==0 || getDeltaY()==0) {
        return GRIB_NOTDEF;
    }
    if (!isYInMap(lat)) {
		return GRIB_NOTDEF;
    } 
    if (!isXInMap(lon)) {
		if (! entireWorldInLongitude) {
			lon += 360.0;               // tour du monde à droite ?
			if (!isXInMap(lon)) {
				lon -= 2*360.0;              // tour du monde à gauche ?
				if (!isXInMap(lon)) {
					return GRIB_NOTDEF;
				}
			}
		}
		else {
			while (lon< 0)
				lon += 360;
			if (lon > xmax) {
			    zero = true;
			}
		}
    }
    else if (lon < xmin) {
        lon += 360.;
    }

    lonLat2XY(lon, lat, &pi, &pj);
    i0 = (int) floor(pi);  // point 00
	i1 = zero?0:i0+1;
	
    j0 = (int) floor(pj);
	j1 = j0+1;
	
	// value very close to a grid point ?
    double ddx, ddy;
	ddx = fabs (pi-i0);
	ddy = fabs (pj-j0);
	int ii = (ddx<eps) ? i0 : ((1-ddx)<eps) ? i1 : -1;
	int jj = (ddy<eps) ? j0 : ((1-ddy)<eps) ? j1 : -1;
	if (ii>=0 && jj>=0) {
        return getValueOnRegularGrid (ii, jj);
	}

    bool   h00,h01,h10,h11;
	
	double x00 = getValueOnRegularGrid (i0,j0);
	double x01 = getValueOnRegularGrid (i0,j1);
	double x10 = getValueOnRegularGrid (i1,j0);
	double x11 = getValueOnRegularGrid (i1,j1);

// if (dtc.dataType==GRB_CLOUD_TOT)
// printf("GriddedRecord::getInterpolatedValueUsingRegularGrid : %d: %f %f %f %f\n",dtc.dataType, x00,x01,x10,x11);	

	int nbval = 0;     // how many values in grid ?
    if ((h00 = GribDataIsDef(x00)))
        nbval ++;
    if ((h10 = GribDataIsDef(x10)))
        nbval ++;
    if ((h01 = GribDataIsDef(x01)))
        nbval ++;
    if ((h11 = GribDataIsDef(x11)))
        nbval ++;
	
    if (nbval <3) {
        return GRIB_NOTDEF;
    }

    // distances to 00
    double dx = pi-i0;
    double dy = pj-j0;

	if (! interpolateValues)
	{
		if (dx < 0.5) {
			if (dy < 0.5)
				val = x00;
			else
				val = x01;
		}
		else {
			if (dy < 0.5)
				val = x10;
			else
				val = x11;
		}
		return val;
	}

    dx = (3.0 - 2.0*dx)*dx*dx;   // pseudo hermite interpolation
    dy = (3.0 - 2.0*dy)*dy*dy;

    double xa, xb, xc, kx, ky;
    // Triangle :
    //   xa  xb
    //   xc
    // kx = distance(xa,x)
    // ky = distance(xa,y)
    if (nbval == 4)
    {
        double x1 = (1.0-dx)*x00 + dx*x10;
        double x2 = (1.0-dx)*x01 + dx*x11;
        val =  (1.0-dy)*x1 + dy*x2;
        return val;
    }
    // here nbval==3, check the corner without data
    if (!h00) {
        //printf("! h00  %f %f\n", dx,dy);
        xa = x11;   // A = point 11
        xb = x01;   // B = point 01
        xc = x10;   // C = point 10
        kx = 1-dx;
        ky = 1-dy;
    }
    else if (!h01) {
        //printf("! h01  %f %f\n", dx,dy);
        xa = x10;   // A = point 10
        xb = x11;   // B = point 11
        xc = x00;   // C = point 00
        kx = dy;
        ky = 1-dx;
    }
    else if (!h10) {
        //printf("! h10  %f %f\n", dx,dy);
        xa = x01;     // A = point 01
        xb = x00;     // B = point 00
        xc = x11;     // C = point 11
        kx = 1-dy;
        ky = dx;
    }
    else {
        //printf("! h11  %f %f\n", dx,dy);
        xa = x00;  // A = point 00
        xb = x10;  // B = point 10
        xc = x01;  // C = point 01
        kx = dx;
        ky = dy;
    }
    
    double k = kx + ky;
    if (k<0 || k>1) {
        val = GRIB_NOTDEF;
    }
    else if (k == 0) {
        val = xa;
    }
    else {
        // axes interpolation
        double vx = k*xb + (1-k)*xa;
        double vy = k*xc + (1-k)*xa;
        // diagonal interpolation
        double k2 = kx / k;
        val =  k2*vx + (1-k2)*vy;
    }
    return val;
}
