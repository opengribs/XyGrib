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

#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

#include "DateChooser.h"
#include "Util.h"
#include "Font.h"

//=====================================================================
DateChooserPopup::DateChooserPopup (QWidget *parent)
		: QWidget (parent, Qt::ToolTip)
{	
    QVBoxLayout *lay = new QVBoxLayout();
    assert (lay);
	lay->setContentsMargins (16,8,16,8);
	this->setLayout(lay);
	lay->addWidget (&label);
	
	setAutoFillBackground (true);
}
//-------------------------------------------------
void DateChooserPopup::setText (QString txt)
{
	label.setText(txt);
}


//=====================================================================
DateChooser::DateChooser ()
		: QWidget ()
{	
	tabDates = NULL;
	nbDates  = 0;
	currentDate = 0;
	plotter  = NULL;
	markToday = true;
	
    QVBoxLayout *lay = new QVBoxLayout();
    assert (lay);
	lay->setContentsMargins (0,3,0,0);
	
	slider = new QSlider (Qt::Horizontal, this);
    assert (slider);
	slider->setRange (0, 0);
	slider->setTickInterval (1);
	slider->setTickPosition (QSlider::TicksAbove);
	
	slider->setSingleStep (0);	// ignore keyboard events
	slider->setPageStep (0);
	
	popup = new DateChooserPopup (NULL);
    assert (popup);
	popup->setVisible (false);
	popup->raise ();
	
	connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderDatesValueChanged(int)));
	connect (slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
	connect (slider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()));
	
	lay->addWidget (slider);
	this->setLayout(lay);
}
//------------------------------------------------------------------------
DateChooser::~DateChooser ()
{	
	if (tabDates)
		delete [] tabDates;
}

//------------------------------------------------------------------------
void DateChooser::slotSliderDatesValueChanged (int pos)
{
	if (pos>=0 && pos<nbDates) {
		currentDate = tabDates [pos];
		bool  down = slider->isSliderDown ();
		emit signalDateChanged (currentDate, down);
	}
	update();
}
//------------------------------------------------------------------------
void DateChooser::slotSliderPressed ()
{
	update();
}
//------------------------------------------------------------------------
void DateChooser::slotSliderReleased ()
{
	popup->setVisible (false);
	int pos = slider->value();
	if (pos>=0 && pos<nbDates) {
		currentDate = tabDates [pos];
		emit signalDateChanged (currentDate, false);
	}
	update();
}
//------------------------------------------------------------------------
void DateChooser::setDate (time_t date)
{
	int pos = -1;
	for (int i=0; pos<0 && i<nbDates; i++) {
		if (tabDates [i] == date)
			pos = i;
	}
	if (pos >= 0) {
		currentDate = date;
		slider->setValue (pos);
	}
	update();
}
//------------------------------------------------------------------------
void DateChooser::reset ()
{
	if (tabDates) {
		delete [] tabDates;
		tabDates = NULL;
	}
	nbDates = 0;
	currentDate = 0;
	slider->setRange (0, 0); 
	slider->setValue (0);
	update();
}
//------------------------------------------------------------------------
void DateChooser::setGriddedPlotter (GriddedPlotter *plotter)
{
	this->plotter = plotter;
	if (plotter && plotter->isReaderOk()) {
		setListDates (plotter->getListDates(), plotter->getCurrentDate());
	}
	else {
		this->reset();
	}
} 
//------------------------------------------------------------------------
void DateChooser::setListDates (std::set<time_t> * setDates, 
					time_t currentDate )
{
	if (tabDates) {
		delete [] tabDates;
		tabDates = NULL;
	}
	nbDates = 0;
	this->currentDate = currentDate;
	int current = 0;
	if (setDates && setDates->size()>0)
	{
		nbDates = setDates->size();
		tabDates = new time_t [nbDates];
		int i=0;
		std::set<time_t>::iterator it;
		for (it=setDates->begin(); it!=setDates->end(); it++, i++) {
			tabDates [i] = *it;
			if (*it == currentDate)
				current = i;
		}
	}
	slider->setRange (0, nbDates-1);
	slider->setValue (current);
	update();
}

//------------------------------------------------------------------------
void DateChooser::paintEvent(QPaintEvent */*event*/)
{
    QPainter pnt(this); 
	int W = width();
	int H = height();

	QColor c1 (200,220,100);
	QColor c2 (100,220,200);
	QColor c = c2;
	pnt.fillRect(0,0,W,H, QBrush(c2));

	if (nbDates > 1 && plotter && plotter->isReaderOk()) 
	{
		int t1, t2;
		double x, xcurrent=-1, xclosest=-1;
		time_t closestDate = plotter->getClosestDateFromNow();

		QStyleOptionSlider slopt;
		slopt.initFrom(this);
		QStyle *st = slider->style();
		int slen = st->pixelMetric (QStyle::PM_SliderLength, &slopt);
		int slspace = st->pixelMetric (QStyle::PM_SliderSpaceAvailable, &slopt);
		int dx = QStyle::sliderPositionFromValue (0, nbDates-1, 1, slspace) 
					- QStyle::sliderPositionFromValue (0, nbDates-1, 0, slspace);
	
		// fill a colored rectangle for each day
		t1 = Util::getDayNum (tabDates[0]);
		for (int i=0; i<nbDates; i++) 
		{
			t2 = Util::getDayNum (tabDates[i]);
			if (t2 != t1) {  // same day ?
				c = (c==c1) ? c2 : c1;
				t1 = t2;
			}
			x = QStyle::sliderPositionFromValue (0, nbDates-1, i, slspace) 
							+ slen/2.0; 		
			pnt.fillRect((int)x,0, (int)dx+1, H-8, QBrush(c));
			if (currentDate == tabDates[i]) {
				xcurrent = QStyle::sliderPositionFromValue (0, nbDates-1, i, slspace) 
							+ slen/2.0; 		
			}
			if (closestDate == tabDates[i]) {
				xclosest = QStyle::sliderPositionFromValue (0, nbDates-1, i, slspace) 
							+ slen/2.0; 
			}
		}
		// mark closest date from now
		if (markToday && xclosest != -1)
			pnt.fillRect((int)xclosest-5,0,10,H-8, QBrush(QColor(250,250,0)));
		
		// mark the current position
		if (xcurrent != -1) {
			pnt.fillRect((int)xcurrent-3,0,6,H-8, QBrush(QColor(255,0,0)));
		}
		
		// show the date in a popup
		if (xcurrent != -1) {
			if (slider->isSliderDown ())
			{
				QString sdate = Util::formatDateTimeLong (currentDate);
				popup->setText (sdate);
				int px,py;
				px = slider->x() + slider->width()/2;
				py = slider->y();
				px -= popup->width()/2;
				py -= popup->height()+40;
				QPoint pos (px,py);
				
				pos = mapToGlobal (pos);
				//DBG ("%d %d", px,py);				
				//DBG ("%d %d", pos.x(),pos.y());				
				popup->move (pos);
				popup->setVisible (true);
				popup->raise ();
			}
		}
		
		// draw the short name of each day
		QFont fontTitle = Font::getFont(FONT_DateChooser);
		pnt.setFont (fontTitle);
		int y = 10;
		c = QColor(0,0,0);
		pnt.setPen(c);
		t1 = Util::getDayNum (tabDates[0]);

		// A.Degwerth [Cassidian]: added to support right to left font direction/layout		
		QFontMetrics fm(fontTitle);
		
		for (int i=0; i<nbDates; i++) {
			t2 = Util::getDayNum (tabDates[i]);
			if (i==0 || t2 != t1) {
				x = QStyle::sliderPositionFromValue (0, nbDates-1, i, slspace) 
								+ slen/2.0; 		
				// A.Degwerth [Cassidian]: added to support right to left font direction/layout
				// get the text width
				QString strDayName = Util::formatDayName (tabDates[i]);
				QRect rcText = fm.boundingRect(strDayName);
				
				pnt.drawText (QApplication::isRightToLeft() ? 
							W-((int)x+3)-rcText.width(): 
							(int)x+3, y,  strDayName );
				t1 = t2;
			}
		}
	}

}



