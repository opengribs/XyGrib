#ifndef ANGLECONVERTORWIDGET_H
#define ANGLECONVERTORWIDGET_H

#include <QWidget>
#include <QFrame>

#include "ui_AngleConverterWidget.h"

//===========================================================
class  AngleConverterWidget
	 : public QWidget,
	   private Ui::AngleConverterWidget
{
    Q_OBJECT
	public:
		AngleConverterWidget (QWidget *parent = 0, double angle=0);

	
	private slots:
		void   slotValueChanged ();
		
	private:
		void   displayValue (double angledeg, QObject *objsender);
		void   stopSignals  (bool b);
	
};

#endif
