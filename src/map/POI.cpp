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

#include <cmath>
#include <cassert>

#include <QTimer>
#include <QMessageBox>

#include "Settings.h"
#include "Font.h"
#include "POI.h"
#include "POI_Editor.h"

//----------------------------------------------
// added by Tim Holtschneider, 05.2010
// list storing selected POIs
QList<POI*> GLOB_listSelectedPOI;

//-------------------------------------------------------------------------------
// Read from old settings format (version <= 3.3.0)
//-------------------------------------------------------------------------------
POI::POI (QString seralizedPOI_oldFormat)//
				 // Projection *proj, QWidget *ownerSlotsPOI, QWidget *parentWindow)
	: QWidget(NULL)
{
	valid = true;
	isMovable = false;
	showLabel = true;
	QStringList  lst = seralizedPOI_oldFormat.split(";");
	bool ok, ok1,ok2;	
	this->code = lst[0].toUInt(&ok);
	name = QString( QByteArray::fromBase64(lst[1].toUtf8()) );
	lon = lst[2].toFloat (&ok1);
	lat = lst[3].toFloat (&ok2);
	if (  !ok || !ok1 || !ok2
		|| lat<-90  || lat>90  || lon<-360 || lon>360
	) {
		valid = false;
		return;
	}
	this->markColor = Qt::red;
	this->labelFont = Font::getFont(FONT_POILabel);
	this->labelTextColor = Qt::black;
	this->labelBgColor = Qt::white;
	this->labelBgColor.setAlpha(200);
    widgetFocus = NULL;
}

//-------------------------------------------------------------------------------
POI::POI(uint code, QString name, double lon, double lat,
				 Projection *proj, QWidget *ownerSlotsPOI, QWidget *parentWindow)
	: QWidget(parentWindow)
{
	valid = true;
	isMovable = false;
	showLabel = true;
	this->code = code;
	this->name = name;
	this->lon = lon;
	this->lat = lat;
	this->parent = parentWindow;
	setProjection (proj);
	createWidget  (ownerSlotsPOI);
	setDisplayParams (Qt::red, Font::getFont(FONT_POILabel), Qt::black,Qt::white);
	adjustGeometry();
    widgetFocus = NULL;
}

//-------------------------------------------------------------------------------
// Read POI's params from native settings file
POI::POI (uint codeFromOldSettings)
{
	valid = true;
	code = codeFromOldSettings;
	this->parent = NULL;
	readSettings (codeFromOldSettings, true);
    widgetFocus = NULL;
}

//-------------------------------------------------------------------------------
// Read POI's params from current (.ini) settings file
POI::POI (uint code,
			Projection *proj, QWidget *ownerSlotsPOI, QWidget *parentWindow)
	: QWidget(parentWindow)
{
	valid = true;
	this->code = code;
	this->parent = parentWindow;
	readSettings (code, false);
	setProjection (proj);
	createWidget  (ownerSlotsPOI);
	adjustGeometry();
    widgetFocus = NULL;
}
//-------------------------------------------------------------------------------
void POI::setDisplayParams ( QColor markColor,
						QFont  labelFont,
						QColor textColor,
						QColor bgColor     )
{
	this->markColor = markColor;
	this->labelFont = labelFont;
	this->labelTextColor = textColor;
	if (labelBgColorMarkedPOI.isValid()) {
		this->labelBgColorMarkedPOI   = bgColor;
		this->labelBgColorMarkedPOI.setAlpha(200);
	} else {
		this->labelBgColor   = bgColor;
		this->labelBgColor.setAlpha(200);
	}
}

//-------------------------------------------------------------------------------
void POI::readSettings (uint code, bool fnat)
{
	name = Settings::getSettingPOI (code, "name", "", fnat).toString();
	if (name == "")
		valid = false;
	lon  = Settings::getSettingPOI (code, "lon", 0, fnat).toDouble();
	lat  = Settings::getSettingPOI (code, "lat", 0, fnat).toDouble();
	isMovable = Settings::getSettingPOI (code, "move", false, fnat).toBool();
	showLabel = Settings::getSettingPOI (code, "showLabel", true, fnat).toBool();
	setDisplayParams
		( Settings::getSettingPOI (code, "markColor", QColor(Qt::red), fnat).value<QColor>(),
	      Settings::getSettingPOI (code, "labelFont", Font::getFont(FONT_POILabel), fnat).value<QFont>(),
		  Settings::getSettingPOI (code, "labelTextColor", QColor(Qt::black), fnat).value<QColor>(),
		  Settings::getSettingPOI (code, "labelBgColor", QColor(Qt::white), fnat).value<QColor>()
		);		
}
//-------------------------------------------------------------------------------
void POI::writeSettings ()
{
	//printf("write poi: %s\n", qPrintable(name));
	Settings::setSettingPOI (code, "name", name);
	Settings::setSettingPOI (code, "lon", lon);
	Settings::setSettingPOI (code, "lat", lat);
	Settings::setSettingPOI (code, "markColor", markColor);
	Settings::setSettingPOI (code, "labelFont", labelFont);
	Settings::setSettingPOI (code, "labelTextColor", labelTextColor);
	// write stored bg-color for marked POIs, TH20110103
	if (labelBgColorMarkedPOI.isValid()) {
		Settings::setSettingPOI (code, "labelBgColor", labelBgColorMarkedPOI);
	}else {
		Settings::setSettingPOI (code, "labelBgColor", labelBgColor);
	}
	Settings::setSettingPOI (code, "move", isMovable);
	Settings::setSettingPOI (code, "showLabel", showLabel);
}


//-------------------------------------------------------------------------------
void POI::createWidget(QWidget *ownerSlotsPOI)
{
	connect(proj, SIGNAL(projectionUpdated()), this, SLOT(projectionUpdated()) );
    connect(this, SIGNAL(signalOpenMeteotablePOI(POI*)),
    						ownerSlotsPOI, SLOT(slotOpenMeteotablePOI(POI*)));
	connect(this, SIGNAL(signalPOImoved(POI *)), ownerSlotsPOI, SLOT(slotPOImoved(POI *)));

	countClick = 0;
	
	setAutoFillBackground (false);
	moveInCourse = false;
	adjustGeometry();
}

//-------------------------------------------------------------------------------
void POI::adjustGeometry()
{
	if (proj == NULL)
		return;
	QFontMetrics fmet(labelFont);
	QRect rect = fmet.boundingRect(name);
	textHeight = fmet.ascent();
	
	bool ok = proj->map2screen_glob (lon, lat, &pi, &pj);

	int hw = rect.height();
	if (hw <= 5)
		hw = 9;
	
	hpx = 3;				// hot point
	hpy = hw/2;

	xLabel = 9;
	
	if (ok) {
		if (showLabel)
			setGeometry(pi-hpx, pj-hpy,  rect.width()+xLabel+hpx+5, hw);
		else
			setGeometry(pi-hpx, pj-hpy,  9,9);
	}
	else {
		setGeometry(-1000,-1000,  rect.width()+14,  rect.height());  // not visible
	}
}

//-------------------------------------------------------------------------------
// serialized POI = "code;base64(name);lon;lat"
QString POI::serialize()
{
	QString r;
	QByteArray b64 = name.toUtf8().toBase64();
	QString sb64 (b64);
	
	r = QString("%1;%2;%3;%4")
			.arg(code)
			.arg(sb64)
			.arg(lon)
			.arg(lat);
	return r;
}

//-------------------------------------------------------------------------------
void POI::setName(QString name)
{
	this->name=name;
	setToolTip(tr("Point of interest: ")+name);
	adjustGeometry();
}

//-------------------------------------------------------------------------------
void POI::setProjection( Projection *proj)
{
	this->proj = proj;
	adjustGeometry();
}

//-------------------------------------------------------------------------------
void POI::projectionUpdated()
{
	setProjection(proj);
}

//-------------------------------------------------------------------------------
void  POI::drawContent(QPainter &pnt, Projection *proj, bool drawingInPixmap)
{
	pnt.save();
	int dy = height()/2;
	bool isPoiInMap = true;
	if (drawingInPixmap)
	{
		isPoiInMap = proj->map2screen_glob (lon, lat, &pi, &pj);
		pnt.translate(pi-3, pj-dy);
	}
	if (isPoiInMap) {
		QPen pen(markColor);
		pen.setWidth(4);
		pnt.setPen(pen);	
		pnt.fillRect(0,dy-3,7,7, QBrush(markColor));

		if (showLabel)
		{
			pnt.fillRect(xLabel,0, width()-xLabel-1,height()-1, QBrush(labelBgColor));
			int g = 60;
			pen = QPen(QColor(g,g,g));
			pen.setWidth(1);
			pnt.setPen(pen);	
			pnt.drawRect(xLabel,0,width()-xLabel-1,height()-1);

			pnt.setFont(labelFont);
			pnt.setPen(labelTextColor);
			pnt.drawText(xLabel+3,textHeight, name);
		}	
	}
	pnt.restore();
}

//-------------------------------------------------------------------------------
void  POI::paintEvent(QPaintEvent *)
{    
    QPainter pnt(this);
    drawContent(pnt, proj, false);
}

//=========================================================================
void  POI::enterEvent (QEvent *)
{
//printf("POI::enterEvent\n");
    enterCursor = cursor();
    setCursor (Qt::PointingHandCursor);

    widgetFocus = QApplication::focusWidget();
    this->setEnabled(true);
    this->setFocus();
}
//-------------------------------------------------------------------------------
void  POI::leaveEvent (QEvent *)
{
//printf("POI::leaveEvent\n");
    setCursor(enterCursor);
	if (QApplication::allWidgets().contains(widgetFocus)) {
     	widgetFocus->setFocus();
	}
}

//=========================================================================
void  POI::keyPressEvent (QKeyEvent *e)
{
//printf("POI::keyPressEvent\n");
	if (isMovable && e->modifiers()==Qt::ControlModifier) {
		lastLon = lon;
		lastLat = lat;
    	setCursor(Qt::SizeAllCursor);
	}
}
//-------------------------------------------------------------------------------
void  POI::keyReleaseEvent (QKeyEvent *)
{	
    setCursor(Qt::PointingHandCursor);
}
//=========================================================================
void POI::mouseMoveEvent (QMouseEvent * e) {
    if (moveInCourse)
    {
    	int i,j;	// position on screen
    	i = x() + e->x() - xMouse + hpx;
    	j = y() + e->y() - yMouse + hpy;
    	if (i>0 && j>0 && i+hpx<proj->getW() && j+hpy<proj->getH())
    	{
			proj->screen2map(i, j, &lon, &lat);
			emit signalPOImoved(this);
			adjustGeometry();
			// printf("%d %d   %f %f\n", e->x(), e->y(),  i,j, lon,lat);
		}
    }
}
//---------------------------------------------------------
void  POI::mousePressEvent(QMouseEvent *e)
{
	if (isMovable && e->modifiers()==Qt::ControlModifier) {
		moveInCourse = true;		// Ctrl+Clic : move POI
		xMouse = e->x();
		yMouse = e->y();
	} else if (e->modifiers()==Qt::ShiftModifier) { 
	// add POI to global list, TH20100514
		GLOB_listSelectedPOI.append( this );
		this->labelBgColorMarkedPOI = this->labelBgColor;
		this->labelBgColor = QColor(32, 32,0,127);
	}
	else
		moveInCourse = false;	
	
}
//-------------------------------------------------------------------------------
void  POI::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->x()<0 ||e->x()>width() ||e->y()<0 ||e->y()>height())
		return;
		
	if (! moveInCourse && e->modifiers()==Qt::NoModifier) {
		if (e->button() == Qt::LeftButton)
		{
			setCursor(Qt::BusyCursor);
			if (countClick == 0) {
				//QTimer::singleShot(qApp->doubleClickInterval(), this, SLOT(timerClickEvent()));
				QTimer::singleShot(300, this, SLOT(timerClickEvent()));
			}
			countClick ++;
			if (countClick==2)
			{
				// Double Clic : Edit this Point Of Interest
				new POI_Editor(this, parent);
				countClick = 0;
			}
		}
		else if (e->button() == Qt::RightButton)
		{
			// Right clic : Edit this Point Of Interest
			new POI_Editor(this, parent);
		}
	}
	else {
		// Need to exclude writeSettings for marking POI, TH20110103
		if (e->button()!=Qt::LeftButton || e->modifiers()!=Qt::ShiftModifier) {
			// fin de déplacement du POI
			if (lon!=lastLon || lat!=lastLat) {
				this->writeSettings();		// save new position
			}
		}
	}
	moveInCourse = false;
}
//-------------------------------------------------------------------------------
void  POI::timerClickEvent()
{
	if (countClick==1)
	{
		// Single clic : Meteotable for this Point Of Interest
		emit signalOpenMeteotablePOI(this);
	}
	countClick = 0;
}
//-------------------------------------------------------------------------------
// Restore background color for all selected POIs, TH20110103
void POI::restoreBgOfSelectedPOIs()
{
	QList<POI*>::iterator iterPOI;
	for( iterPOI = GLOB_listSelectedPOI.begin(); iterPOI != GLOB_listSelectedPOI.end(); iterPOI++ ) {
		if( (*iterPOI)->isValid() ){
			(*iterPOI)->labelBgColor = (*iterPOI)->labelBgColorMarkedPOI;
			(*iterPOI)->labelBgColorMarkedPOI = QColor( QColor::Invalid );
			(*iterPOI)->update();						
		}
	}
}

