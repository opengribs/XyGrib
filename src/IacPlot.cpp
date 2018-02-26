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

#include "IacPlot.h"
#include "Font.h"

//---------------------------------------------------
IacPlot::IacPlot()
{
	iacReader = NULL;
}

//---------------------------------------------------
IacPlot::IacPlot(QString fname)
{
	iacReader = NULL;
    loadFile(fname);
}

//---------------------------------------------------
void IacPlot::loadFile (QString fname)
{
	this->fileName = fname;
    if (iacReader != NULL) {
    	delete iacReader;
        iacReader = NULL;
    }

	iacReader = new IacReader (qPrintable(fileName));
    if (iacReader != NULL  &&  iacReader->isOk())
    {
		//iacReader->display_text();
	}
}
//---------------------------------------------------
void IacPlot::updateGraphicsParameters ()
{
}

//======================================================================
void  IacPlot::draw_PRESSURE_TroughLines (QPainter &pnt, Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }

	std::vector <IAC_TroughLine *>
			*lslines = iacReader->getList_PressureTroughLines();
	for (uint i=0; i<lslines->size(); i++)
	{
		IAC_TroughLine * trline= lslines->at(i);
		std::vector <IAC_Point *> lspoints = trline->points;
		drawIacLine(lspoints, true, pnt, proj);
    }
}
//======================================================================
void  IacPlot::draw_PRESSURE_IsobarsLabels (QPainter &pnt, Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }
    int   a,b;
    QString label="1XXX";

    QPen penText(QColor(0,0,0));
    QFont fontText = Font::getFont(FONT_IAC_Isobar);
    QFontMetrics fmet(fontText);
    QRect rect = fmet.boundingRect(label);
    pnt.setPen(penText);
    pnt.setFont(fontText);

	// use a gradient, because it's a bug sometimes with solid pattern (black background)
	QLinearGradient gradient;
    int r = 255;
	gradient.setColorAt(0, QColor(r,r,r, 140));
	gradient.setColorAt(1, QColor(r,r,r, 140));
    pnt.setBrush(gradient);

    //---------------------------------------------------------
    // Ecrit les labels
    //---------------------------------------------------------
	std::vector <IAC_Isobar *>
			*lslines = iacReader->getList_PressureIsobars();
	for (uint i=0; i<lslines->size(); i++)
	{
		IAC_Isobar * trline= lslines->at(i);
		std::vector <IAC_Point *> lspoints = trline->points;
		if (lspoints.size() > 1)
		{
    		label = label.sprintf("%d", (int)(trline->value+0.5));
    		rect = fmet.boundingRect(label);
			IAC_Point *pt = lspoints.at(lspoints.size()/2);
            proj->map2screen( pt->x, pt->y, &a, &b );
            rect.moveTo(a-rect.width()/2, b-rect.height()/2);
            pnt.drawRect(rect.x()-1, rect.y(), rect.width()+2, fmet.ascent()+2);
            pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, label);
            //
            proj->map2screen( pt->x-360, pt->y, &a, &b );
            rect.moveTo(a-rect.width()/2, b-rect.height()/2);
            pnt.drawRect(rect.x()-1, rect.y(), rect.width()+2, fmet.ascent()+2);
            pnt.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, label);
		}
    }
}
//======================================================================
void  IacPlot::draw_PRESSURE_Isobars (QPainter &pnt, Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }

	std::vector <IAC_Isobar *>
			*lslines = iacReader->getList_PressureIsobars();
	for (uint i=0; i<lslines->size(); i++)
	{
		IAC_Isobar * trline= lslines->at(i);
		std::vector <IAC_Point *> lspoints = trline->points;
		drawIacLine(lspoints, true, pnt, proj);
    }
}
//======================================================================
void  IacPlot::draw_PRESSURE_MinMax (QPainter &pnt, Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }

    int pi,pj;
    double x, y;
	QString HorL, tval;

	std::vector <IAC_HighLowPressurePoint *>
			*lspoints = iacReader->getList_HighLowPressurePoints();

    QFont fontPressureMinMax = Font::getFont(FONT_IAC_PressHL);
    QFont fontPressureMinMaxValue = Font::getFont(FONT_IAC_PressHLVal);
    QFontMetrics fmet(fontPressureMinMax);
    QFontMetrics fmetval(fontPressureMinMaxValue);

    pnt.setPen(QColor(0,0,0));

	for (uint i=0; i<lspoints->size(); i++)
	{
		IAC_HighLowPressurePoint * ptr= lspoints->at(i);
		x = ptr->x;
		y = ptr->y;
	    HorL = ptr->type.c_str();	            // 'H' or 'L' symbol
	    if (ptr->completevalue > 0)
	    	tval = tval.sprintf("%d", ptr->completevalue);  // Pressure value
		else
	    	tval = tval.sprintf("..%d", ptr->value);  // Pressure value

		proj->map2screen(x,y, &pi, &pj);
    	pnt.setFont(fontPressureMinMax);
		pnt.drawText(pi-fmet.width(HorL)/2, pj+fmet.ascent()/2, HorL);
    	pnt.setFont(fontPressureMinMaxValue);
		pnt.drawText(pi-fmetval.width(tval)/2, pj+fmet.ascent()+2, tval);

		proj->map2screen(x-360.0,y, &pi, &pj);   // tour du monde ?
    	pnt.setFont(fontPressureMinMax);
		pnt.drawText(pi-fmet.width(HorL)/2, pj+fmet.ascent()/2, HorL);
    	pnt.setFont(fontPressureMinMaxValue);
		pnt.drawText(pi-fmetval.width(tval)/2, pj+fmet.ascent()+2, tval);
	}

}

//==========================================================================
// Rectangle translucide sur la zone couverte par les données
void IacPlot::show_CoverZone(QPainter &pnt, const Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }

    double x0,y0, x1,y1;
    int i, j, k,l;
    if (iacReader->getZoneExtension(&x0,&y0, &x1,&y1))
    {
        pnt.setPen(QColor(120, 120,120));
        pnt.setBrush(QColor(255,255,255,160));
        proj->map2screen(x0,y0, &i, &j);
        proj->map2screen(x1,y1, &k, &l);
        pnt.drawRect(i, j, k-i, l-j);
        proj->map2screen(x0-360.0,y0, &i, &j);
        proj->map2screen(x1-360.0,y1, &k, &l);
        pnt.drawRect(i, j, k-i, l-j);
    }
}

//======================================================================
void  IacPlot::IacPlot::draw_FRONT_all (QPainter &pnt, Projection *proj)
{
    if (iacReader==NULL || !iacReader->isOk()) {
        return;
    }

	std::vector <IAC_Front *>
			*lsfronts = iacReader->getList_Fronts();
	for (uint i=0; i<lsfronts->size(); i++)
	{
		IAC_Front * front = lsfronts->at(i);
		QPen pen;
		double penw = 7;
		switch (front->type) {
			case IAC_Front::IAC_FRONT_COLD :
				pen.setColor(QColor(0,60,255));
				pen.setWidthF(penw);
				break;
			case IAC_Front::IAC_FRONT_WARM :
				pen.setColor(QColor(255,60,60));
				pen.setWidthF(penw);
				break;
			case IAC_Front::IAC_FRONT_OCCLUSION :
				pen.setColor(QColor(250,80,200));
				pen.setWidthF(penw);
				break;
			default :
				pen.setColor(QColor(200,200,200));
				pen.setWidthF(1);
				break;
		}
		pnt.setPen(pen);
		std::vector <IAC_Point *> lspoints = front->points;
		drawIacLine(lspoints, true, pnt, proj);
    }
}
//---------------------------------------------------------------------
void IacPlot::drawIacLine(std::vector <IAC_Point *> &lspoints, bool smooth,
							QPainter &pnt, Projection *proj)
{
	pnt.setBrush(Qt::NoBrush);

	if (lspoints.size() > 1)
	{
		if (lspoints.size() < 3) {
			smooth = false;
		}

		if (! smooth)
		{
			IAC_Point *prec = lspoints.at(0);
			for (uint j=1; j<lspoints.size(); j++)
			{
				IAC_Point *curr = lspoints.at(j);
				int i1,j1, i2,j2;
				proj->map2screen(prec->x,prec->y, &i1, &j1);
				proj->map2screen(curr->x,curr->y, &i2, &j2);
				pnt.drawLine(i1,j1, i2,j2);

				proj->map2screen(prec->x-360,prec->y, &i1, &j1);
				proj->map2screen(curr->x-360,curr->y, &i2, &j2);
				pnt.drawLine(i1,j1, i2,j2);

				prec = curr;
			}
		}
		else	// draw a bezier curve
		{
		    QPainterPath path;
			makeBezierPath(path, lspoints, 0, proj);
			makeBezierPath(path, lspoints, -360, proj);
			pnt.drawPath(path);
		}
	}
}
//---------------------------------------------------------------------
void IacPlot::makeBezierPath(QPainterPath &path,
							std::vector <IAC_Point *> &lspoints,
							double offsetX,
							Projection *proj)
{
	int i0, j0, i1,j1, i2,j2;
	int mi1,mj1, mi2,mj2;
	uint N = lspoints.size();

	proj->map2screen(lspoints.at(0)->x+offsetX, lspoints.at(0)->y, &i0, &j0);
	proj->map2screen(lspoints.at(1)->x+offsetX, lspoints.at(1)->y, &i1, &j1);
	mi1 = (i0+i1)/2;
	mj1 = (j0+j1)/2;
	path.moveTo(mi1,mj1);	// start = middle of first segment

	// Join with a bezier curve the middles of segments.
	// Control points are the ends of segments.
 	for (uint j=2; j<N; j++)
	{
		proj->map2screen(lspoints.at(j  )->x+offsetX, lspoints.at(j  )->y, &i2, &j2);
		mi1 = (i0+i1)/2;
		mj1 = (j0+j1)/2;
		mi2 = (i1+i2)/2;
		mj2 = (j1+j2)/2;
		path.cubicTo(i1,j1, i1,j1, mi2,mj2);
		i0 = i1;
		j0 = j1;
		i1 = i2;
		j1 = j2;
	}
	// Ends of lines
	if (     lspoints.at(0)->x == lspoints.at(N-1)->x
		  && lspoints.at(0)->y == lspoints.at(N-1)->y  )
	{
		// Closed curve : join middles of first and last segment
		proj->map2screen(lspoints.at(N-1)->x+offsetX, lspoints.at(N-1)->y, &i0, &j0);
		proj->map2screen(lspoints.at(0)->x+offsetX, lspoints.at(0)->y, &i1, &j1);
		proj->map2screen(lspoints.at(1)->x+offsetX, lspoints.at(1)->y, &i2, &j2);
		mi1 = (i0+i1)/2;
		mj1 = (j0+j1)/2;
		mi2 = (i1+i2)/2;
		mj2 = (j1+j2)/2;
		path.cubicTo(i1,j1, i1,j1, mi2,mj2);
	}
	else {
		// join start to middle of first segment
		proj->map2screen(lspoints.at(0)->x+offsetX, lspoints.at(0)->y, &i0, &j0);
		proj->map2screen(lspoints.at(1)->x+offsetX, lspoints.at(1)->y, &i1, &j1);
		mi1 = (i0+i1)/2;
		mj1 = (j0+j1)/2;
		path.moveTo(i0, j0);
		path.lineTo(mi1,mj1);
		// join middle of last segment to end
		proj->map2screen(lspoints.at(N-2)->x+offsetX, lspoints.at(N-2)->y, &i0, &j0);
		proj->map2screen(lspoints.at(N-1)->x+offsetX, lspoints.at(N-1)->y, &i1, &j1);
		mi1 = (i0+i1)/2;
		mj1 = (j0+j1)/2;
		path.moveTo(mi1,mj1);
		path.lineTo(i1, j1);
	}
}

