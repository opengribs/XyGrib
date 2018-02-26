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

#ifndef DATECHOOSER_H
#define DATECHOOSER_H

#include <set>
#include <list>

#include <QApplication>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "GriddedPlotter.h"



//--------------------------------------------------
class DateChooserPopup : public QWidget
{
    Q_OBJECT
	public:
		DateChooserPopup (QWidget *parent);
		void setDate (time_t date);
		void setText (QString txt);
		
	private:
		QLabel label;
};

//--------------------------------------------------
class DateChooser : public QWidget
{
    Q_OBJECT
	public:
		DateChooser ();
		~DateChooser ();
		
		void  setDate (time_t date);
		void  reset ();
		void  setGriddedPlotter (GriddedPlotter *plotter);
		void  setMarkToday (bool b) {markToday = b;}
		
	public slots:
		void slotSliderDatesValueChanged (int value);
		void slotSliderPressed ();
		void slotSliderReleased ();
		
	signals:
		void  signalDateChanged (time_t date, bool isMoving);

	private:        
		GriddedPlotter *plotter;
		time_t *tabDates;
		int     nbDates;
		time_t currentDate;
		bool   markToday;
		
		QSlider *slider;
		DateChooserPopup *popup;
		
        void setListDates (std::set<time_t> * listDates, 
						   time_t currentDate );
		
		void  paintEvent(QPaintEvent *ev);
};

#endif
