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

#include "QPainter"

#include "Font.h"
#include "LonLatGrid.h"

//---------------------------------------------------------------
double LonLatGrid::computeDeltaGrid(int *min, double pixperdeg)
{
	double pixmin = 140;		// Ecart mini en pixels entre lignes
	
	if (pixperdeg <= 0.1)
		pixperdeg = 0.1;
		
	double delta = pixmin/pixperdeg;
	
	// Alignement sur 360
	if (delta > 90) {
		delta = 90;
	}
	else if (delta > 45) {
		delta = 45;
	}
	else if (delta > 30) {
		delta = 30;
	}
	else if (delta > 15) {
		delta = 15;
	}
	else if (delta > 10) {
		delta = 10;
	}
	else if (delta > 5) {
		delta = 5;
	}
	else if (delta > 2) {
		delta = 2;
	}
	else if (delta > 1) {
		delta = 1;
	}
	else if (delta > 0.5) {
		delta = 0.5;
	}
	else if (delta > 0.2) {
		delta = 0.2;
	}
	else if (delta > 0.1) {
		delta = 0.1;
	}
	else {
		delta = 0.05;
	}

	// Alignement sur 0 (entiers)
	if (delta >= 1) {
		while ( (abs(*min) % (int)delta) != 0)
			(*min) --;
	}
//printf("pixperdeg=%g  delta=%g\n",	pixperdeg, delta);
	return delta;
}

//---------------------------------------------------------------
void LonLatGrid::drawLongitude (QPainter &pnt, Projection *proj)
{
	int  min = (int) floor (proj->getXmin());
	int  max = (int) ceil  (proj->getXmax());
	double pixperdeg = proj->getW()/abs(max-min);
	double delta = computeDeltaGrid(&min, pixperdeg);
	
	QColor gridColor(40,40,40);
	QPen pen (gridColor);
	pen.setWidthF(0.8);
	pnt.setPen(pen);
	
	double x;
	int i, j0,j1;
	for (x=min; x<=max; x += delta)
	{
		proj->map2screen(x, floor(proj->getYmin()), &i, &j0);
		proj->map2screen(x, ceil (proj->getYmax()), &i, &j1);
		
		pnt.drawLine (i, j0, i, j1);
	}	
}

//---------------------------------------------------------------
void LonLatGrid::drawLatitude (QPainter &pnt, Projection *proj)
{
	int  min = (int) floor (proj->getYmin());
	int  max = (int) ceil  (proj->getYmax());
	double pixperdeg = proj->getW()/abs(max-min);	
	double delta = computeDeltaGrid(&min, pixperdeg);
	
	QColor gridColor(40,40,40);
	QPen pen (gridColor);
	pen.setWidthF(0.8);
	pnt.setPen(pen);
	
	double y;
	int i0, i1, j;
	for (y=min; y<=max; y += delta)
	{
		proj->map2screen( floor(proj->getXmin()), y, &i0, &j);
		proj->map2screen( ceil (proj->getXmax()), y, &i1, &j);
		
		pnt.drawLine (i0, j, i1, j);

	}	
}

//---------------------------------------------------------------
void LonLatGrid::drawLonLatGrid (QPainter &pnt, Projection *proj)
{
	drawLongitude (pnt, proj);
	drawLatitude (pnt, proj);
}


//---------------------------------------------------------------
void LonLatGrid::drawLabel(QPainter &pnt, const Projection *proj,
				double value, int i, int j)
{
    int   a,b,c,d;
    QString label;
	QColor couleur(10,10,10);

    label = label.sprintf("%g", value);

    QPen penText(couleur);
    QFont fontText = Font::getFont(FONT_LonLatGridLabel);
    QFontMetrics fmet(fontText);
    QRect rect = fmet.boundingRect(label);
    pnt.setPen(penText);
    pnt.setFont(fontText);

	// use a gradient, because it's a bug sometimes with solid pattern (black background)
	QLinearGradient gradient;
    int r = 255;
	gradient.setColorAt(0, QColor(r,r,r, 170));
	gradient.setColorAt(1, QColor(r,r,r, 170));
    pnt.setBrush(gradient);

    //---------------------------------------------------------
    // Ecrit le label
    //---------------------------------------------------------
	
// 	rect = fmet.boundingRect(label);
// 	rect.moveTo((a+c)/2-rect.width()/2, (b+d)/2-rect.height()/2);
// 	pnt.drawRect(rect.x()-1, rect.y(), rect.width()+2, fmet.ascent()+2);
// 	pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, label);

}













