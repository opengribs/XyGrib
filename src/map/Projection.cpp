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

#include <iostream>
#include <cmath>
#include <cassert>

#include "Projection.h"

//---------------------------------------------------------------------------
// Constructeur
//---------------------------------------------------------------------------
Projection::Projection (int w, int h, double cx, double cy, double scale)
	: QObject()
{
    init(w, h, cx, cy, scale);
}
//---------------------------------------------------------------------------
Projection::Projection (const Projection &model)
	: QObject()
{
    init(model.getW(),model.getH(),model.getCX(),model.getCY(), model.getScale());
}
//---------------------------------------------------------------------------
void Projection::init(int w, int h, double cx, double cy, double scale)
{
    scalemax = 40000;
	cylindrical = true;
    W = w;
    H = h;
    CX = cx;
    CY = cy;
    this->scale = scale;
}
//--------------------------------------------------------------
void Projection::updateBoundaries () {
    // Extrémités de la zone
    double x0,y0, x1,y1;
    
    screen2map(-1, -1, &x0, &y0);
    screen2map(W+1, H+1, &x1, &y1);
    
    xmin = x0;
    xmax = x1;
    
	if (y0 < y1) {
		double a = y1; y1 = y0; y0 = a;
	}
    ymin = y1;
    ymax = y0;
		    
//printf("Projection::updateBoundaries X(%f %f) Y(%f %f)\n", xmin,xmax, ymin,ymax);

	if (W*H != 0)
		coefremp = 10000.0*fabs( ((xmax-xmin)*(ymax-ymin)) / (W*H) );
	else
		coefremp = 10000.0;
		
	emit projectionUpdated();
}

//--------------------------------------------------------------
void Projection::setMapPointInScreen (double x,double y,  int pi,int pj)
{
	while (x > 180.0) {
		x -= 360.0;
	}
	while (x < -180.0) {
		x += 360.0;
	}

//printf("setMapPointInScreen  x=%f y=%f   pi=%d pj=%d\n", x,y, pi,pj);
	
	// Recherche dichotomique de la valeur de CX,CY pour que le pt (x,y) soit près de (pi,pj)
	// ie. screen2map(i,j) = (x,y)
	int n; 
	double xx, yy;
	double vmin, vtest, vmax;
	
	// Cherche CY
	vmin = -360;
	vmax =  360;
	n = 0;
	CX = x;    // valeur provisoire
	do {
		n ++;
		vtest = (vmin+vmax)/2.0;
		CY = vtest;
		screen2map(pi,pj, &xx,&yy);
		if (y < yy)
			vmax = vtest;
		else
			vmin = vtest;
		//printf("Y %3d : y=%f yy=%f CY=%f\n", n, y,yy, CY);
	} while (n<30 && fabs(yy-y)>1e-4);

	// Cherche CX
	vmin = -360;
	vmax =  360;
	n = 0;
	do {
		n ++;
		vtest = (vmin+vmax)/2.0;
		CX = vtest;
		screen2map(pi,pj, &xx,&yy);
		if (x < xx)
			vmax = vtest;
		else
			vmin = vtest;
		//printf("X %3d : x=%f xx=%f \n", n, x,xx);
	} while (n<30 && fabs(xx-x)>1e-4);
	
	updateBoundaries();
}


//---------------------------------------------------------------------------
bool Projection::intersect (double w,double e,double s,double n) const
{
    return ! (w>xmax || e<xmin || s>ymax || n<ymin);
}
//---------------------------------------------------------------------------
bool Projection::isPointVisible (double x,double y) const
{
    return (x<=xmax && x>=xmin && y<=ymax && y>=ymin);
}
//--------------------------------------------------------------
void Projection::setCentralPixel (int pi, int pj)
{
    double x, y;
    screen2map(pi, pj, &x, &y);
    setMapPointInScreen(x, y, W/2, H/2);
}

//--------------------------------------------------------------
void Projection::setScreenSize (int w, int h) {
    W = w;
    H = h;
    updateBoundaries();
}

//--------------------------------------------------------------
void Projection::move (double dx, double dy)
{
	double px,py;
	screen2map(W/2,H/2, &px,&py);
	setMapPointInScreen(px, py, (int)(W/2+dx*W+0.5), (int)(H/2-dy*H+0.5));
}

//--------------------------------------------------------------
void Projection::zoom (double k)
{
    setScale(scale*k);
}

//-------------------------------------------------------------------------------
bool Projection::map2screen_glob (double lon, double lat, int *pi, int *pj) const
{
	if (isPointVisible(lon, lat)) {
		map2screen(lon, lat, pi, pj);
		return true;
	}
	else if (isPointVisible(lon-360, lat)) {
		map2screen(lon-360, lat, pi, pj);
		return true;
	}
	else if (isPointVisible(lon+360, lat)) {
		map2screen(lon+360, lat, pi, pj);
		return true;
	}
	return false;
}





//====================================================================================
//====================================================================================
//---------------------------------------------------------------------------
Projection_ZYGRIB::Projection_ZYGRIB (int w, int h, double cx, double cy, double scale)
	: Projection(w, h, cx, cy, scale)
{
//printf("init Projection_ZYGRIB : W=%d H=%d   CX=%f CY=%f  scale=%f\n", W,H,CX,CY,scale);
    dscale = 1.2;
    setMapPointInScreen(CX, CY, W/2, H/2);
    setScale(scale);
}
//---------------------------------------------------------------------------
Projection_ZYGRIB::Projection_ZYGRIB (const Projection_ZYGRIB &model)
	: Projection(model)
{
    dscale = 1.2;
    setMapPointInScreen(CX, CY, W/2, H/2);
    setScale(scale);
}
//-------------------------------------------------------------------------------
void Projection_ZYGRIB::map2screen (double x, double y, int *i, int *j) const
{
	double scaley = scale*dscale;
	*i =  W/2 + (int) (scale * (x-CX) + 0.5);
	*j =  H/2 - (int) (scaley * (y-CY) + 0.5);
//printf("Projection_ZYGRIB::map2screen: x= %g   %d\n", x, *i);
}

//-------------------------------------------------------------------------------
void Projection_ZYGRIB::screen2map (int i, int j, double *x, double *y) const
{
	double scaley = scale*dscale;
	*x = (double)(i - W/2 + scale*CX)/ scale;
	*y = (double)(H/2 -j + scaley * CY)/ scaley;
//printf("screen2map : i=%d j=%d  ->  x=%f y=%f \n", i,j, *x,*y);
}

//--------------------------------------------------------------
void Projection_ZYGRIB::setVisibleArea (double x0, double y0, double x1, double y1)
{
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
    double sx, sy;
	// Nouvelle position du centre
	setMapPointInScreen( (x0+x1)/2.0, (y0+y1)/2.0 , W/2, H/2);
	sx = fabs(W/(x1-x0));
	sy = fabs(H/(y1-y0)) / dscale;
	scale = (sx<sy) ? sx : sy;
	setScale(scale);
}

//--------------------------------------------------------------
void Projection_ZYGRIB::setScale(double sc)
{
    double sx, sy, scaleall;

	sx = W/360.0;
	sy = H/180.0;
	scaleall = (sx<sy) ? sx : sy; // scaleall = Zoom sur la terre entière (limite max)
    scale = sc;
    if (scale < scaleall)
        scale = scaleall;
    if (scale > scalemax)
        scale = scalemax;
    updateBoundaries();
}



