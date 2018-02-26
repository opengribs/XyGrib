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

#include "GribPlot.h"
#include <QMessageBox>
#include <QObject>

#include "Font.h"

//----------------------------------------------------
GribPlot::GribPlot()
{
	initNewGribPlot();
}
//----------------------------------------------------
GribPlot::GribPlot (const GribPlot &model)
	: RegularGridPlot ()
{
	initNewGribPlot (model.mustInterpolateValues, model.drawWindArrowsOnGrid, model.drawCurrentArrowsOnGrid);	
	loadFile (model.fileName);
	duplicateFirstCumulativeRecord (model.mustDuplicateFirstCumulativeRecord);
	duplicateMissingWaveRecords (model.mustDuplicateMissingWaveRecords);
}
//----------------------------------------------------
GribPlot::~GribPlot() {
    if (gribReader != NULL) {
    	delete gribReader;
        gribReader = NULL;
    }
}
//----------------------------------------------------
void GribPlot::initNewGribPlot(bool interpolateValues, bool windArrowsOnGribGrid, bool currentArrowsOnGribGrid)
{
    gribReader = NULL;
    
	this->mustInterpolateValues = interpolateValues;
	this->drawWindArrowsOnGrid = windArrowsOnGribGrid;
	this->drawCurrentArrowsOnGrid = currentArrowsOnGribGrid;
}
//----------------------------------------------------
void GribPlot::loadFile (QString fileName,
						 LongTaskProgress * taskProgress, int nbrecs)
{
	this->fileName = fileName;
	listDates.clear();
    
    if (gribReader != NULL) {
    	delete gribReader;
        gribReader = NULL;
    }
	
	gribReader = new GribReader ();
    if (gribReader != NULL)
    {
		gribReader->openFile (qPrintable(fileName), taskProgress, nbrecs);
		if (gribReader->isOk())
		{
			listDates = gribReader->getListDates();
			setCurrentDate ( listDates.size()>0 ? *(listDates.begin()) : 0);
		}
	}
}

//----------------------------------------------------
void GribPlot::duplicateFirstCumulativeRecord ( bool mustDuplicate )
{
	mustDuplicateFirstCumulativeRecord = mustDuplicate;
    if (gribReader != NULL  &&  gribReader->isOk())
    {
		if (mustDuplicate) {
			gribReader->copyFirstCumulativeRecord ();
		}
		else {
			gribReader->removeFirstCumulativeRecord ();
		}
	}
}

//----------------------------------------------------
void GribPlot::duplicateMissingWaveRecords ( bool mustDuplicate )
{
	mustDuplicateMissingWaveRecords = mustDuplicate;
    if (gribReader != NULL  &&  gribReader->isOk())
    {
		if (mustDuplicate) {
			gribReader->copyMissingWaveRecords ();
		}
		else {
			gribReader->removeMissingWaveRecords ();
		}
	}
}

//----------------------------------------------------
void GribPlot::setCurrentDate (time_t t)
{
    currentDate = t;
}
 

//-------------------------------------------------------------
// Grille GRIB
void GribPlot::draw_GridPoints (const DataCode &dtc, QPainter &pnt, const Projection *proj)
{
    if (gribReader == NULL) {
        return;
    }
//     GribRecord *rec = gribReader->getFirstGribRecord ();
	DataCode dd;
	if (dtc.dataType == GRB_PRV_WIND_XY2D)
		dd = DataCode (GRB_WIND_VX, dtc.levelType, dtc.levelValue);
	else if (dtc.dataType == GRB_PRV_CUR_XY2D)
		dd = DataCode (GRB_CUR_VX, dtc.levelType, dtc.levelValue);
	else
		dd = dtc;
		
    GribRecord *rec = gribReader->getRecord (dd, getCurrentDate());
	if (! rec)
			return;
	int deltaI, deltaJ;
	analyseVisibleGridDensity (proj, rec, 6, &deltaI, &deltaJ);
    int px,py, i,j, dl=2;
    for (i=0; i<rec->getNi(); i+=deltaI)
        for (j=0; j<rec->getNj(); j+=deltaJ)
        {
            //if (rec->hasValue(i,j))
            {
                proj->map2screen(rec->getX(i), rec->getY(j), &px,&py);
                pnt.drawLine(px-dl,py, px+dl,py);
                pnt.drawLine(px,py-dl, px,py+dl);
                proj->map2screen(rec->getX(i)-360.0, rec->getY(j), &px,&py);
                pnt.drawLine(px-dl,py, px+dl,py);
                pnt.drawLine(px,py-dl, px,py+dl);
            }
        }
}


//==================================================================================
// Flèches de direction du vent
//==================================================================================
void GribPlot::draw_WIND_Arrows (
				Altitude altitude, 
				bool barbules, QColor arrowsColor, 
				QPainter &pnt, const Projection *proj )
{
    if (gribReader == NULL) {
        return;
    }
	windAltitude = altitude;
    windArrowColor = arrowsColor;
    GribRecord *recx = gribReader->getRecord
								(DataCode(GRB_WIND_VX,altitude),currentDate);
    GribRecord *recy = gribReader->getRecord 
								(DataCode(GRB_WIND_VY,altitude),currentDate);
    if (recx == NULL || recy == NULL)
        return;        
	
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
    	for (int gi=0; gi<recx->getNi(); gi++)
    	{
			for (int gj=0; gj<recx->getNj(); gj++)
			{
				x = recx->getX(gi);
				y = recx->getY(gj);
				
					//----------------------------------------------------------------------
					if (! recx->isXInMap(x))
						x += 360.0;   // tour du monde ?
				proj->map2screen(x,y, &i,&j);
				if (i > W)
					proj->map2screen(x-360,y, &i,&j);
				
					if (recx->isPointInMap(x,y)) {
						vx = recx->getInterpolatedValue(x, y, mustInterpolateValues);
						vy = recy->getInterpolatedValue(x, y, mustInterpolateValues);
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
    else
    {	// Flèches uniformément réparties sur l'écran
		int i0, j0;
		//recx->print("");
		if (recx->entireWorldInLongitude) {
			//DBG("entireWorldInLongitude ");
			i0 = 0;
			j0 = 0;
		}
		else {
			if (recx->getDeltaY() > 0)
				proj->map2screen (recx->getX(0), recx->getY(recx->getNj()-1), &i0, &j0);
			else
				proj->map2screen (recx->getX(0), recx->getY(0), &i0, &j0);
			if (i0 > W) {
				if (recx->getDeltaY() > 0)
					proj->map2screen (recx->getX(0)-360, recx->getY(recx->getNj()-1), &i0, &j0);
				else
					proj->map2screen (recx->getX(0), recx->getY(0), &i0, &j0);
			}
		}
		if (j0<0) {
			j0 = 0;
		}
		for (i=i0; i<W; i+=space)
		{
			for (j=j0; j<H; j+=space)
			{
				proj->screen2map(i,j, &x,&y);
				//----------------------------------------------------------------------    			
				if (! recx->isXInMap(x))
					x += 360.0;   // tour du monde ?
				if (recx->isPointInMap(x,y)) {
					vx = recx->getInterpolatedValue(x, y, mustInterpolateValues);
					vy = recy->getInterpolatedValue(x, y, mustInterpolateValues);
					if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
					{
						if (barbules)
							drawWindArrowWithBarbs(pnt, i,j, vx,vy, (y<0), arrowsColor);
						else
							drawWindArrow(pnt, i,j, vx,vy);
					}
				}
				//else {
					//DBG("not in map: %g %g",x,y);
				//}
			}
		}
	}
}



//==================================================================================
// Flèches de direction du current
//==================================================================================
void GribPlot::draw_CURRENT_Arrows (
				Altitude altitude, 
				QColor arrowsColor, 
				QPainter &pnt, const Projection *proj )
{
    if (gribReader == NULL) {
        return;
    }
	currentAltitude = altitude;
    currentArrowColor = arrowsColor;

    GribRecord *recx = gribReader->getRecord
								(DataCode(GRB_CUR_VX,altitude),currentDate);
    GribRecord *recy = gribReader->getRecord 
								(DataCode(GRB_CUR_VY,altitude),currentDate);
    if (recx == NULL || recy == NULL)
        return;        
    int i, j;
    double x, y, vx, vy;
    int W = proj->getW();
    int H = proj->getH();
    
	int space;    
	    space =  drawCurrentArrowsOnGrid ? currentArrowSpaceOnGrid : currentArrowSpace;
    
    if (drawCurrentArrowsOnGrid)
    {	// Flèches uniquement sur les points de la grille
    	int oldi=-1000, oldj=-1000;
    	for (int gi=0; gi<recx->getNi(); gi++)
    	{
			x = recx->getX(gi);
			y = recx->getY(0);
			proj->map2screen(x,y, &i,&j);
			if (true || abs(i-oldi)>=space)
			{
				oldi = i;
				for (int gj=0; gj<recx->getNj(); gj++)
				{
					x = recx->getX(gi);
					y = recx->getY(gj);
					proj->map2screen(x,y, &i,&j);
					
						//----------------------------------------------------------------------
						if (! recx->isXInMap(x))
							x += 360.0;   // tour du monde ?

						if (recx->isPointInMap(x,y)) {
							if (true || abs(j-oldj)>=space)
							{
								oldj = j;
								vx = recx->getInterpolatedValue(x, y, mustInterpolateValues);
								vy = recy->getInterpolatedValue(x, y, mustInterpolateValues);
								if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
								{
									drawCurrentArrow(pnt, i,j, vx,vy);
								}
							}
						}
				}
			}
    	}
    }
    else
    {	// Flèches uniformément réparties sur l'écran
		for (i=0; i<W; i+=space)
		{
			for (j=0; j<H; j+=space)
			{
				proj->screen2map(i,j, &x,&y);
				//----------------------------------------------------------------------    			
				if (! recx->isXInMap(x))
					x += 360.0;   // tour du monde ?
				if (recx->isPointInMap(x,y)) {
					vx = recx->getInterpolatedValue(x, y, mustInterpolateValues);
					vy = recy->getInterpolatedValue(x, y, mustInterpolateValues);
					if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
					{
						drawCurrentArrow(pnt, i,j, vx,vy);
					}
				}
				//----------------------------------------------------------------------    			
			}
		}
	}
}

//--------------------------------------------------------------------------
void GribPlot::draw_ColoredMapPlain (
						DataCode dtc,
						bool smooth,
						QPainter &pnt, 
						const Projection *proj)
{
    if (gribReader == NULL)
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
			break;
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
		case GRB_TEMP_POT :
		case GRB_DEWPOINT :
		case GRB_SNOW_DEPTH :
		case GRB_SNOW_CATEG :
		case GRB_FRZRAIN_CATEG :
		case GRB_CAPE :
		case GRB_CIN :
		case GRB_PRV_THETA_E :
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
		case GRB_WAV_WHITCAP_PROB :
			drawColorMapGeneric_1D (pnt,proj,smooth, dtc, DataColors::function_getColor);
			break;
		default :
			break;
	}
}


//==================================================================================
// Waves arrows
//==================================================================================
void GribPlot::draw_WAVES_Arrows (
				const DataCode &dtc,
				QPainter &pnt, const Projection *proj )
{
    if (gribReader == NULL || dtc.dataType == GRB_TYPE_NOT_DEFINED) 
        return;
    QColor waveArrowColor (0,0,0);
    GribRecord *recDir, *recPer;
	if (dtc.dataType == GRB_PRV_WAV_PRIM) {
		recDir = gribReader->getRecord (DataCode(GRB_WAV_PRIM_DIR,LV_GND_SURF,0), currentDate);
		recPer = gribReader->getRecord (DataCode(GRB_WAV_PRIM_PER,LV_GND_SURF,0), currentDate);
	}
	else if (dtc.dataType == GRB_PRV_WAV_SCDY) {
		recDir = gribReader->getRecord (DataCode(GRB_WAV_SCDY_DIR,LV_GND_SURF,0), currentDate);
		recPer = gribReader->getRecord (DataCode(GRB_WAV_SCDY_PER,LV_GND_SURF,0), currentDate);
	}
	else if (dtc.dataType == GRB_PRV_WAV_MAX) {
		recDir = gribReader->getRecord (DataCode(GRB_WAV_MAX_DIR,LV_GND_SURF,0), currentDate);
		recPer = gribReader->getRecord (DataCode(GRB_WAV_MAX_PER,LV_GND_SURF,0), currentDate);
	}
	else if (dtc.dataType == GRB_PRV_WAV_SWL) {
		recDir = gribReader->getRecord (DataCode(GRB_WAV_SWL_DIR,LV_GND_SURF,0), currentDate);
		recPer = gribReader->getRecord (DataCode(GRB_WAV_SWL_PER,LV_GND_SURF,0), currentDate);
	}
	else if (dtc.dataType == GRB_PRV_WAV_WND) {
		recDir = gribReader->getRecord (DataCode(GRB_WAV_WND_DIR,LV_GND_SURF,0), currentDate);
		recPer = gribReader->getRecord (DataCode(GRB_WAV_WND_PER,LV_GND_SURF,0), currentDate);
	}
	else {
		recDir = recPer = NULL;
	}
    if (recDir==NULL || recPer==NULL)
        return;        
	
    int i, j;
    double x, y, vx, vy;
    int W = proj->getW();
    int H = proj->getH();
    
	int space = 30;    
	space =  drawWindArrowsOnGrid ? windArrowSpaceOnGrid : windArrowSpace;
    
    if (drawWindArrowsOnGrid)
    {	// Flèches uniquement sur les points de la grille
    	int oldi=-1000, oldj=-1000;
    	for (int gi=0; gi<recDir->getNi(); gi++)
    	{
			x = recDir->getX(gi);
			y = recDir->getY(0);
			proj->map2screen(x,y, &i,&j);
			if (true || abs(i-oldi)>=space)
			{
				oldi = i;
				for (int gj=0; gj<recDir->getNj(); gj++)
				{
					x = recDir->getX(gi);
					y = recDir->getY(gj);
					proj->map2screen(x,y, &i,&j);
						//----------------------------------------------------------------------
						if (! recDir->isXInMap(x))
							x += 360.0;   // tour du monde ?
						if (recDir->isPointInMap(x,y)) {
							if (true || abs(j-oldj)>=space)
							{
								oldj = j;
								vx = recDir->getInterpolatedValue(x, y, mustInterpolateValues);
								vy = recPer->getInterpolatedValue(x, y, mustInterpolateValues);
								if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
								{
									drawWaveArrow (pnt, i,j, vx,vy);
								}
							}
						}
				}
			}
    	}
    }
    else
    {	// Flèches uniformément réparties sur l'écran
		for (i=0; i<W; i+=space)
		{
			for (j=0; j<H; j+=space)
			{
				proj->screen2map(i,j, &x,&y);
				//----------------------------------------------------------------------    			
				if (! recDir->isXInMap(x))
					x += 360.0;   // tour du monde ?
				if (recDir->isPointInMap(x,y)) {
					vx = recDir->getInterpolatedValue(x, y, mustInterpolateValues);
					vy = recPer->getInterpolatedValue(x, y, mustInterpolateValues);
					if (vx != GRIB_NOTDEF && vy != GRIB_NOTDEF)
					{
						drawWaveArrow (pnt, i,j, vx,vy);
					}
				}
				//----------------------------------------------------------------------    			
			}
		}
	}
}

