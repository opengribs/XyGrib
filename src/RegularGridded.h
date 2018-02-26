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

#ifndef REGULARGRIDDED_H
#define REGULARGRIDDED_H

#include <iostream>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QPainter>
#include <QString>

#include "zuFile.h"

#include "GriddedReader.h" 
#include "GriddedRecord.h" 
#include "GriddedPlotter.h" 

#include "Projection.h"


//===============================================================
class RegularGridRecord : public GriddedRecord
{
    public:
        RegularGridRecord () {};
        virtual ~RegularGridRecord () {};
		virtual bool isRegularGrid () const 	{ return true; }
};

//===============================================================
class RegularGridReader : public GriddedReader
{
    public:
        RegularGridReader () {};
        virtual ~RegularGridReader () {};
};
 
//===============================================================
class RegularGridPlot : public GriddedPlotter
{
    public:
        RegularGridPlot () {}
        virtual ~RegularGridPlot() {}

	protected:
		QString fileName;
};


#endif
