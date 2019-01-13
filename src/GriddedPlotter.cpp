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

#include "GriddedPlotter.h"
#include "DataQString.h"

/* Longueur de fleche courant */
#define LF_MINC_A	3.
#define LF_MAXC_A	14.
#define LF_MINC		0.05
#define	LF_MAXC		0.5

/* Largeur de fleche courant */
#define QF_MINC_A	2.
#define QF_MAXC_A	14.
#define QF_MINC		0.5
#define QF_MAXC		10.

/* Tete de fleche */
#define TF_MINC_A	1.0
#define TF_MAXC_A	4.0
#define TF_MINC		0.5
#define TF_MAXC		8.0

//--------------------------------------------------------------------
GriddedPlotter::GriddedPlotter ()
{
	fastInterpolation = true;
	windAltitude = Altitude (LV_TYPE_NOT_DEFINED,0);
	
    windArrowSpace = 28;      // distance mini entre flèches
    windBarbuleSpace = 34;    // distance mini entre flèches
    
    windArrowSpaceOnGrid = 20;      // distance mini entre flèches
    windBarbuleSpaceOnGrid = 28;    // distance mini entre flèches
    
    windArrowSize = 14;       // longueur des flèches
    windBarbuleSize = 26;     // longueur des flèches avec barbules

    currentArrowSpace = 28;      // distance mini entre flèches
    currentArrowSpaceOnGrid = 20;      // distance mini entre flèches
    
    GriddedPlotter::updateGraphicsParameters ();
	
	useJetStreamColorMap = false;
    setUseGustColorAbsolute(Util::getSetting("useAbsoluteGustSpeed", true).toBool());
}

//--------------------------------------------------------------------
GriddedPlotter::~GriddedPlotter ()
{
	listDates.clear();
}
//---------------------------------------------------
void GriddedPlotter::setUseGustColorAbsolute (bool b)
{
	useGustColorAbsolute = b;
	if (b)
 	    colors_Gust.readFile (Util::pathColors()+"colors_wind_kts.txt", 1.852/3.6, 0);
 	else
 	    colors_Gust.readFile (Util::pathColors()+"colors_gust_kts.txt", 1.852/3.6, 0);
}
//---------------------------------------------------
void GriddedPlotter::updateGraphicsParameters ()
{
	setCloudsColorMode ("cloudsColorMode");	
    thinWindArrows = Util::getSetting("thinWindArrows", false).toBool();
}

//==================================================================================
// Flèches de direction du vent
//==================================================================================
void GriddedPlotter::drawTransformedLine( QPainter &pnt,
        double si, double co,int di, int dj, int i,int j, int k,int l)
{
//     int ii, jj, kk, ll;
//     ii = (int) (i*co-j*si +0.5) + di;
//     jj = (int) (i*si+j*co +0.5) + dj;
//     kk = (int) (k*co-l*si +0.5) + di;
//     ll = (int) (k*si+l*co +0.5) + dj;
    double ii, jj, kk, ll;
    ii = i*co-j*si + di;
    jj = i*si+j*co  + dj;
    kk = k*co-l*si + di;
    ll = k*si+l*co + dj;
    // Clip forcé à cause d'un bug qpixmap sous windows
    int w = pnt.device()->width();
    int h = pnt.device()->height();
    if (       Util::isInRange(ii, 0, w)
            && Util::isInRange(kk, 0, w)
            && Util::isInRange(jj, 0, h)
            && Util::isInRange(ll, 0, h) )
        pnt.drawLine(ii, jj, kk, ll);
}
//-----------------------------------------------------------------------------
//void GriddedPlotter::drawWaveArrow (QPainter &pnt,
//			int i, int j, double dir, double /*period*/)
void GriddedPlotter::drawWaveArrow (QPainter &pnt,
            int i, int j, double dir)
{
    double ang = (dir-90)/180.0*M_PI;
    double si=sin(ang),  co=cos(ang);
    QPen pen (QColor(255,0,255)); // color fuchsia
    pen.setWidth (1);
    pnt.setPen (pen);
    drawTransformedLine(pnt, si,co, i+windArrowSize/2,j,  0,0, windArrowSize, 0);   // hampe
    drawTransformedLine(pnt, si,co, i+windArrowSize/2,j,  0,0,  5, 2);   // flèche
    drawTransformedLine(pnt, si,co, i+windArrowSize/2,j,  0,0,  5,-2);   // flèche
}
//-----------------------------------------------------------------------------
void GriddedPlotter::drawCurrentArrow (QPainter &pnt, int i, int j, double cx, double cy)
{
    double vkn = sqrt(cx*cx+cy*cy)*3.6/1.852;
	// double ang = atan2(cy, -cx)-M_PI;  // unlike wind, arrows follows the current
    double ang = atan2(cy, -cx);  // unlike wind, arrows follows the current
    
    double si=sin(ang),  co=cos(ang);

    double tf_a = (TF_MAXC_A - TF_MINC_A) / (TF_MAXC - TF_MINC);
    double tf_b = TF_MAXC_A-TF_MAXC*tf_a;
    double qf_a = (QF_MAXC_A - QF_MINC_A) / (QF_MAXC - QF_MINC);
    double qf_b = QF_MAXC_A-QF_MAXC*qf_a;
    double lf_a = (LF_MAXC_A - LF_MINC_A) / (LF_MAXC - LF_MINC);
    double lf_b = LF_MAXC_A-LF_MAXC*lf_a;
    double coefLen;
    double ytt,yqt;

    if (vkn > LF_MAXC)
		coefLen = lf_a*LF_MAXC + lf_b;
    else if (vkn < LF_MINC)
        coefLen = lf_a*LF_MINC + lf_b;
    else
        coefLen = lf_a*vkn + lf_b;

    if (vkn > QF_MAXC)
        yqt = qf_a*QF_MAXC + qf_b;
    else if (vkn < QF_MINC)
        yqt = qf_a*QF_MINC + qf_b;
    else
        yqt = qf_a*vkn + qf_b;

    if (vkn > TF_MAXC)
        ytt = tf_a*TF_MAXC + tf_b;
    else if (vkn < TF_MINC)
        ytt = tf_a*TF_MINC + tf_b;
    else
        ytt = tf_a*vkn + tf_b;

    currentArrowColor = QColor(0,0,255); // color blue
	coefLen = 1.4*coefLen;	
	
    QPen pen (currentArrowColor);
    pen.setWidth (yqt);
    pnt.setPen (pen);
    drawTransformedLine(pnt, si,co, i+ coefLen/2,j,  0,0, coefLen, 0);   // hampe

    drawTransformedLine(pnt, si,co, i+ coefLen/2,j,  0,0,  5, 2*ytt);   // flèche
    drawTransformedLine(pnt, si,co, i+ coefLen/2,j,  0,0,  5,-2*ytt);   // flèche
    drawTransformedLine(pnt, si,co, i+ coefLen/2,j,  5,2*ytt,  5,-2*ytt);   // flèche
}
//-----------------------------------------------------------------------------
// Called from MeteoTable
void GriddedPlotter::drawCurrentArrow (
			QPainter &pnt,
			int i, int j, double cx, double cy,
			bool /*south*/,
			const QColor& arrowColor
	)
{
    double vkn = sqrt(cx*cx+cy*cy)*3.6/1.852;
    double ang = atan2(cy, -cx);  // unlike wind, arrows follows the current
    double si=sin(ang),  co=cos(ang);
 
    QPen pen( arrowColor);
	pen.setWidth (2);
    pnt.setPen(pen);
    pnt.setBrush(Qt::NoBrush);

    if (vkn < LF_MINC)
    {
        int r = 5;     // courant très faible, dessine un cercle
        pnt.drawEllipse(i-r,j-r,2*r,2*r);
    }
    else {
        // Flèche centrée sur l'origine
        int dec = -windArrowSize/2;
        drawTransformedLine(pnt, si,co, i,j,  dec,0, dec+windArrowSize, 0);   // hampe

        drawTransformedLine(pnt, si,co, i,j,  dec,0,  dec+5, 2);   // flèche
        drawTransformedLine(pnt, si,co, i,j,  dec,0,  dec+5,-2);   // flèche

    }        
}
//-----------------------------------------------------------------------------
void GriddedPlotter::drawWindArrow (QPainter &pnt, int i, int j, double vx, double vy)
{
    double ang = atan2(vy, -vx);
    double si=sin(ang),  co=cos(ang);
    QPen pen (windArrowColor);
	if (thinWindArrows)
		pen.setWidth (1);
	else
		pen.setWidth (2);
    pnt.setPen (pen);
    drawTransformedLine(pnt, si,co, i-windArrowSize/2,j,  0,0, windArrowSize, 0);   // hampe
    drawTransformedLine(pnt, si,co, i-windArrowSize/2,j,  0,0,  5, 2);   // flèche
    drawTransformedLine(pnt, si,co, i-windArrowSize/2,j,  0,0,  5,-2);   // flèche
}
//-----------------------------------------------------------------------------
void GriddedPlotter::drawWindArrowWithBarbs (
			QPainter &pnt,
			int i, int j, double vx, double vy,
			bool south,
			const QColor& arrowColor
	)
{
    GriddedPlotter::drawWindArrowWithBarbs_static (
                    pnt,
					i, j, vx, vy,
					south,
        			arrowColor,
					this->windBarbuleSize,
					this->thinWindArrows);
}
//-----------------------------------------------------------------------------
void GriddedPlotter::drawWindArrowWithBarbs_static (
					QPainter &pnt,
					int i, int j, double vx, double vy,
					bool south,
        			const QColor& arrowColor,
					int  windBarbuleSize,
					bool thinWindArrows
	)
{
	if (! GribDataIsDef(vx) || ! GribDataIsDef(vy))
		return;
    double vkn = sqrt(vx*vx+vy*vy)*3.6/1.852;
    double ang = atan2(vy, -vx);
    double si=sin(ang),  co=cos(ang);
    
    QPen pen (arrowColor);
	if (thinWindArrows) {
		pen.setColor (QColor(40,40,40)); // color dark jungle green
		//pen.setColor (QColor(255,255,255));
		pen.setWidth (1);
	}
	else {
		//pen.setColor (QColor(0,0,0));
		pen.setWidth (2);
	}
    pnt.setPen(pen);
    pnt.setBrush(Qt::NoBrush);
    
    if (vkn < 1)
    {
        int r = 5;     // vent très faible, dessine un cercle
        pnt.drawEllipse(i-r,j-r,2*r,2*r);
    }
    else {
        // Flèche centrée sur l'origine
        int dec = -windBarbuleSize/2;
        drawTransformedLine(pnt, si,co, i,j,  dec,0,  dec+windBarbuleSize, 0);   // hampe
		if (! thinWindArrows) {
			drawTransformedLine(pnt, si,co, i,j,  dec,0,  dec+5, 2);    // flèche
			drawTransformedLine(pnt, si,co, i,j,  dec,0,  dec+5, -2);   // flèche
		}
	 	int b1 = dec+windBarbuleSize -4;  // position de la 1ère barbule
		if (vkn >= 7.5  &&  vkn < 45 ) {
			b1 = dec+windBarbuleSize;  // position de la 1ère barbule si >= 10 noeuds
		}
        
        if (vkn < 7.5) {  // 5 ktn
            drawPetiteBarbule(pnt,south, si,co, i,j, b1);
        }
        else if (vkn < 12.5) { // 10 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
        }
        else if (vkn < 17.5) { // 15 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawPetiteBarbule(pnt,south, si,co, i,j, b1-4);
        }
        else if (vkn < 22.5) { // 20 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-4);
        }
        else if (vkn < 27.5) { // 25 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-4);
            drawPetiteBarbule(pnt,south, si,co, i,j, b1-8);
        }
        else if (vkn < 32.5) { // 30 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
        }
        else if (vkn < 37.5) { // 35 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
            drawPetiteBarbule(pnt,south, si,co, i,j, b1-12);
        }
        else if (vkn < 45) { // 40 ktn
            drawGrandeBarbule(pnt,south, si,co, i,j, b1);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-12);
        }
        else if (vkn < 55) { // 50 ktn
            drawTriangle(pnt,south, si,co, i,j, b1-4);
        }
        else if (vkn < 65) { // 60 ktn
            drawTriangle(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
        }
        else if (vkn < 75) { // 70 ktn
            drawTriangle(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-12);
        }
        else if (vkn < 85) { // 80 ktn
            drawTriangle(pnt,south, si,co, i,j, b1-4);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-8);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-12);
            drawGrandeBarbule(pnt,south, si,co, i,j, b1-16);
        }
        else { // > 90 ktn
            drawTriangle(pnt,south, si,co, i,j, b1-4);
            drawTriangle(pnt,south, si,co, i,j, b1-12);
        }
    }
}
//---------------------------------------------------------------
void GriddedPlotter::drawPetiteBarbule(QPainter &pnt, bool south,
                    double si, double co, int di, int dj, int b)
{
    if (south)
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+2, -5);
    else
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+2, 5);
}
//---------------------------------------------------------------
void GriddedPlotter::drawGrandeBarbule(QPainter &pnt, bool south,
                    double si, double co, int di, int dj, int b)
{
    if (south)
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+4,-10);
    else
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+4,10);
}
//---------------------------------------------------------------
void GriddedPlotter::drawTriangle(QPainter &pnt, bool south,
                    double si, double co, int di, int dj, int b)
{
    if (south) {
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+4,-10);
        drawTransformedLine(pnt, si,co, di,dj,  b+8,0,  b+4,-10);
    }
    else {
        drawTransformedLine(pnt, si,co, di,dj,  b,0,  b+4,10);
        drawTransformedLine(pnt, si,co, di,dj,  b+8,0,  b+4,10);
    }
}

//==========================================================================
// Rectangle translucide sur la zone couverte par les données
void GriddedPlotter::draw_CoveredZone 
						(QPainter &pnt, const Projection *proj)
{
	GriddedReader *reader = getReader ();
    if (reader == nullptr) {
        return;
    }   
    double x0,y0, x1,y1;
    int i, j, k,l;
    if (reader->getZoneExtension(&x0,&y0, &x1,&y1))
    {
        pnt.setPen(QColor(120, 120,120));
        pnt.setBrush(QColor(255,255,255,40));
        proj->map2screen(x0,y0, &i, &j);
        proj->map2screen(x1,y1, &k, &l);
        pnt.drawRect(i, j, k-i, l-j);
        proj->map2screen(x0-360.0,y0, &i, &j);
        proj->map2screen(x1-360.0,y1, &k, &l);
        pnt.drawRect(i, j, k-i, l-j);
    }
}

//==========================================================================
// draw colored map

//--------------------------------------------------------------------------
// Carte de couleurs générique en dimension 1
//--------------------------------------------------------------------------
void  GriddedPlotter::drawColorMapGeneric_1D (
		QPainter &pnt, const Projection *proj, bool smooth,
		DataCode dtc,
		QRgb (DataColors::*function_getColor) (double v, bool smooth)
	)
{
	//DBGQS (Util::formatDateTimeLong(currentDate)); 
    assert( getReader() != nullptr);
	GriddedRecord *rec = getReader()->getRecord (dtc, currentDate);
    if (rec == nullptr)
        return;
    int i, j;
    double x, y, v;
    int W = proj->getW();
    int H = proj->getH();
    QRgb   rgb;
    QImage *image = new QImage(W,H,QImage::Format_ARGB32);
    image->fill( qRgba(0,0,0,0));
    for (i=0; i<W-1; i+=2) {
        for (j=0; j<H-1; j+=2)
        {
            proj->screen2map(i,j, &x, &y);
            if (! rec->isXInMap(x))
                x += 360.0;    // tour complet ?
            if (rec->isPointInMap(x, y))
            {
                v = rec->getInterpolatedValue (dtc, x, y, mustInterpolateValues);
                if (GribDataIsDef(v))
                {
                    rgb = (this->*function_getColor) (v, smooth);
                    image->setPixel(i,  j, rgb);
                    image->setPixel(i+1,j, rgb);
                    image->setPixel(i,  j+1, rgb);
                    image->setPixel(i+1,j+1, rgb);
                }
            }
        }
    }
	pnt.drawImage(0,0,*image);
    delete image;
}
//--------------------------------------------------------------------------
// Carte de couleurs générique en dimension 2
//--------------------------------------------------------------------------
void  GriddedPlotter::drawColorMapGeneric_2D (
		QPainter &pnt, const Projection *proj, bool smooth,
		DataCode dtcX, DataCode dtcY, 
		QRgb (DataColors::*function_getColor) (double v, bool smooth)
	)
{
    assert( getReader() != nullptr);
	//DBGQS (Util::formatDateTimeLong(currentDate)); 
	GriddedRecord *recX = getReader()->getRecord (dtcX, currentDate);
	GriddedRecord *recY = getReader()->getRecord (dtcY, currentDate);
    if (recX == nullptr || recY == nullptr)
        return;
    int i, j;
    double x, y, vx, vy, v;
    int W = proj->getW();
    int H = proj->getH();
    QRgb   rgb;
    QImage *image = new QImage(W,H,QImage::Format_ARGB32);
    image->fill( qRgba(0,0,0,0));
    for (i=0; i<W-1; i+=2) {
        for (j=0; j<H-1; j+=2)
        {
            proj->screen2map(i,j, &x, &y);
            
            if (! recX->isXInMap(x))
                x += 360.0;    // tour complet ?
            if (recX->isPointInMap(x, y))
            {
                vx = recX->getInterpolatedValue (dtcX, x, y, mustInterpolateValues);
                vy = recY->getInterpolatedValue (dtcY, x, y, mustInterpolateValues);
				
                if (GribDataIsDef(vx) && GribDataIsDef(vy))
                {
                    v = sqrt(vx*vx+vy*vy);
                    rgb = (this->*function_getColor) (v, smooth);
                    image->setPixel(i,  j, rgb);
                    image->setPixel(i+1,j, rgb);
                    image->setPixel(i,  j+1, rgb);
                    image->setPixel(i+1,j+1, rgb);
                }
            }
        }
    }
	pnt.drawImage(0,0,*image);
    delete image;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void  GriddedPlotter::drawColorMapGeneric_Abs_Delta_2D (
		QPainter &pnt, const Projection *proj, bool smooth,
		DataCode dtcX, DataCode dtcY, DataCode dtc2,
		QRgb (DataColors::*function_getColor) (double v, bool smooth)
	)
{
    assert( getReader() != nullptr);
    GriddedRecord *recX = getReader()->getRecord (dtcX, currentDate);
    GriddedRecord *recY = getReader()->getRecord (dtcY, currentDate);
    if (recX == nullptr || recY == nullptr)
        return;

	GriddedRecord *rec2 = getReader()->getRecord (dtc2, currentDate);
    if (rec2 == nullptr )
        return;

    int i, j;
    double x, y, vx, vy, v, v2;
    int W = proj->getW();
    int H = proj->getH();
    QRgb   rgb;
    QImage *image = new QImage(W,H,QImage::Format_ARGB32);
    image->fill( qRgba(0,0,0,0));
    for (i=0; i<W-1; i+=2) {
        for (j=0; j<H-1; j+=2)
        {
            proj->screen2map(i,j, &x, &y);

            if (! recX->isXInMap(x))
                x += 360.0;    // tour complet ?
            if (recX->isPointInMap(x, y))
            {
                vx = recX->getInterpolatedValue (dtcX, x, y, mustInterpolateValues);
                vy = recY->getInterpolatedValue (dtcY, x, y, mustInterpolateValues);
                v2 = rec2->getInterpolatedValue (dtc2, x, y, mustInterpolateValues);

                if (GribDataIsDef(vx) && GribDataIsDef(vy) && GribDataIsDef(v2))
                {
                    v = fabs(sqrt(vx*vx+vy*vy) -v2);
                    rgb = (this->*function_getColor) (v, smooth);
                    image->setPixel(i,  j, rgb);
                    image->setPixel(i+1,j, rgb);
                    image->setPixel(i,  j+1, rgb);
                    image->setPixel(i+1,j+1, rgb);
                }
            }
        }
    }
	pnt.drawImage(0,0,*image);
    delete image;
}
//--------------------------------------------------------------------------
// Carte de couleurs générique de la différence entre 2 champs
//--------------------------------------------------------------------------
void  GriddedPlotter::drawColorMapGeneric_Abs_Delta_Data (
		QPainter &pnt, const Projection *proj, bool smooth,
		DataCode dtc1, DataCode dtc2, 
		QRgb (DataColors::*function_getColor) (double v, bool smooth)
	)
{
    assert( getReader() != nullptr);
	GriddedRecord *rec1 = getReader()->getRecord (dtc1, currentDate);
	GriddedRecord *rec2 = getReader()->getRecord (dtc2, currentDate);
    if (rec1 == nullptr || rec2 == nullptr )
        return;
    int i, j;
    double x, y, vx, vy, v;
    int W = proj->getW();
    int H = proj->getH();
    QRgb   rgb;
    QImage *image = new QImage(W,H,QImage::Format_ARGB32);
    image->fill( qRgba(0,0,0,0));
    for (i=0; i<W-1; i+=2) {
        for (j=0; j<H-1; j+=2)
        {
            proj->screen2map(i,j, &x, &y);
            
            if (! rec1->isXInMap(x))
                x += 360.0;    // tour complet ?
            if (rec1->isPointInMap(x, y))
            {
                vx = rec1->getInterpolatedValue (dtc1, x, y, mustInterpolateValues);
                vy = rec2->getInterpolatedValue (dtc2, x, y, mustInterpolateValues);

                if (GribDataIsDef(vx) && GribDataIsDef(vy))
                {
                    v = fabs(vx-vy);
                    rgb = (this->*function_getColor) (v, smooth);
                    image->setPixel(i,  j, rgb);
                    image->setPixel(i+1,j, rgb);
                    image->setPixel(i,  j+1, rgb);
                    image->setPixel(i+1,j+1, rgb);
                }
            }
        }
    }
	pnt.drawImage(0,0,*image);
    delete image;
}


//-------------------------------------------------------------------------
void GriddedPlotter::draw_listIsolines (
						std::vector <IsoLine *> & listIsolines,
						QPainter &pnt, const Projection *proj)
{
    for(auto & listIsoline : listIsolines)
    {
        listIsoline->drawIsoLine (pnt, proj);
    }
}
//--------------------------------------------------------------------------
void GriddedPlotter::draw_listIsolines_labels (
						std::vector <IsoLine *>  & listIsolines,
						double coef,
						double offset,
						QColor &color,
						QPainter &pnt, const Projection *proj, 
						int density 	// default -1
					)
{
    int nbseg = 0;
    for(auto & listIsoline : listIsolines)
    {
        nbseg += listIsoline->getNbSegments();
    }
    int nbpix, first;
	nbpix = proj->getW()*proj->getH();
	if (nbpix == 0)
		return;
	if (density <= 0) {
		double r = (double)nbseg/nbpix *1000;
		double dens = 10;
		density =  (int) (r*dens +0.5);
		if (density < 20)
			density = 20;
	}
    first = 0; 
    for(auto & listIsoline : listIsolines)
    {
        first += 20;
        listIsoline->drawIsoLineLabels (pnt, color, proj, density, first, coef,offset);
    }
}
//----------------------------------------------------
void GriddedPlotter::complete_listIsolines (
				std::vector <IsoLine *> *listIsolines,
				DataCode dtc,
				double dataMin, double dataMax, double dataStep, 
				const Projection *proj
) {
    Util::cleanVectorPointers (*listIsolines);    
	GriddedReader *reader = getReader ();
    if (reader == nullptr)
        return;

    GriddedRecord *rec = reader->getRecord (dtc, currentDate);
    if (rec == nullptr)
        return;

    int deltaI, deltaJ;
    analyseVisibleGridDensity (proj, rec, 16, &deltaI, &deltaJ);
	//DBG("deltaI=%d deltaJ=%d", deltaI, deltaJ);
	IsoLine *iso;
	for (double val=dataMin; val<=dataMax; val += dataStep)
	{
		iso = new IsoLine (dtc, val, rec, deltaI, deltaJ);
        if (iso->getNbSegments()>0)
            listIsolines->push_back (iso);
        else
            delete iso;
	}
}
//-----------------------------------------------------------------
void GriddedPlotter::analyseVisibleGridDensity 
		(const Projection *proj, GriddedRecord *rec, 
		 double coef, int *deltaI, int *deltaJ
) {
    int i0, j0, i1, j1;
	double x0,y0, x1,y1;
	i0 = proj->getW()/2;
	j0 = proj->getH()/2;
	i1 = i0+1;
	j1 = j0+1;
	proj->screen2map (i0,j0, &x0,&y0);
	proj->screen2map (i1,j1, &x1,&y1);
	double dx = fabs(x1-x0);
	double dy = fabs(y1-y0);
	double densx = dx/rec->getDeltaX();
	double densy = dy/rec->getDeltaY();
	//DBG("dx=%g  dy=%g",  dx,dy);
	//DBG("densx=%g  densy=%g",  densx,densy);
	*deltaI = (int)(coef*densx); // proportional to the number of grid points per pixel
	*deltaJ = (int)(coef*densy);
	if (*deltaI < 1)
		*deltaI = 1;
	if (*deltaJ < 1)
		*deltaJ = 1;
}
//======================================================================
void GriddedPlotter::draw_DATA_Labels (
				DataCode dtc, 
                const QFont 	&labelsFont,
                const QColor   &labelsColor,
				QString  (formatLabelFunction) (float v, bool withUnit),
				QPainter &pnt, const Projection *proj)
{
	GriddedReader *reader = getReader();
    if (reader == nullptr)
        return;

    GriddedRecord *rec = reader->getRecord (dtc, currentDate);
	if (rec == nullptr)
		return;

    QFontMetrics fmet (labelsFont);
    pnt.setFont (labelsFont);
    pnt.setPen  (labelsColor);

	double x, y, v;
    int i, j, dimin, djmin;
    dimin = 50;
    djmin = 30;
    for (j=0; j<proj->getH(); j+= djmin) {
        for (i=0; i<proj->getW(); i+= dimin) {
            proj->screen2map(i,j, &x,&y);
            v = rec->getInterpolatedValue (dtc, x, y, mustInterpolateValues);
            if (GribDataIsDef(v)) {
                QString strtemp = formatLabelFunction (v,false);
                pnt.drawText(i-fmet.width("XXX")/2, j+fmet.ascent()/2, strtemp);
            }
        }
    } 
} 
//------------------------------------------------------------
void GriddedPlotter::draw_DATA_MinMax (
						DataCode dtc, 
						double   meanValue,
                        const QString  &minSymbol,
                        const QString  &maxSymbol,
                        const QFont 	 &labelsFont,
                        const QColor   &labelsColor,
						QPainter &pnt, const Projection *proj)
{
	GriddedReader *reader = getReader();
    if (reader == nullptr)
        return;

    GriddedRecord *rec = reader->getRecord (dtc, currentDate);
	if (rec == nullptr)
		return;

    QFontMetrics fmet (labelsFont);
    pnt.setFont (labelsFont);
    pnt.setPen  (labelsColor);

    int i, j, Ni, Nj, pi,pj;
    double x, y, v;
    double q1savLv = 9999999.9;
    double q2savLv = 9999999.9;
    double q3savLv = 9999999.9;
    double q4savLv = 9999999.9;

    double q1savHv = 0.0;
    double q2savHv = 0.0;
    double q3savHv = 0.0;
    double q4savHv = 0.0;

    double q1savHx, q1savHy, q1savLx, q1savLy;
    double q2savHx, q2savHy, q2savLx, q2savLy;
    double q3savHx, q3savHy, q3savLx, q3savLy;
    double q4savHx, q4savHy, q4savLx, q4savLy;

    Ni = rec->getNi();
    Nj = rec->getNj();

    for (j=1; j<Nj-1; j++) {     // !!!! 1 to end-1
        for (i=1; i<Ni-1; i++) {
            v = rec->getValueOnRegularGrid (dtc, i, j );
            if ( v <= meanValue
                   && v < rec->getValueOnRegularGrid (dtc, i-1, j-1 )  // Minima local ?
                   && v < rec->getValueOnRegularGrid (dtc, i-1, j   )
                   && v < rec->getValueOnRegularGrid (dtc, i-1, j+1 )
                   && v < rec->getValueOnRegularGrid (dtc, i  , j-1 )
                   && v < rec->getValueOnRegularGrid (dtc, i  , j+1 )
                   && v < rec->getValueOnRegularGrid (dtc, i+1, j-1 )
                   && v < rec->getValueOnRegularGrid (dtc, i+1, j   )
                   && v < rec->getValueOnRegularGrid (dtc, i+1, j+1 )
            ) {
                rec->getXY(i, j, &x, &y);

                // in which quarter of the grid are we
                if (i <= Ni/2 && j <= Nj/2) {
                    // if we found a record low for the quarter - we save it
                    if (v < q1savLv) {
                        q1savLv = v;
                        q1savLx = x;
                        q1savLy = y;
                    }
                } else if (i <= Ni/2 && j > Nj/2) {
                    // if we found a record low for the quarter - we save it
                    if (v < q2savLv) {
                        q2savLv = v;
                        q2savLx = x;
                        q2savLy = y;
                    }
                } else if (i > Ni/2 && j <= Nj/2) {
                    // if we found a record low for the quarter - we save it
                    if (v < q3savLv) {
                        q3savLv = v;
                        q3savLx = x;
                        q3savLy = y;
                    }
                } else if (i > Ni/2 && j > Nj/2) {
                    // if we found a record low for the quarter - we save it
                    if (v < q4savLv) {
                        q4savLv = v;
                        q4savLx = x;
                        q4savLy = y;
                    }
                }

            }
            else if ( v > meanValue
                   && v > rec->getValueOnRegularGrid (dtc, i-1, j-1 )  // Maxima local ?
                   && v > rec->getValueOnRegularGrid (dtc, i-1, j   )
                   && v > rec->getValueOnRegularGrid (dtc, i-1, j+1 )
                   && v > rec->getValueOnRegularGrid (dtc, i  , j-1 )
                   && v > rec->getValueOnRegularGrid (dtc, i  , j+1 )
                   && v > rec->getValueOnRegularGrid (dtc, i+1, j-1 )
                   && v > rec->getValueOnRegularGrid (dtc, i+1, j   )
                   && v > rec->getValueOnRegularGrid (dtc, i+1, j+1 )
            ) {
                rec->getXY(i, j, &x, &y);

                // in which quarter of the grid are we
                if (i <= Ni/2 && j <= Nj/2) {
                    // if we found a record high for the quarter - we save it
                    if (v > q1savHv) {
                        q1savHv = v;
                        q1savHx = x;
                        q1savHy = y;
                    }
                } else if (i <= Ni/2 && j > Nj/2) {
                    // if we found a record high for the quarter - we save it
                    if (v > q2savHv) {
                        q2savHv = v;
                        q2savHx = x;
                        q2savHy = y;
                    }
                } else if (i > Ni/2 && j <= Nj/2) {
                    // if we found a record high for the quarter - we save it
                    if (v > q3savHv) {
                        q3savHv = v;
                        q3savHx = x;
                        q3savHy = y;
                    }
                } else if (i > Ni/2 && j > Nj/2) {
                    // if we found a record high for the quarter - we save it
                    if (v > q4savHv) {
                        q4savHv = v;
                        q4savHx = x;
                        q4savHy = y;
                    }
                }

            }
        }
    }
    // now display the maxima and minima for each quarter
    if (q1savLv < 9999999.9) {
        proj->map2screen(q1savLx, q1savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
        proj->map2screen(q1savLx-360.0,q1savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
    }
    if (q1savHv > 0.0) {
        proj->map2screen(q1savHx, q1savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
        proj->map2screen(q1savHx-360.0,q1savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
    }
    // next quarter
    if (q2savLv < 9999999.9) {
        proj->map2screen(q2savLx, q2savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
        proj->map2screen(q2savLx-360.0,q2savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
    }
    if (q2savHv > 0.0) {
        proj->map2screen(q2savHx, q2savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
        proj->map2screen(q2savHx-360.0,q2savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
    }
    // next quarter
    if (q3savLv < 9999999.9) {
        proj->map2screen(q3savLx, q3savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
        proj->map2screen(q3savLx-360.0,q3savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
    }
    if (q3savHv > 0.0) {
        proj->map2screen(q3savHx, q3savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
        proj->map2screen(q3savHx-360.0,q3savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
    }
    // last quarter
    if (q4savLv < 9999999.9) {
        proj->map2screen(q4savLx, q4savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
        proj->map2screen(q4savLx-360.0,q4savLy, &pi, &pj);
        pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);;
    }
    if (q4savHv > 0.0) {
        proj->map2screen(q4savHx, q4savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
        proj->map2screen(q4savHx-360.0,q4savHy, &pi, &pj);
        pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
    }

 }

//------------------------------------------------------------
void GriddedPlotter::setCurrentDateClosestFromNow ()
{
	if (!isReaderOk())
        return;

	time_t date = getReader()->getClosestDateFromNow ();
	if (date > 0) {
		setCurrentDate (date);
	}
}
//------------------------------------------------------------
std::set<Altitude> GriddedPlotter::getAllAltitudes (int dataType) const
{
	std::set<Altitude> empty;
	if (!isReaderOk())
        return empty;

    return getReader()->getAllAltitudes (dataType);
}
//------------------------------------------------------------
std::set<DataCode> GriddedPlotter::getAllDataCode () const
{
	std::set<DataCode> empty;
	if (!isReaderOk())
        return empty;

    return getReader()->getAllDataCode ();
}
//------------------------------------------------------------
bool GriddedPlotter::hasWaveDataType (int dataType)  const
{ 
	if (! isReaderOk())
		return false;

    return getReader()->hasWaveDataType (dataType);
}


