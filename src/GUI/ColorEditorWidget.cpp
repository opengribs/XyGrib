#include <QtGui>

#include "ColorEditorWidget.h"


//=================================================================================
ColorTestZone::ColorTestZone(QWidget *parent)
	 : QWidget(parent)
{
	color = Qt::yellow;
}

//----------------------------------------------------------------------
void  ColorTestZone::mouseReleaseEvent(QMouseEvent *)
{
	// Open Choose color dialog
	QColor col = QColorDialog::getColor(color, this);
	if (col.isValid()) {
		color = col;
		update();
	}
}
//----------------------------------------------------------------------
void ColorTestZone::paintEvent(QPaintEvent *)
{
	QPainter pnt(this);
	pnt.fillRect(0,0, width(), height(), QBrush(color));
}


//=================================================================================
ColorEditorWidget::ColorEditorWidget( QWidget *parent,QColor color, QColor defaultColor)
    : QWidget(parent)
{
    setupUi(this);
	this->defaultColor = defaultColor;
	testZone->setColor(color);
	connect(resetButton, SIGNAL(clicked()), this, SLOT(slotRaz()));
}

//---------------------------------------------------------------------------------------
void ColorEditorWidget::slotRaz()
{	
	testZone->setColor(defaultColor);
}


