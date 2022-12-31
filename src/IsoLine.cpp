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

#include "IsoLine.h"
#include "Font.h"

//---------------------------------------------------------------
IsoLine::IsoLine (double val, GriddedRecord *rec, int deltaI, int deltaJ)
{
    this->rec    = rec;
    this->value  = val;
	
    W = rec->getNi();
    H = rec->getNj();
    int gr = 80;
    isoLineColor = QColor(gr,gr,gr);
    //---------------------------------------------------------
    // Génère la liste des segments.
    extractIsoLine (rec, deltaI, deltaJ);
    //extractIsoLine (rec, 1, 1);
}
//---------------------------------------------------------------
IsoLine::~IsoLine()
{
    for (auto & it : trace) {
        delete it;
        it = nullptr;
    }
    trace.clear();
}

//---------------------------------------------------------------
void IsoLine::drawIsoLine (QPainter &pnt,
                            const Projection *proj)
{
    int   a,b,c,d;
	pnt.setRenderHint(QPainter::Antialiasing, true);

    //---------------------------------------------------------
    // Dessine les segments
    //---------------------------------------------------------
    for (auto const &seg :trace)
    {
        // Teste la visibilité (bug clipping sous windows avec pen.setWidthF())
        if ( proj->isPointVisible(seg->px1, seg->py1)
                 ||    proj->isPointVisible(seg->px2, seg->py2))
        {
            proj->map2screen( seg->px1, seg->py1, &a, &b );
            proj->map2screen( seg->px2, seg->py2, &c, &d );
            pnt.drawLine(a,b, c,d);
        }
        // tour du monde ?
        if ( proj->isPointVisible(seg->px1-360.0, seg->py1)
                 ||    proj->isPointVisible(seg->px2-360.0, seg->py2))
        {
            proj->map2screen( seg->px1-360.0, seg->py1, &a, &b );
            proj->map2screen( seg->px2-360.0, seg->py2, &c, &d );
            pnt.drawLine(a,b, c,d);
        }
    }
}

//---------------------------------------------------------------
void IsoLine::drawIsoLineLabels(QPainter &pnt, std::vector <QRect> &overlap, 
                            QColor &couleur, const Projection *proj,
                            int density, int first, double coef,double offset)
{
    int   a,b,c,d;
    int nb = first;
    QString label;

    label = QString::asprintf("%d", qRound(value*coef+offset));

    QPen penText(couleur);
    QFont fontText = Font::getFont(FONT_IsolineLabel);
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
    // Ecrit les labels
    //---------------------------------------------------------
    for (auto it=trace.begin(); it!=trace.end(); ++it,nb++)
    {
        if (nb % density == 0) {
            Segment *seg = *it;
    		rect = fmet.boundingRect(label);
            proj->map2screen( seg->px1, seg->py1, &a, &b );
            proj->map2screen( seg->px2, seg->py2, &c, &d );
            rect.moveTo((a+c)/2-rect.width()/2, (b+d)/2-rect.height()/2);
            bool o = false;
            // XXX Bad, linear search
            for (auto const &r : overlap) {
                if (r.intersects(rect)) {
                    o = true;
                    break;
                }
            }
            if (!o) {
                pnt.drawRect(rect.x()-1, rect.y(), rect.width()+2, fmet.ascent()+2);
                pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, label);
                overlap.push_back({rect.x() -rect.width()/2, rect.y() -rect.height()/2, 
                    rect.width()*2, rect.height()*2});
                // tour du monde ?
                proj->map2screen( seg->px1-360.0, seg->py1, &a, &b );
                proj->map2screen( seg->px2-360.0, seg->py2, &c, &d );
                rect.moveTo((a+c)/2-rect.width()/2, (b+d)/2-rect.height()/2);
                pnt.drawRect(rect.x()-1, rect.y(), rect.width()+2, fmet.ascent()+2);
                pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, label);
            }
        }
    }
}
//==================================================================================
// Segment
//==================================================================================
Segment::Segment ( int I, int J,
				char c1, char c2, char c3, char c4,
				GriddedRecord *rec, double val,
				int deltaI, int deltaJ)
{
	this->deltaI = deltaI;
	this->deltaJ = deltaJ;
    traduitCode(I,J, c1, i,j);
    traduitCode(I,J, c2, k,l);
    traduitCode(I,J, c3, m,n);
    traduitCode(I,J, c4, o,p);

    intersectionAreteGrille (i,j, k,l,  &px1,&py1, rec, val);
    intersectionAreteGrille (m,n, o,p,  &px2,&py2, rec, val);
}
//-----------------------------------------------------------------------
void Segment::intersectionAreteGrille (
					int i,int j, int k,int l,
					double *x, double *y,
					GriddedRecord *rec, double val)
{
    double xa, xb, ya, yb, pa, pb, dec;

    pa = rec->getValueOnRegularGrid (i,j);
    pb = rec->getValueOnRegularGrid (k,l);

    // Abscisse
    rec->getXY(i, j, &xa, &ya);
    rec->getXY(k, l, &xb, &yb);

    if (pb != pa)
        dec = (val-pa)/(pb-pa);
    else
        dec = 0.5;
    if (fabs(dec)>1)
        dec = 0.5;
    *x = xa+(xb-xa)*dec;

    // Ordonnée
    if (pb != pa)
        dec = (val-pa)/(pb-pa);
    else
        dec = 0.5;
    if (fabs(dec)>1)
        dec = 0.5;
    *y = ya+(yb-ya)*dec;
}
//---------------------------------------------------------------
void Segment::traduitCode (int I, int J, char c1, int &i, int &j) {
    switch (c1) {
        case 'a':  i=I-deltaI; j=J-deltaJ; break;
        case 'b':  i=I;        j=J-deltaJ; break;
        case 'c':  i=I-deltaI; j=J; break;
        case 'd':  i=I;        j=J; break;
        default:   i=I;        j=J;
    }
}

//-----------------------------------------------------------------------
// Génère la liste des segments. XXXX
// Les coordonnées sont les indices dans la grille du GriddedRecord
//---------------------------------------------------------
void IsoLine::extractIsoLine (GriddedRecord *rec, int deltaI, int deltaJ)
{
    int i, j, W, H;
    double a,b,c,d;
    W = rec->getNi();
    H = rec->getNj();

	for (j=deltaI; j<H; j+=deltaJ)     // !!!! 1 to end
    {
        a = rec->getValueOnRegularGrid (deltaI, j-deltaJ );
        c = rec->getValueOnRegularGrid (deltaI, j  );
        for (i=deltaI; i<W; i+=deltaI, a =b, c = d )
        {
            b = rec->getValueOnRegularGrid (i,        j-deltaJ);
            d = rec->getValueOnRegularGrid (i,        j  );

            if( a == GRIB_NOTDEF || b == GRIB_NOTDEF || c == GRIB_NOTDEF || d == GRIB_NOTDEF ) continue;

            if ((a< value && b< value && c< value  && d < value)
                 || (a>value && b>value && c>value  && d > value))
                continue;
            // Détermine si 1 ou 2 segments traversent la case ab-cd
            // a  b
            // c  d
            //--------------------------------
            // 1 segment en diagonale
            //--------------------------------
            if     ((a<=value && b<=value && c<=value  && d>value)
                 || (a>value && b>value && c>value  && d<=value))
                trace.push_back(new Segment (i,j, 'c','d',  'b','d',rec,value,deltaI,deltaJ));
            else if ((a<=value && c<=value && d<=value  && b>value)
                 || (a>value && c>value && d>value  && b<=value))
                trace.push_back(new Segment (i,j, 'a','b',  'b','d',rec,value,deltaI,deltaJ));
            else if ((c<=value && d<=value && b<=value  && a>value)
                 || (c>value && d>value && b>value  && a<=value))
                trace.push_back(new Segment (i,j, 'a','b',  'a','c',rec,value,deltaI,deltaJ));
            else if ((a<=value && b<=value && d<=value  && c>value)
                 || (a>value && b>value && d>value  && c<=value))
                trace.push_back(new Segment (i,j, 'a','c',  'c','d',rec,value,deltaI,deltaJ));
            //--------------------------------
            // 1 segment H ou V
            //--------------------------------
            else if ((a<=value && b<=value   &&  c>value && d>value)
                 || (a>value && b>value   &&  c<=value && d<=value))
                trace.push_back(new Segment (i,j, 'a','c',  'b','d',rec,value,deltaI,deltaJ));
            else if ((a<=value && c<=value   &&  b>value && d>value)
                 || (a>value && c>value   &&  b<=value && d<=value))
                trace.push_back(new Segment (i,j, 'a','b',  'c','d',rec,value,deltaI,deltaJ));
            //--------------------------------
            // 2 segments en diagonale
            //--------------------------------
            else if  (a<=value && d<=value   &&  c>value && b>value) {
                trace.push_back(new Segment (i,j, 'a','b',  'b','d',rec,value,deltaI,deltaJ));
                trace.push_back(new Segment (i,j, 'a','c',  'c','d',rec,value,deltaI,deltaJ));
            }
            else if  (a>value && d>value   &&  c<=value && b<=value) {
                trace.push_back(new Segment (i,j, 'a','b',  'a','c',rec,value,deltaI,deltaJ));
                trace.push_back(new Segment (i,j, 'b','d',  'c','d',rec,value,deltaI,deltaJ));
            }

        }
    }
}
