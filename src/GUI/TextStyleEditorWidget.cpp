#include <QtGui>

#include "TextStyleEditorWidget.h"


//=================================================================================
TextDisplayLabel::TextDisplayLabel(QWidget *parent)
	 : QLabel(parent)
{
	parentWidget = parent;
	this->setText( (font().family()+" %1").arg(font().pointSizeF()) );
}
//----------------------------------------------------------------------
void TextDisplayLabel::changeFont (QFont ft)
{
	QString description = (ft.family()+" %1").arg(ft.pointSizeF());
	this->setText(description);
	this->setToolTip(description);
	this->setFont(ft);
	parentWidget->setMinimumHeight(ft.pointSizeF()+4);
}
//----------------------------------------------------------------------
void TextDisplayLabel::setColors (QColor textColor, QColor bgColor)
{
	this->textColor = textColor;
	this->bgColor   = bgColor;
	QPalette p = this->palette();
	p.setBrush(QPalette::Active,   QPalette::WindowText, textColor);
	p.setBrush(QPalette::Inactive, QPalette::WindowText, textColor);
	p.setBrush(QPalette::Active,   QPalette::Window, bgColor);
	p.setBrush(QPalette::Inactive, QPalette::Window, bgColor);
	this->setPalette(p);
	update();
}
//----------------------------------------------------------------------
void  TextDisplayLabel::mouseReleaseEvent(QMouseEvent *)
{
	bool ok;
	QFont font = QFontDialog::getFont(
                 &ok, this->font(), this);
	if (ok) {
		changeFont(font);
 	}
}


//=================================================================================
TextColorWidget::TextColorWidget(QWidget *parent)
	 : QWidget(parent)
{
	color = Qt::yellow;
}

//----------------------------------------------------------------------
void  TextColorWidget::mouseReleaseEvent(QMouseEvent *)
{
	// Open Choose color dialog
	QColor col = QColorDialog::getColor(color, this);
	if (col.isValid()) {
		color = col;
		update();
		emit colorChanged(color);
	}
}
//----------------------------------------------------------------------
void TextColorWidget::paintEvent(QPaintEvent *)
{
	QPainter pnt(this);
	pnt.fillRect(0,0, width(), height(), QBrush(color));
}


//=================================================================================
TextStyleEditorWidget::TextStyleEditorWidget (
					QWidget *parent,
					QFont  fonte,
					QFont  defaultFont,
					QColor textColor,
					QColor bgColor,
					QColor defaultTextColor,
					QColor defaultBgColor
		)
	: QWidget(parent)
{
    setupUi(this);
	this->defaultFont = defaultFont;
	this->defaultTextColor = defaultTextColor;
	this->defaultBgColor   = defaultBgColor;

	textColorWidget->setColor (textColor);
	bgColorWidget->setColor   (bgColor);

	testZoneLabel->setColors  (textColor, bgColor);
	testZoneLabel->changeFont (fonte);
	
	connect(resetButton, SIGNAL(clicked()), this, SLOT(slotRaz()));
	connect(textColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(slotColorChanged(QColor)));
	connect(bgColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(slotColorChanged(QColor)));
}

//---------------------------------------------------------------------------------------
void TextStyleEditorWidget::slotColorChanged(QColor)
{	
	testZoneLabel->setColors (textColorWidget->getColor(), bgColorWidget->getColor());
}

//---------------------------------------------------------------------------------------
void TextStyleEditorWidget::slotRaz()
{	
	textColorWidget->setColor(defaultTextColor);
	bgColorWidget->setColor  (defaultBgColor);
	
	testZoneLabel->setColors  (defaultTextColor, defaultBgColor);
	testZoneLabel->changeFont (defaultFont);
}


