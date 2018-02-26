#ifndef LINEEDITORWIDGET_H
#define LINEEDITORWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QColorDialog>

//===========================================================
class LineTestZone : public QWidget
{
    Q_OBJECT
	public:
		LineTestZone(QWidget *parent = 0);
		
		QColor getLineColor()
					{return lineColor;}
		double getLineWidth()
					{return lineWidth;}
			
	public slots:
		void setLineColor(QColor c)
					{lineColor=c; update();}
		void setLineWidth(double w)
					{lineWidth=w; update();}
		
	private slots:
		void   paintEvent(QPaintEvent *);
		void   mouseReleaseEvent(QMouseEvent *);
	
	private:
		QColor lineColor;
		double lineWidth;
};

//===========================================================
#include "ui_LineEditorWidget.h"

//===========================================================
class /*QDESIGNER_WIDGET_EXPORT*/ LineEditorWidget
	 : public QWidget,
	   private Ui::LineEditorWidget
{
    Q_OBJECT
	public:
		LineEditorWidget
					(	QWidget *parent = 0,
						double width = 1.0,
						QColor color = Qt::black,
						double defaultWidth = 1.0,
						QColor defaultColor = Qt::black,
						double minWidth = 0.2,
						double maxWidth = 6
        			);
		
		QColor getLineColor()
					{return testZone->getLineColor();}
		double getLineWidth()
					{return testZone->getLineWidth();}
		
		void   setDefaultColor(QColor c)
					{defaultColor = c;}
		void   setDefaultWidth(double w)
					{defaultWidth = w;}
		
		void   setValues (double w,  const QColor &c,
						  double defw, const QColor &defc)
		{
			testZone->setLineWidth (w);
			testZone->setLineColor (c);
			defaultWidth = defw;
			defaultColor = defc;
			sbWidth->setValue (w);
		}


	private slots:
		void   slotRaz();
		
	private:
		QColor defaultColor;
		double defaultWidth;
		
};

#endif
