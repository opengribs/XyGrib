#include <cmath>
#include <QtGui>

#include "PositionEditorWidget.h"

//--------------------------------------------------------------
PositionEditorWidget::PositionEditorWidget
    				( QWidget *parent,
    				  double lat,
    				  double lon,
					  QString userOrient_lat,   /* "Auto" "North+" "South+" */
					  QString userOrient_lon    /* "Auto" "East+" "West+"   */
					)
    : QWidget(parent)
{
    setupUi(this);
	orientLat = userOrient_lat;
	orientLon = userOrient_lon;
	
    lat_NS->addItem(tr("N"),"N");
    lat_NS->addItem(tr("S"),"S");
    lon_EW->addItem(tr("E"),"E");
    lon_EW->addItem(tr("W"),"W");
    lat_sign->addItem(tr("+"),"+");
    lat_sign->addItem(tr("-"),"-");
    lon_sign->addItem(tr("+"),"+");
    lon_sign->addItem(tr("-"),"-");

	setLatitude  (lat);
	setLongitude (lon);
}

//-------------------------------------------------------
double PositionEditorWidget::getLongitude()
{
	QString signe = lon_sign->itemData (lon_sign->currentIndex() ).toString();
	QString dir   = lon_EW->itemData (lon_EW->currentIndex() ).toString();
	double  deg = (double) lon_degrees->value();
	double  min = (double) lon_minutes->value();

	double val = deg + min/60.0;
	if (signe == "-")
		val = -val;
	if (dir == "W")
		val = -val;
	
	return val;
}

//-------------------------------------------------------
double PositionEditorWidget::getLatitude()
{
	QString signe = lat_sign->itemData (lat_sign->currentIndex() ).toString();
	QString dir   = lat_NS->itemData (lat_NS->currentIndex() ).toString();
	double  deg = (double) lat_degrees->value();
	double  min = (double) lat_minutes->value();
	
	double val = deg + min/60.0;
	if (signe == "-")
		val = -val;	
	if (dir == "S")
		val = -val;
	
	return val;
}

//--------------------------------------------------------------
void PositionEditorWidget::setLongitude(double val)
{
	lat_sign->setCurrentIndex( lat_sign->findData("+") );

	while (val > 360)
		val -= 360;
	while (val < -360)
		val += 360;
    
    int    deg;
    double min;
    if (orientLon == "East+")    {
    	lon_EW->setCurrentIndex(lon_EW->findData("E"));
		if (val < 0) {
			val = 360 + val;
		}
    }
    else if (orientLon == "West+")    {
    	lon_EW->setCurrentIndex(lon_EW->findData("W"));
		if (val > 0) {
			val = 360 - val;
		}
    }
    else {
		// Mode auto
		if (val > 0) {
			if (val <= 180) {
    			lon_EW->setCurrentIndex(lon_EW->findData("E"));
			}
			else {
    			lon_EW->setCurrentIndex(lon_EW->findData("W"));
				val = 360 - val;
			}
		}
		else {
			if (val >= -180) {
    			lon_EW->setCurrentIndex(lon_EW->findData("W"));
    			val = -val;
			}
			else {
    			lon_EW->setCurrentIndex(lon_EW->findData("E"));
				val = val + 360;
			}
		}
    }
	deg = (int) trunc(val);
	lon_degrees->setValue( abs(deg) );
	min = 60.0*fabs(val-trunc(val));
	lon_minutes->setValue( min );
}


//--------------------------------------------------------------
void PositionEditorWidget::setLatitude(double val)
{
	lat_sign->setCurrentIndex( lat_sign->findData("+") );
    
    int    deg;
    double min;
    if (orientLat == "North+")    {
    	lat_NS->setCurrentIndex(lat_NS->findText("N"));
    }
    else if (orientLat == "South+")    {
    	lat_NS->setCurrentIndex(lat_NS->findText("S"));
		val = - val;
    }
    else {
		// Mode auto
		if (val > 0) {
	    	lat_NS->setCurrentIndex(lat_NS->findText("N"));
		}
		else {
	    	lat_NS->setCurrentIndex(lat_NS->findText("S"));
	    	val = -val;
		}
    }
	if (val < 0)
		lat_sign->setCurrentIndex( lat_sign->findText("-") );
		
	deg = (int) trunc(val);
	lat_degrees->setValue( abs(deg) );
	min = 60.0*fabs(val-trunc(val));
	lat_minutes->setValue( min );
}













