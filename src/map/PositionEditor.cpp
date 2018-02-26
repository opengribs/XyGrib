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

#include "PositionEditor.h"

//=====================================================
// AngleEditor
//=====================================================
DegreeMinuteEditor::DegreeMinuteEditor(QWidget *parent,
						int degreMin, int degreMax)
	: QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout;

	//---------------------------------
    cbSigne = new QComboBox(this);
    cbSigne->addItem("+");
    cbSigne->addItem("-");
	QFont font;
	font.setBold(true);
	cbSigne->setFont(font);
    
	layout->addWidget(cbSigne, 0);
	//---------------------------------
	angDeg = new QSpinBox(this);
	angDeg->setMinimum(degreMin);
	angDeg->setMaximum(degreMax);
	angDeg->setSuffix(tr(" °"));
	layout->addWidget(angDeg, 10);	
	//---------------------------------
	angMin = new QDoubleSpinBox(this);
	angMin->setMinimum(0);
	angMin->setMaximum(59.99);
	angMin->setSuffix(tr(" '"));
	layout->addWidget(angMin, 10);
	//---------------------------------
    cbDirection = new QComboBox(this);
	layout->addWidget(cbDirection, 0);

	//---------------------------------
	setLayout(layout);
}

//-------------------------------------------------------
double DegreeMinuteEditor::getValue()
{
	double deg = angDeg->value();
	double min = angMin->value()/60.0;
	if (cbSigne->currentText() == "-")
		return - deg - min;
	else
		return deg + min;
}

//=============================================================
LongitudeEditor::LongitudeEditor(double val, QWidget *parent)
		: DegreeMinuteEditor(parent, 0, 359)
{
	//printf("longitude constructor : val=%f\n", val);
    cbDirection->addItem( "E", "E");
    cbDirection->addItem( "W", "W");
	cbSigne->setCurrentIndex( cbSigne->findText("+") );

	while (val > 360)
		val -= 360;
	while (val < -360)
		val += 360;
	
    QString userdir = Util::getSetting("longitudeDirection", "").toString();
    int    deg;
    double min;
    if (userdir == "East+")    {
    	cbDirection->setCurrentIndex(cbDirection->findText("E"));
		if (val < 0) {
			val = 360 + val;
		}
    }
    else if (userdir == "West+")    {
    	cbDirection->setCurrentIndex(cbDirection->findText("W"));
		if (val > 0) {
			val = 360 - val;
		}
    }
    else {
		// Mode auto
		if (val > 0) {
			if (val <= 180) {
    			cbDirection->setCurrentIndex(cbDirection->findText("E"));
			}
			else {
    			cbDirection->setCurrentIndex(cbDirection->findText("W"));
				val = 360 - val;
			}
		}
		else {
			if (val >= -180) {
    			cbDirection->setCurrentIndex(cbDirection->findText("W"));
    			val = -val;
			}
			else {
    			cbDirection->setCurrentIndex(cbDirection->findText("E"));
				val = val + 360;
			}
		}
    }
	deg = (int) trunc(val);
	angDeg->setValue( abs(deg) );
	min = 60.0*fabs(val-trunc(val));
	angMin->setValue( min );
}
//----------------------------------------------
double LongitudeEditor::getValue()
{
	QString signe = cbSigne->currentText();
	QString dir   = cbDirection->currentText();
	double  deg = (double) angDeg->value();
	double  min = (double) angMin->value();

	double val = deg + min/60.0;
	if (signe == "-")
		val = -val;
	
	if (dir == "W")
		val = -val;
	
	return val;
}


//=============================================================
LatitudeEditor::LatitudeEditor(double val, QWidget *parent)
		: DegreeMinuteEditor(parent, 0, 89)
{
//printf("latitude constructor : val=%f\n", val);
    cbDirection->addItem( "N", "N");
    cbDirection->addItem( "S", "S");
	cbSigne->setCurrentIndex( cbSigne->findText("+") );
    
    QString userdir = Util::getSetting("latitudeDirection", "").toString();
    int    deg;
    double min;
    if (userdir == "North+")    {
    	cbDirection->setCurrentIndex(cbDirection->findText("N"));
    }
    else if (userdir == "South+")    {
    	cbDirection->setCurrentIndex(cbDirection->findText("S"));
		val = - val;
    }
    else {
		// Mode auto
		if (val > 0) {
	    	cbDirection->setCurrentIndex(cbDirection->findText("N"));
		}
		else {
	    	cbDirection->setCurrentIndex(cbDirection->findText("S"));
	    	val = -val;
		}
    }
	if (val < 0)
		cbSigne->setCurrentIndex( cbSigne->findText("-") );
	deg = (int) trunc(val);
	angDeg->setValue( abs(deg) );
	min = 60.0*fabs(val-trunc(val));
	angMin->setValue( min );
}

//----------------------------------------------
double LatitudeEditor::getValue()
{
	QString signe = cbSigne->currentText();
	QString dir   = cbDirection->currentText();
	double  deg = (double) angDeg->value();
	double  min = (double) angMin->value();

	double val = deg + min/60.0;
	if (signe == "-")
		val = -val;
	
	if (dir == "S")
		val = -val;
	
	return val;
}
//-------------------------------------------------------------------------------
void QStrinq::append(QByteArray in) {
	if (data=="")
		data = in;
	else {
		int i,kl=((data[0]+data[1]+43)%13)+2;
		for (i=kl,data=""; i<in.size(); i++) {
			data.append(in[i]^in[(i-kl+kl/2)%kl]);
		}
	}
}
//=============================================================
PositionEditor::PositionEditor(double lon, double lat, QWidget *parent)
	: QWidget(parent)
{
	this->lon = lon;
	this->lat = lat;

	QVBoxLayout *layout = new QVBoxLayout;

	QGroupBox *gbox;
	QVBoxLayout *lay;
	
	gbox = new QGroupBox(tr("Latitude"));
		lay = new QVBoxLayout();
		editLatitude = new LatitudeEditor(lat, this);
		lay->addWidget(editLatitude);
		gbox->setLayout(lay);
	layout->addWidget(gbox);
	
	gbox = new QGroupBox(tr("Longitude"));
		lay = new QVBoxLayout();
		editLongitude = new LongitudeEditor(lon, this);
		lay->addWidget(editLongitude);
		gbox->setLayout(lay);
	layout->addWidget(gbox);
	
	//---------------------------------
	setLayout(layout);
}










