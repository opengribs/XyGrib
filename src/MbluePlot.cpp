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

#include "MbluePlot.h" 
#include "Font.h"
#include "DataColors.h"

//-------------------------------------------------------------------
MbluePlot::MbluePlot ()
{
	reader = NULL;
	windAltitude = Altitude (LV_ABOV_GND,10);
}
//-------------------------------------------------------------------
MbluePlot::~MbluePlot ()
{
// 	DBGS("Destroy MbluePlot");
	if (reader != NULL) {
		delete reader;
		reader = NULL;
	}
}
//--------------------------------------------------------------------
void  MbluePlot::loadFile (QString fname,
						   LongTaskProgress *taskProgress, int /*nbrecs*/)
{
	if (reader != NULL) {
		delete reader;
		reader = NULL;
	}
	reader = new MblueReader ();
    if (reader != NULL)
    {
		reader->openFile (qPrintable(fname), fastInterpolation, taskProgress);
		if (reader->isOk())
		{
			listDates = reader->getListDates();
			setCurrentDate ( listDates.size()>0 ? *(listDates.begin()) : 0);
		}
	}
}
//--------------------------------------------------------------------
/** Draw a mark at each grid vertex.
*/
void MbluePlot::draw_GridPoints ( const DataCode &,
				QPainter &pnt, const Projection *proj )
{
	if (!reader || !reader->isOk())
		return;

	MblueRecord *rec = reader->getMblueRecordByDate (currentDate);
	if (rec == NULL)
		return;
	
	std::vector <MbluePoint *> *lspoints = rec->getListPoints ();
	std::vector <MbluePoint *>::iterator itp;
	for (itp=lspoints->begin(); itp != lspoints->end(); itp++) {
		MbluePoint *pt = *itp;
		int px,py, dl=2;
		proj->map2screen(pt->x, pt->y, &px,&py);
		pnt.drawLine(px-dl,py, px+dl,py);
		pnt.drawLine(px,py-dl, px,py+dl);
		proj->map2screen(pt->x-360.0, pt->y, &px,&py);
		pnt.drawLine(px-dl,py, px+dl,py);
		pnt.drawLine(px,py-dl, px,py+dl);
	}
}
//--------------------------------------------------------------------
/** Draw wind arrows on the colored map.
*/
void MbluePlot::draw_WIND_Arrows (
		Altitude altitude, 
		bool barbules, QColor arrowsColor, 
		QPainter &pnt, const Projection *proj )
{
	if (!reader || !reader->isOk())
		return;
	MblueRecord *rec = reader->getMblueRecordByDate (currentDate);
	if (rec == NULL)
		return;
	
	windAltitude = altitude;
	
	int i, j;
    double x, y, vx, vy;
    int W = proj->getW();
    int H = proj->getH();
    
	int space;    
    if (barbules)
	    space =  drawWindArrowsOnGrid ? windBarbuleSpaceOnGrid : windBarbuleSpace;
    else
	    space =  drawWindArrowsOnGrid ? windArrowSpaceOnGrid : windArrowSpace;

	if (drawWindArrowsOnGrid)
    {	// Flèches uniquement sur les points de la grille
		std::vector <MbluePoint *> *lspoints = rec->getListPoints ();
		std::vector <MbluePoint *>::iterator itp;
		for (itp=lspoints->begin(); itp != lspoints->end(); itp++) {
			MbluePoint *pt = *itp;
			x = pt->x;
			y = pt->y;
			if (! rec->isXInMap(x))
				x += 360.0;   // tour du monde ?
			if (rec->isPointInMap(x,y)) {
				vx = pt->getValue (DataCode (GRB_WIND_VX,windAltitude));
				vy = pt->getValue (DataCode (GRB_WIND_VY,windAltitude));
				proj->map2screen (x,y, &i,&j);
				if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
				{
					if (barbules)
						drawWindArrowWithBarbs(pnt, i,j, vx,vy, (y<0), arrowsColor);
					else
						drawWindArrow(pnt, i,j, vx,vy);
				}
			}
		}
	}
	else
    {	// Flèches uniformément réparties sur l'écran
		for (i=0; i<W; i+=space) {
			for (j=0; j<H; j+=space) {
				proj->screen2map (i,j, &x,&y);
				if (! rec->isXInMap(x))
					x += 360.0;   // tour du monde ?
				if (rec->isPointInMap(x,y)) {
					vx = rec->getInterpolatedValue (
								DataCode (GRB_WIND_VX,windAltitude), 
								x, y, mustInterpolateValues);
					vy = rec->getInterpolatedValue (
								DataCode (GRB_WIND_VY,windAltitude), 
								x, y, mustInterpolateValues);
					if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
					{
						if (barbules)
							drawWindArrowWithBarbs(pnt, i,j, vx,vy, (y<0), arrowsColor);
						else
							drawWindArrow(pnt, i,j, vx,vy);
					}
				}
			}
		}
	}
	
}

//--------------------------------------------------------------------
/** Draw current arrows on the colored map.
*/
void MbluePlot::draw_CURRENT_Arrows (
		Altitude altitude, 
		QColor /*arrowsColor*/, 
		QPainter &pnt, const Projection *proj )
{
	if (!reader || !reader->isOk())
		return;
	if (altitude.levelType!=LV_GND_SURF || altitude.levelValue!=0)
		return;
	MblueRecord *rec = reader->getMblueRecordByDate (currentDate);
	if (rec == NULL)
		return;
	
	currentAltitude = altitude;
	
	int i, j;
    double x, y, cx, cy;
    int W = proj->getW();
    int H = proj->getH();
    
	int space;    
 
	space =  drawCurrentArrowsOnGrid ? currentArrowSpaceOnGrid : currentArrowSpace;

	if (drawCurrentArrowsOnGrid)
    {	// Flèches uniquement sur les points de la grille
		std::vector <MbluePoint *> *lspoints = rec->getListPoints ();
		std::vector <MbluePoint *>::iterator itp;
		for (itp=lspoints->begin(); itp != lspoints->end(); itp++) {
			MbluePoint *pt = *itp;
			x = pt->x;
			y = pt->y;
			if (! rec->isXInMap(x))
				x += 360.0;   // tour du monde ?
			if (rec->isPointInMap(x,y)) {
				cx = pt->getValue (DataCode (GRB_CUR_VX,LV_ABOV_GND,10));
				cy = pt->getValue (DataCode (GRB_CUR_VY,LV_ABOV_GND,10));
				proj->map2screen (x,y, &i,&j);
				if (cx != GRIB_NOTDEF && cy != GRIB_NOTDEF)
				{
					drawWindArrow(pnt, i,j, cx,cy);
				}
			}
		}
	}
	else
    {	// Flèches uniformément réparties sur l'écran
		for (i=0; i<W; i+=space) {
			for (j=0; j<H; j+=space) {
				proj->screen2map (i,j, &x,&y);
				if (! rec->isXInMap(x))
					x += 360.0;   // tour du monde ?
				if (rec->isPointInMap(x,y)) {
					cx = rec->getInterpolatedValue (
								DataCode (GRB_CUR_VX,LV_ABOV_GND,10), 
								x, y, mustInterpolateValues);
					cy = rec->getInterpolatedValue (
								DataCode (GRB_CUR_VY,LV_ABOV_GND,10), 
								x, y, mustInterpolateValues);
					if (cx != GRIB_NOTDEF && cy != GRIB_NOTDEF)
					{
							drawWindArrow(pnt, i,j, cx,cy);
					}
				}
			}
		}
	}
	
}

//--------------------------------------------------------------------
/** Draw the complete colored map.
*/
void MbluePlot::draw_ColoredMapPlain ( 
				DataCode dtc,
				bool smooth,
				QPainter &pnt, 
				const Projection *proj )
{
    if (reader == NULL) {
        return;
    }
	MblueRecord *rec = reader->getMblueRecordByDate (currentDate);
	if (rec == NULL)
		return;
	
	if (dtc.dataType == GRB_PRV_WIND_JET) {
		dtc.dataType = GRB_PRV_WIND_XY2D;
	}
	DataCode dtc2 = dtc;
	if (useJetStreamColorMap && dtc.dataType==GRB_PRV_WIND_XY2D) {
		dtc2.dataType = GRB_PRV_WIND_JET;
	}
	DataColors::setColorDataTypeFunction (dtc2);

	switch (dtc.dataType) {
		case GRB_PRV_WIND_XY2D :
			windAltitude = dtc.getAltitude ();
			drawColorMapGeneric_2D (pnt,proj,smooth, 
						DataCode (GRB_WIND_VX, dtc.levelType,dtc.levelValue),
						DataCode (GRB_WIND_VY, dtc.levelType,dtc.levelValue), 
						DataColors::function_getColor ); 
		case GRB_PRV_CUR_XY2D :
			currentAltitude = dtc.getAltitude ();
			drawColorMapGeneric_2D (pnt,proj,smooth, 
						DataCode (GRB_CUR_VX, dtc.levelType,dtc.levelValue),
						DataCode (GRB_CUR_VY, dtc.levelType,dtc.levelValue), 
						DataColors::function_getColor ); 
			break;
		case GRB_PRV_DIFF_TEMPDEW :
			drawColorMapGeneric_Abs_Delta_Data (pnt,proj,smooth, 
						DataCode (GRB_TEMP, dtc.levelType,dtc.levelValue), 
						DataCode (GRB_DEWPOINT, dtc.levelType,dtc.levelValue), 
						DataColors::function_getColor );
			break;
		case GRB_TEMP :
		case GRB_CLOUD_TOT :
		case GRB_PRECIP_TOT :
		case GRB_HUMID_REL :
		case GRB_DEWPOINT :
			drawColorMapGeneric_1D (pnt,proj,smooth, dtc, DataColors::function_getColor);
			break;
		default :
			break;
	}

}
//-------------------------------------------------------------------------
void MbluePlot::draw_DATA_MinMax (
						DataCode dtc, 
						double   meanValue,
						QString  minSymbol,
						QString  maxSymbol,
						QFont 	 labelsFont,
						QColor   labelsColor,
						QPainter &pnt, const Projection *proj)
{
	GriddedReader *reader = getReader();
    if (reader == NULL)
        return;
	GriddedRecord *rec = reader->getRecord (dtc, currentDate);
	if (rec == NULL)
		return;
	MblueRecord *recMB = dynamic_cast <MblueRecord *> (rec);
	
    QFontMetrics fmet (labelsFont);
    pnt.setFont (labelsFont);
    pnt.setPen  (labelsColor);

    int i, j, Ni, Nj, pi,pj;
    double x, y, v;
	
    Ni = recMB->getNi();
    Nj = recMB->getNj();

	#define getFilteredValue(i,j) (recMB->getSmoothPressureMSL((i),(j)))
    
    for (j=2; j<Nj-2; j+=2) {
        for (i=2; i<Ni-2; i+=2) {
            v = getFilteredValue (i, j );
            if ( v < meanValue
                   && v < getFilteredValue (i-2, j-2 )  // Minima local ?
                   && v < getFilteredValue (i-2, j   )
                   && v < getFilteredValue (i-2, j+2 )
                   && v < getFilteredValue (i  , j-2 )
                   && v < getFilteredValue (i  , j+2 )
                   && v < getFilteredValue (i+2, j-2 )
                   && v < getFilteredValue (i+2, j   )
                   && v < getFilteredValue (i+2, j+2 )
            ) {
                x = rec->getX(i);
                y = rec->getY(j);
                proj->map2screen(x,y, &pi, &pj);
                pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);
                proj->map2screen(x-360.0,y, &pi, &pj);
                pnt.drawText(pi-fmet.width('L')/2, pj+fmet.ascent()/2, minSymbol);
            }
            else if ( v > meanValue
                   && v > getFilteredValue (i-2, j-2 )  // Maxima local ?
                   && v > getFilteredValue (i-2, j   )
                   && v > getFilteredValue (i-2, j+2 )
                   && v > getFilteredValue (i  , j-2 )
                   && v > getFilteredValue (i  , j+2 )
                   && v > getFilteredValue (i+2, j-2 )
                   && v > getFilteredValue (i+2, j   )
                   && v > getFilteredValue (i+2, j+2 )
            ) {
                x = rec->getX(i);
                y = rec->getY(j);
                proj->map2screen(x,y, &pi, &pj);
                pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
                proj->map2screen(x-360.0,y, &pi, &pj);
                pnt.drawText(pi-fmet.width('H')/2, pj+fmet.ascent()/2, maxSymbol);
            }
        }
    }

}







