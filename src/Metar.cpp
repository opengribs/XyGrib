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

#include "Metar.h" 

//==============================================================
bool Airport::operator < (const Airport &o) const
{
	if (country < o.country)
		return true;
	else if  (country==o.country) {
		if (state < o.state)
			return true;
		else if (state == o.state) {
			if  (name < o.name)
				return true;
		}
	}
	return false;
}

//==============================================================
MetarWidget::MetarWidget (const Airport &airport, Projection *proj, QWidget *parent)
	: QLabel (parent)
{
	this->proj = proj;
	this->airport = airport;
	
	QPixmap pixmap (Util::pathImg("airport.png"));
	setPixmap (pixmap);
	resize (pixmap.width(), pixmap.height());
	
	setToolTip ("METAR station: "+airport.icao+" - "+airport.name);
	adjustGeometry ();
	connect (proj, SIGNAL(projectionUpdated()), this, SLOT(projectionUpdated()) );
}
//-------------------------------------------------------------------------------
void MetarWidget::projectionUpdated()
{
	adjustGeometry ();
}

//-------------------------------------------------------------------------------
void MetarWidget::adjustGeometry()
{
	if (proj == NULL)
		return;
	int  pi, pj;
	bool ok = proj->map2screen_glob (airport.lon, airport.lat, &pi, &pj);
	
	if (ok) {
		//setGeometry (pi, pj,  10, 10);
		//DBG ("%g %g",width()/2., height()/2.);
		move (pi-width()/2, pj-height()/2);
	}
	else {
		//setGeometry (-1000,-1000,  10,10);  // not visible
		move (-1000,-1000);
	}
}
//-------------------------------------------------------------------------------
void  MetarWidget::enterEvent (QEvent *)
{
    enterCursor = cursor();
    setCursor (Qt::UpArrowCursor);

}
//-------------------------------------------------------------------------------
void  MetarWidget::leaveEvent (QEvent *)
{
    setCursor(enterCursor);
}
void  MetarWidget::mousePressEvent(QMouseEvent *)
{
}
void  MetarWidget::mouseReleaseEvent(QMouseEvent *)
{
	DBGQS ("Open METAR : "+airport.icao+" : "+airport.name);
}
//==============================================================
MetarWidgetFactory::MetarWidgetFactory ()
{
	read_metar_list ();
}
//-----------------------------------------------
MetarWidgetFactory::~MetarWidgetFactory ()
{
	mapAirports.clear ();
	mapCountries.clear ();
	mapStates.clear ();
}
//-----------------------------------------------
MetarWidget * MetarWidgetFactory::createMetarWidget 
			( const QString &icao, bool isVisible, Projection *proj, QWidget *parent )
{
	MetarWidget *w = NULL;
	w = new MetarWidget (mapAirports.value(icao), proj, parent);
	w->setVisible (isVisible);
	return w;
}
//-------------------------------------------------------------------------------
void MetarWidgetFactory::read_metar_list ()
{
	char buf [512];
	ZUFILE *f;
	Airport a;
	mapAirports.clear ();
	mapCountries.clear ();
	mapStates.clear ();
 	f = zu_open (qPrintable(Util::pathGis()+"stations_metar.txt.gz"), "rb");
    if (f != NULL) {
		while (zu_fgets(buf, 512, f)) {
			if (strlen(buf)>0) {
				QString line = QString (buf);
				QStringList ls = line.split(";");
				if (ls.size() >= 7) {
					a.icao     = ls[0].trimmed();
					a.country  = ls[1].trimmed();
					a.state    = ls[2].trimmed();
					a.lat      = ls[3].toDouble();
					a.lon      = ls[4].toDouble();
					a.altitude = ls[5].toDouble();
					a.name     = ls[6].trimmed();
					mapAirports.insert (a.icao, a);
				}
			}
		}
        zu_close(f);
    }
 	f = zu_open (qPrintable(Util::pathGis()+"countries_en.txt.gz"), "rb");
    if (f != NULL) {
		while (zu_fgets(buf, 512, f)) {
			if (strlen(buf)>0) {
				QString line = QString (buf);
				QStringList ls = line.split(";");
				if (ls.size() >= 2) {
					mapCountries.insert (ls[0].trimmed(), ls[1].trimmed());
				}
			}
		}
        zu_close(f);
    }
 	f = zu_open (qPrintable(Util::pathGis()+"states_en.txt.gz"), "rb");
    if (f != NULL) {
		while (zu_fgets(buf, 512, f)) {
			if (strlen(buf)>0) {
				QString line = QString (buf);
				QStringList ls = line.split(";");
				if (ls.size() >= 3) {
					mapStates.insert (QPair<QString,QString>(ls[0].trimmed(),ls[1].trimmed()), ls[2].trimmed());
				}
			}
		}
        zu_close(f);
    }
}

