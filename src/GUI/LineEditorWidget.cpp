#include <QtGui>

#include "LineEditorWidget.h"


//=================================================================================
LineTestZone::LineTestZone(QWidget *parent)
	 : QWidget(parent)
{
	lineColor = Qt::yellow;
}

//----------------------------------------------------------------------
void  LineTestZone::mouseReleaseEvent(QMouseEvent *)
{
	// Open Choose color dialog
	QColor col = QColorDialog::getColor(lineColor, this);
	if (col.isValid()) {
		lineColor = col;
		update();
	}
}
//----------------------------------------------------------------------
void LineTestZone::paintEvent(QPaintEvent *)
{
	QPainter pnt(this);
	pnt.setRenderHint(QPainter::Antialiasing, true);
	
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(width()-1, 0));
 	linearGrad.setColorAt(0, Qt::black);
 	linearGrad.setColorAt(1, Qt::white);
	pnt.fillRect(0,0, width(), height(), QBrush(linearGrad));

	QPen pen(lineColor);
	pen.setWidthF(lineWidth);
	pnt.setPen(pen);
	pnt.drawLine(0,height()-1, width()-1,0);
}


//=================================================================================
LineEditorWidget::LineEditorWidget (
					QWidget *parent,
					double width, 		QColor color,
					double defaultWidth,	QColor defaultColor,
					double minWidth, double maxWidth
				)
    : QWidget(parent)
{
    setupUi(this);
	this->defaultColor = defaultColor;
	this->defaultWidth = defaultWidth;
	sbWidth->setRange(minWidth, maxWidth);
	sbWidth->setValue  (width);
	connect(resetButton, SIGNAL(clicked()), this, SLOT(slotRaz()));
	connect(sbWidth, SIGNAL(valueChanged(double)), testZone, SLOT(setLineWidth(double)));
	testZone->setLineColor(color);
	testZone->setLineWidth(width);
}

//---------------------------------------------------------------------------------------
void LineEditorWidget::slotRaz()
{	
	testZone->setLineColor (defaultColor);
	sbWidth->setValue  (defaultWidth);
}


