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

#ifndef COLORSCALEWIDGET_H
#define COLORSCALEWIDGET_H

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QDockWidget>

#include <vector>

#include "DataDefines.h"
#include "DataColors.h"
#include "Util.h"

//-------------------------------------------------
class ColorScaleGraph : public QWidget
{Q_OBJECT

	public:
		ColorScaleGraph (QWidget *parent=NULL);
		void setColorScale (DataColors *datacolors, const DataCode &dtc);

	private:
		void  paintEvent (QPaintEvent *);
		QString formatValue (double v);

		ColorScale *sc;
		DataColors *datacolors;
		DataCode dtc;
};


//-------------------------------------------------
class ColorScaleWidget : public QDockWidget
{Q_OBJECT

	public:
		ColorScaleWidget (QWidget *parent=NULL);
					
		void setColorScale (DataColors *datacolors, const DataCode &dtc);
		
	private:
		ColorScaleGraph *graph;
};




#endif
