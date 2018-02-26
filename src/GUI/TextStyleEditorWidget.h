#ifndef TEXTSTYLEEDITORWIDGET_H
#define TEXTSTYLEEDITORWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QColorDialog>
#include <QFontDialog>

//===========================================================
class TextDisplayLabel : public QLabel
{
    Q_OBJECT
	public:
		TextDisplayLabel(QWidget *parent = 0);
		
		void setColors  (QColor textColor, QColor bgColor);
		void changeFont (QFont ft);
		
		QColor getTextColor() {return textColor;}
		QColor getBgColor()   {return bgColor;}

		// getFont() is inherited from QLabel
	
	private slots:
		void   mouseReleaseEvent(QMouseEvent *);
	private:
		QWidget *parentWidget;
		QColor textColor, bgColor;
};

//===========================================================
class TextColorWidget : public QWidget
{
    Q_OBJECT
	public:
		TextColorWidget(QWidget *parent = 0);
		
		void setColor(QColor c)
					{color=c; update();}
		QColor getColor()
					{return color;}

	signals:
		void   colorChanged(QColor);
		
	private slots:
		void   paintEvent(QPaintEvent *);
		void   mouseReleaseEvent(QMouseEvent *);
	private:
		QColor color;
};

//===========================================================
#include "ui_TextStyleEditorWidget.h"

//===========================================================
class /*QDESIGNER_WIDGET_EXPORT*/ TextStyleEditorWidget
	 : public QWidget,
	   private Ui::TextStyleEditorWidget
{
    Q_OBJECT
	public:
		TextStyleEditorWidget ( QWidget *parent = 0,
					QFont  fonte       = QFont("Helvetica",10),
					QFont  defaultFont = QFont("Helvetica",10),
					QColor textColor = Qt::black,
					QColor bgColor   = Qt::white,
					QColor defaultTextColor = Qt::black,
					QColor defaultBgColor   =  Qt::white
			);

 		QColor getTextColor() {return testZoneLabel->getTextColor();}
 		QColor getBgColor()   {return testZoneLabel->getBgColor();}
 		QFont  getFont()      {return testZoneLabel->font();}

		void  setTextColor (QColor c) {
					testZoneLabel->setColors (c, testZoneLabel->getBgColor());
					textColorWidget->setColor(c);
				}
		void  setBgColor (QColor c) {
					testZoneLabel->setColors (testZoneLabel->getTextColor(), c);
					bgColorWidget->setColor(c);
				}
		
		void  setFont (QFont f) {testZoneLabel->changeFont(f);}

	private slots:
		void   slotRaz();
		void   slotColorChanged(QColor);
		
	private:
		QFont  defaultFont;
		QColor defaultTextColor;
		QColor defaultBgColor;

};

#endif
