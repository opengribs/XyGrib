#ifndef COLOREDITORWIDGET_H
#define COLOREDITORWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QColorDialog>

//===========================================================
class ColorTestZone : public QWidget
{
    Q_OBJECT
	public:
		ColorTestZone(QWidget *parent = 0);
		
		void setColor(QColor c)
					{color=c; update();}
		QColor getColor()
					{return color;}
			
	private slots:
		void   paintEvent(QPaintEvent *);
		void   mouseReleaseEvent(QMouseEvent *);
	private:
		QColor color;
};

//===========================================================
#include "ui_ColorEditorWidget.h"

//===========================================================
class /*QDESIGNER_WIDGET_EXPORT*/ ColorEditorWidget
	 : public QWidget,
	   private Ui::ColorEditorWidget
{
    Q_OBJECT
	public:
		ColorEditorWidget(QWidget *parent = 0,
						  QColor color    = Qt::white,
						  QColor defaultColor = Qt::white );
		
		QColor getColor()
					{return testZone->getColor();}
		
 		void   setColor(QColor c)
 					{testZone->setColor(c);}
 					
 		void   setDefaultColor(QColor c)
 					{defaultColor = c;}

	private slots:
		void   slotRaz();
		
	private:
		QColor defaultColor;
};

#endif
