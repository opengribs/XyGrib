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

#ifndef IRREGULARGRIDDED_H
#define IRREGULARGRIDDED_H

#include <iostream>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QPainter>
#include <QString>

#include "zuFile.h"

#include "GriddedPlotter.h"
#include "Projection.h"


//===============================================================
class IrregularGridRecord : public GriddedRecord
{
    public:
        IrregularGridRecord () {};
        virtual ~IrregularGridRecord () {};
		virtual bool isRegularGrid () const 	{ return false; }
};

//===============================================================
class IrregularGridReader : public GriddedReader
{
    public:
        IrregularGridReader () {};
        virtual ~IrregularGridReader () {};
};

//===============================================================
class IrregularGridPlot : public GriddedPlotter
{
    public:
        IrregularGridPlot ();
        virtual ~IrregularGridPlot() {};

		virtual void  setCurrentDate (time_t t);


	protected:
		QString fileName;
};


#endif
