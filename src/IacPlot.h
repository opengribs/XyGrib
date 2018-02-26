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

/*************************
Lecture mise en mémoire d'un fichier IAC (fleetcodes)

*************************/

#ifndef IACPLOT_H
#define IACPLOT_H

#include <iostream>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QPainter>
#include <QString>

#include "DataMeteoAbstract.h"
#include "zuFile.h"
#include "IacReader.h"
#include "Projection.h"

//===============================================================
class IacPlot : public DataPlotterAbstract
{
    public:
        IacPlot();
        IacPlot(QString fname);
        
        ~IacPlot () {}

		void  loadFile(QString fileName);
		bool  isReaderOk()
					{return iacReader!=NULL && iacReader->isOk();}	
		IacReader *getReader()
					{return iacReader;}

		void  draw_PRESSURE_MinMax      (QPainter &pnt, Projection *proj);
		void  draw_PRESSURE_TroughLines (QPainter &pnt, Projection *proj);
		void  draw_PRESSURE_Isobars     (QPainter &pnt, Projection *proj);
		void  draw_PRESSURE_IsobarsLabels (QPainter &pnt, Projection *proj);
		
		void  draw_FRONT_all   (QPainter &pnt, Projection *proj);
		void  draw_FRONT_Warm       (QPainter &pnt, Projection *proj);
		void  draw_FRONT_Cold       (QPainter &pnt, Projection *proj);
		void  draw_FRONT_Occlus     (QPainter &pnt, Projection *proj);
        
        void  show_CoverZone (QPainter &pnt, const Projection *proj);
		void  updateGraphicsParameters ();


    private:
		IacReader  *iacReader;

		QString fileName;
		
		void drawIacLine(std::vector <IAC_Point *> &lspoints, bool smooth,
							QPainter &pnt, Projection *proj);
		void makeBezierPath(QPainterPath &path,
							std::vector <IAC_Point *> &lspoints,
							double offsetX,
							Projection *proj);
};


#endif
