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

#ifndef METAR_H
#define METAR_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>

#include "Util.h"
#include "Projection.h"
#include "zuFile.h"

class MetarWidgetFactory;

//---------------------------------------
class Airport {
    public:
		QString icao;
		QString country, state;
		float   lon, lat;
		float   altitude;
		QString name;
		
		bool operator < (const Airport &o) const;
};
//---------------------------------------
class MetarWidget : public QLabel
{ Q_OBJECT
	public:
		friend class MetarWidgetFactory;		// only factory can construct
		
	public slots:
		void projectionUpdated ();
		
	private:
		// Constructor is private, so only factory can construct item
        MetarWidget (const Airport &airport, Projection *proj, QWidget *parent);
		
		Airport airport;
		Projection *proj;
		
		void adjustGeometry ();
		
		// Events
    	QCursor   enterCursor;
		
		void  enterEvent (QEvent * e);
		void  leaveEvent (QEvent * e);
		void  mousePressEvent(QMouseEvent * e);
		void  mouseReleaseEvent(QMouseEvent * e);
		
};

//---------------------------------------
class MetarWidgetFactory
{
	public:
		MetarWidgetFactory ();
		~MetarWidgetFactory ();
		
		MetarWidget * createMetarWidget 
				(const QString &icao, bool isVisible,
					Projection *proj, QWidget *parent);
		
		QMap <QString, Airport> mapAirports;    // icao => data
		QMap <QString, QString> mapCountries;   // code country => name
		QMap <QPair <QString,QString>, QString> mapStates; // codes(country,state) => name
		
	private:
		void read_metar_list ();
};



#endif
