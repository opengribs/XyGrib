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

#include <iostream>
#include <cmath>
#include <cassert>
#include <cstdio>

#include "Projection.h"


//-----------------------------------------------------------------------------------------
Projection_libproj::Projection_libproj(int code, int w, int h, double cx, double cy, double scale)
	: Projection(w,h, cx,cy, scale)
{
	setProjection(code);
	CX = cx;
	CY = cy;
    Projection_libproj::setScale(scale); // virtual function call in CTOR
//    setCenterPosition(cx, cy);
}
//-----------------------------------------------------------------------------------------
Projection_libproj::Projection_libproj(const Projection_libproj &model)
	: Projection(model.getW(),model.getH(), model.getCX(),model.getCY(),model.getScale())
{
	setProjection(model.currentProj);
	CX = model.getCX();
	CY = model.getCY();
    Projection_libproj::setScale(model.getScale());
//    setCenterPosition(model.getCX(),model.getCY());
}
//-----------------------------------------------------------------------------------------
void Projection_libproj::setProjection(int code)
{
    const char *params[20];
	int nbpar=0;
	switch (code)
	{
		case PROJ_UTM :
			nbpar = 0;
            params[nbpar++] = "proj=utm";
			cylindrical = false;
			break;
		case PROJ_CENTRAL_CYL:
			nbpar = 0;
            params[nbpar++] = "proj=cc";
			cylindrical = true;
			break;
		case PROJ_EQU_CYL:
			nbpar = 0;
            params[nbpar++] = "proj=eqc";
			cylindrical = true;
			break;
		case PROJ_MILLER :
			// Miller
			nbpar = 0;
            params[nbpar++] = "proj=mill";
			cylindrical = true;
			break;
		case PROJ_LAMBERT_CONF_CON :
			// Lambert Conformal Conic
			nbpar = 0;
            params[nbpar++] = "proj=lcc";
            params[nbpar++] = "lat_1=80N";
            params[nbpar++] = "lat_2=20N";
            params[nbpar++] = "lon_0=0W";
			cylindrical = false;
			break;
		case PROJ_MERCATOR :
		default :
			// Mercator
			nbpar = 0;
            params[nbpar++] = "proj=merc";
			cylindrical = true;
			break;
	}
    params[nbpar++] = "ellps=WGS84";
    params[nbpar++] = "no_defs";
    params[nbpar++] = "over";	// allow longitude > 180Â°
#ifdef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
    libProj = pj_init(nbpar, (char **)params);
	if (!libProj)
		printf("proj error: %s\n", pj_strerrno(pj_errno));
#else
    libProj = proj_create_argv(PJ_DEFAULT_CTX, nbpar, (char **)params);
	if (!libProj)
		printf("proj error: %s\n", proj_errno_string(proj_errno(libProj)));
#endif
	assert(libProj);
	currentProj = code;
//	libProj->over = 1;		// allow longitude > 180°
//printf("Projection: %s\n", libProj->descr);
}

//-----------------------------------------------------------------------------------------
Projection_libproj::~Projection_libproj()
{
    if (libProj != nullptr) {
#ifdef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
		pj_free(libProj);
#else
		proj_destroy(libProj);
#endif
	}
}

//-------------------------------------------------------------------------------
void Projection_libproj::map2screen(double x, double y, int *i, int *j) const
{
#ifdef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
	projUV data, res;
#else
	PJ_COORD data, res;
#endif
	if (y <= -90.0)
		y = -90.0+1e-5;
	if (y >= 90.0)
		y = 90.0-1e-5;
#ifdef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
	data.v =  y * DEG_TO_RAD;
	data.u =  x * DEG_TO_RAD;
	res = pj_fwd(data, libProj);
	*i =  (int) (W/2.0 + scale * (res.u/111319.0-CX) + 0.5);
	*j =  (int) (H/2.0 - scale * (res.v/111319.0-CY) + 0.5);
#else
	data = proj_coord(0., 0., 0., 0.);
	data.uv.v =  proj_torad(y);
	data.uv.u =  proj_torad(x);
	res = proj_trans(libProj, PJ_FWD, data);
	*i =  (int) (W/2.0 + scale * (res.uv.u/111319.0-CX) + 0.5);
	*j =  (int) (H/2.0 - scale * (res.uv.v/111319.0-CY) + 0.5);
#endif
	//printf("PROJ   map2screen (%f %f) -> (%3d %3d)\n", x,y, *i,*j);
}


//-------------------------------------------------------------------------------
void Projection_libproj::screen2map(int i, int j, double *x, double *y) const
{
#ifdef ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
	projUV data, res;
	data.u =  ((i-W/2.0)/scale+ CX)*111319.0 ;
	data.v =  ((H/2.0-j)/scale+ CY)*111319.0 ;
	res = pj_inv(data, libProj);
	*x = (double)(res.u*RAD_TO_DEG);
	*y = (double)(res.v*RAD_TO_DEG);
#else
	PJ_COORD data, res;

	data = proj_coord(0., 0., 0., 0.);
	data.uv.u =  ((i-W/2.0)/scale+ CX)*111319.0 ;
	data.uv.v =  ((H/2.0-j)/scale+ CY)*111319.0 ;
	res = proj_trans(libProj, PJ_INV, data);
	*x = proj_todeg(res.uv.u);
	*y = proj_todeg(res.uv.v);
#endif
	//printf("PROJ   screen2map (%3d %3d) -> (%f %f)\n", i,j, *x,*y);
}
//--------------------------------------------------------------
void Projection_libproj::setVisibleArea(double x0, double y0, double x1, double y1)
{
//printf("setVisibleArea X(%f %f) Y(%f %f)\n", x0, x1,y0,y1);
    if (x1 == x0) {
    	x1 = x0+0.1;
	}
    if (y1 == y0) {
    	y1 = y0+0.1;
	}
	if (x0 > x1) {
		double a = x1; x1 = x0; x0 = a;
	}
	if (y0 > y1) {
		double a = y1; y1 = y0; y0 = a;
	}
	while (x0 > 360) {
		x0 -= 360;
		x1 -= 360;
	}
	while (x0 < -360) {
		x0 += 360;
		x1 += 360;
	}
	while (y0 <= -98) {
		y0 += 10;
		y1 += 10;
	}
	while (y1 >= 98) {
		y0 -= 10;
		y1 -= 10;
	}
	if (x0 > 180. && x1 > 180.) {
		// in this case screen2map returns -180 .. 0
		x0 -= 360.;
		x1 -= 360.;
	}
    
    double sx, sy;
	// On cherche l'échelle qui permet d'englober la zone
	// En commençant par placer le coin haut gauche (x0,y1) de la zone
	int n;
	double dsc, smin, stest, smax, xx,yy;
	sx = sy = scale;
	// Vertical scale
	stest = 0;
	smin = 0.01;
	smax = scalemax;
	n = 0;
	do {
		dsc = stest;
		stest = (smin+smax)/2.0;
		dsc = fabs(dsc-stest);
		setScale(stest);
		setMapPointInScreen(x0,y1, 0,0);
		screen2map(W, H,  &xx,&yy);
		if (yy < y0)
			smin = stest;
		else
			smax = stest;
		//printf("Y %3d smin=%f smax=%f   %f -> %f\n", n,smin,smax, y1,yy);
		n ++;
	} while (n<30 && dsc>1e-4);
	sy = stest;

	// Horizontal scale
	stest = 0;
	smin = 0.01;
	smax = scalemax;
	n = 0;
	do {
		dsc = stest;
		stest = (smin+smax)/2.0;
		dsc = fabs(dsc-stest);
		setScale(stest);
		setMapPointInScreen(x0,y1, 0,0);
		screen2map(W, H,  &xx,&yy);
		if (xx > x1)
			smin = stest;
		else
			smax = stest;
		//printf("X %3d stest=%f    %f -> %f\n", n,stest, x1,xx);
		n ++;
	} while (n<30 && dsc>1e-4);
	sx = stest;

	if (sx < sy) {
		setScale(sx);
		setMapPointInScreen(x0, (y0+y1)/2, 0,H/2);
	//printf("1\n");
	}
	else {
	//printf("2\n");
		setScale(sy);
		setMapPointInScreen( (x0+x1)/2, y0, W/2,H);
	}
}

//--------------------------------------------------------------
void Projection_libproj::setScale(double sc)
{
    double scaleall;
    
	scaleall = 0.4; // scaleall = Zoom sur la terre entière (limite max)
    
    scale = sc;
    if (scale < scaleall)
        scale = scaleall;
    if (scale > scalemax)
        scale = scalemax;
    updateBoundaries();
//printf("setScale(%f) -> scale=%f\n", sc,scale);
}

