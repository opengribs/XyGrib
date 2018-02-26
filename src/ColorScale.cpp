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

#include "ColorScale.h" 

#include "zuFile.h" 

//----------------------------------------------------
// ColorScale
//----------------------------------------------------
ColorScale::ColorScale () {
	transparence = 255;
}
//--------------------------------------------
ColorScale::~ColorScale () {
	Util::cleanVectorPointers (colors);
}

//--------------------------------------------
QRgb ColorScale::getColor (double v, bool smooth)
{	
	int imin = 0;
	int imax = colors.size()-1;
	int ii = (imin+imax)/2;
	while (imin<=imax && !colors[ii]->isIn(v)) {
		if (colors[ii]->isLow(v))
			imax = ii-1;
		else
			imin = ii+1;
		ii = (imin+imax)/2;
	}
	if (imin>imax) {
		if (colors[0]->isLow(v))
			return colors[0]->getColor (v, smooth, transparence);
		else
			return colors[colors.size()-1]->getColor (v, smooth, transparence);
	}
	else
		return colors[ii]->getColor (v, smooth, transparence);
}

//--------------------------------------------
bool ColorScale::readFile (QString filename, double kv, double offset)
{
	ColorElement *ea, *eb;
	char buf [1000];
	Util::cleanVectorPointers (colors);
	ZUFILE *f = zu_open (qPrintable(filename), "r");
	if (f) {	
		char *oldlocale = setlocale (LC_NUMERIC, "C");
		double va = -1e300;		// previous element values
		int ra=0, ga=0, ba=0;   

		double v;
		int r, g, b;
		while (zu_fgets(buf, 1000, f)) {
			if (strlen(buf)>0  &&  buf[0]!='#')
			{
				if (sscanf(buf, "%lf %d %d %d", &v,&r,&g,&b) == 4) {
					v = v*kv + offset;
					if (v > va) {
						ea = new ColorElement (va, v, ra,ga,ba, r,g,b);
						assert (ea);
						colors.push_back (ea);
						ra=r; ga=g; ba=b;
						va=v;
					}
					else {
						DBG("Error: %g %d %d %d : %g >= %lf", v,r,g,b, va,v);
					}
				}
			}
		}
		if (colors.size() < 2) {
			Util::cleanVectorPointers (colors);
			DBG("Error: file %s: colors.size() < 2: %d", 
								qPrintable(filename), (int)colors.size());
			ea = new ColorElement (-10,0, 80,80,255, 80,80,255);
			assert (ea);
			colors.push_back (ea);
			ea = new ColorElement (0,10, 80,80,255, 120,255,200);
			assert (ea);
			colors.push_back (ea);
			ea = new ColorElement (10,100, 120,255,200, 255,120,120);
			assert (ea);
			colors.push_back (ea);
		}
		// first element : unique color for all values < vmin
		ea = colors [0];
		eb = colors [1];
		ea->vmin = eb->vmin - (eb->vmax-eb->vmin);   // same length than first real element
		ea->vmax = eb->vmin;                 // same length than first real element
		ea->ra=ea->rb; ea->ga=ea->gb; ea->ba=ea->bb;  // unique color
		setlocale (LC_NUMERIC, oldlocale);
	}
	else {
		DBG ("Error: can't open file %s", qPrintable(filename));
		ea = new ColorElement (-10,0, 80,80,255, 80,80,255);
		assert (ea);
		colors.push_back (ea);
		ea = new ColorElement (0,10, 80,80,255, 120,255,200);
		assert (ea);
		colors.push_back (ea);
		ea = new ColorElement (10,100, 120,255,200, 255,120,120);
		assert (ea);
		colors.push_back (ea);
	}
	
	return true;
}
//--------------------------------------------
void ColorScale::dbg ()
{
	DBG("--- ColorScale (sz=%d) ---", (int)colors.size());
	for (uint i=0; i<colors.size(); i++) {
		colors[i] -> dbg();
	}
}
//--------------------------------------------
void ColorScale::addColor (ColorElement *color)
{
	if (color && ( colors.size()==0 
				 || color->vmin==colors[colors.size()-1]->vmax)
	) {
		colors.push_back (color);
	}
}

//----------------------------------------------------
// ColorElement
//----------------------------------------------------
void ColorElement::dbg ()
{
	DBG("ColorElement: (%g->%g) (%d %d %d)->(%d %d %d)", vmin,vmax,ra,ga,ba,rb,gb,bb);
}

ColorElement::ColorElement (double vmin, double vmax, 
							 int ra, int ga, int ba,
							 int rb, int gb, int bb
) {
	if (vmin >= vmax) {
		DBG ("Color error: vmin=%g vmax=%g", vmin, vmax);
		if (vmin == vmax)
			vmax = vmin+1;
		else {
			double vt=vmin; vmin=vmax; vmax=vt;
		}
	}
	this->vmin = vmin;
	this->vmax = vmax;
	this->ra = ra;  this->ga = ga;  this->ba = ba;
	this->rb = rb;  this->gb = gb;  this->bb = bb;
}
//--------------------------------------------
QRgb ColorElement::getColor (double v, bool smooth, int transp)
{
	if (v <= vmin)
		return qRgba (ra,ga,ba, transp);
	else if (v >= vmax)
		return qRgba (rb,gb,bb, transp);
	
	if (! smooth) {
		if (v-vmin > vmax-v)
			return qRgba (rb,gb,bb, transp);
		else
			return qRgba (ra,ga,ba, transp);
	}
	
	double k = (v-vmin)/(vmax-vmin);
	return qRgba ( (int) ((1-k)*ra + k*rb + 0.5),
				   (int) ((1-k)*ga + k*gb + 0.5),
				   (int) ((1-k)*ba + k*bb + 0.5),  transp );
}


