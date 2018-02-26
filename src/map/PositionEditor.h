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

#ifndef POSITIONEDITOR_H
#define POSITIONEDITOR_H

#include <QApplication>
#include <QToolBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QComboBox>


#include "Util.h"

//===================================================================
class DegreeMinuteEditor : public QWidget
{ Q_OBJECT
    public:
        DegreeMinuteEditor(QWidget *parent, int degreMin, int degreMax);
    	
		virtual  double   getValue();
    	QString  getDirection()  {return cbDirection->currentText();}
    	
    protected:
		QSpinBox  		*angDeg;
		QDoubleSpinBox  *angMin;
		QComboBox 		*cbSigne;
		QComboBox 		*cbDirection;
};
//-------------------------------------------------------------------
class LongitudeEditor : public DegreeMinuteEditor
{
	public:
		LongitudeEditor(double val, QWidget *parent);
    	double getValue();
};
//-------------------------------------------------------------------
class LatitudeEditor : public DegreeMinuteEditor
{
	public:
		LatitudeEditor(double val, QWidget *parent);
    	double getValue();
};


//===================================================================
class PositionEditor : public QWidget
{
    Q_OBJECT
public:
    PositionEditor(double lon, double lat, QWidget *parent);
    
	double   getLongitude() {return editLongitude->getValue();}
	double   getLatitude()  {return editLatitude->getValue();}


private:
	double lon, lat;
	LatitudeEditor  *editLatitude;
	LongitudeEditor *editLongitude;
	
};


#endif
