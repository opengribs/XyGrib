#include <QtGui>
#include <cmath>

#include "AngleConverterWidget.h"

//=================================================================================
AngleConverterWidget::AngleConverterWidget( QWidget *parent, double angle)
    : QWidget(parent)
{
    setupUi(this);
	displayValue(angle, this);
	connect(dddmmss_deg, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
	connect(dddmmss_min, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
	connect(dddmmss_sec, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
	
	connect(dddmm_deg, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));	
	connect(dddmm_min, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
	
	connect(ddd_deg, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
	connect(ddd_rad, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
	connect(ddd_grad, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
}

//---------------------------------------------------------------------------------------
void   AngleConverterWidget::stopSignals (bool b)
{
	dddmmss_deg->blockSignals(b);
	dddmmss_min->blockSignals(b);
	dddmmss_sec->blockSignals(b);
	dddmm_deg->blockSignals(b);
	dddmm_min->blockSignals(b);
	ddd_deg->blockSignals(b);
	ddd_rad->blockSignals(b);
	ddd_grad->blockSignals(b);
}
//---------------------------------------------------------------------------------------
void AngleConverterWidget::slotValueChanged()
{
	QObject *ob = sender();
	double angledeg = 0;
		
	if (ob==dddmmss_deg||ob==dddmmss_min||ob==dddmmss_sec) {
		double deg = dddmmss_deg->value();
		double min = dddmmss_min->value();
		double sec = dddmmss_sec->value();
		angledeg = deg +min/60.0 +sec/3600.0;
	}
	else if (ob==dddmm_deg||ob==dddmm_min) {
		double deg = dddmm_deg->value();
		double min = dddmm_min->value();
		angledeg = deg +min/60.0;
	}
	else if (ob==ddd_deg) {
		angledeg = ddd_deg->value();
	}
	else if (ob==ddd_rad) {
		angledeg = ddd_rad->value()*180.0/M_PI;
	}
	else if (ob==ddd_grad) {
		angledeg = ddd_grad->value()*0.9;
	}

	displayValue(angledeg, ob);
}
//---------------------------------------------------------------------------------------
void AngleConverterWidget::displayValue(double angledeg, QObject *objsender)
{
	stopSignals(true);
	
	double degint =  floor(angledeg);
	double degpart = angledeg-degint;
	double minutes = degpart*60.0;
	
	int minint = (int) (minutes + 1e-10);
	
	double secondes = (minutes-minint)*60.0;
	if (secondes < 0.0)
		secondes = 0.0;
	if (secondes >= 60.0) {
		secondes -= 60.0;
		minint += 1;
	}
	
	if (objsender!=dddmmss_deg) dddmmss_deg->setValue ((int) (degint) );
	if (objsender!=dddmmss_min) dddmmss_min->setValue ((int) (minint));
	if (objsender!=dddmmss_sec) dddmmss_sec->setValue (secondes);
 	
 	if (objsender!=dddmm_deg) dddmm_deg->setValue ((int)degint);
 	if (objsender!=dddmm_min) dddmm_min->setValue (minutes);
	
	if (objsender!=ddd_deg)  ddd_deg->setValue(angledeg);
	if (objsender!=ddd_rad)  ddd_rad->setValue(angledeg*M_PI/180.0);
	if (objsender!=ddd_grad) ddd_grad->setValue(angledeg/0.9);
	
	stopSignals(false);
}


