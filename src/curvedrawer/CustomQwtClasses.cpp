/*
 *  CustomQwtClasses.cpp
 *  zyGrib
 *
 *  Created by Tim Holtschneider on 25.11.10.
 *
 */
#include "CustomQwtClasses.h"
#include "Util.h"

/*
 *=========================================================================================
 *   Custom Scale Draw Class
 *=========================================================================================
 */
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
CustomQwtScale::CustomQwtScale( const QVector<double> &qvOrig, const QVector<double> &qvNew ){
	myOrig = qvOrig;
	myRepl = qvNew;
}
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
CustomQwtScale::CustomQwtScale( const QVector<double> &qvOrig, const QVector<QString> &qvNew ){
	myOrig    = qvOrig;
	myReplStr = qvNew;
}
//-------------------------------------------------------------------------------
// Custom Label
//-------------------------------------------------------------------------------
QwtText CustomQwtScale::label( double val ) const {
	if (myOrig.contains( val ) ) {
		int iElement = myOrig.indexOf( val );
		
		if( myRepl.size() > 0)
		{
			return QString( "%1" ).arg( myRepl.at(iElement), 4, 's', 1 );
		} else if( myReplStr.size() > 0) {
			return myReplStr.at( iElement );
		}
	}
	return QString( "%1" ).arg( val, 4, 's', 1 );
}
/*
 *=========================================================================================
 *  Custom Plot Picker Class
 *=========================================================================================
 */
//-------------------------------------------------------------------------------
// Constructor for custom picker class with current date and picker machine
//-------------------------------------------------------------------------------
CustomQwtPicker::CustomQwtPicker( int xAxis, int yAxis, RubberBand rubberBand, QwtPickerMachine *pickerMachine, DisplayMode trackerMode,
								 QWidget *canvas, const QDateTime &dtCurrent, QwtPlotCurve *qwtCurve )
	: QwtPlotPicker( xAxis, yAxis, rubberBand, trackerMode, canvas )
{
	dtCurPos = dtCurrent;
	this->qwtCurve = qwtCurve;

	setStateMachine( pickerMachine );
	setRubberBandPen( QColor(64,64,124,255) );
    setTrackerPen( QColor(0, 0, 255, 127) );
	setKeyPattern(QwtEventPattern::KeySelect1, Qt::Key_unknown);
	setKeyPattern(QwtEventPattern::KeySelect2, Qt::Key_unknown);
	
	//	setEnabled( true );
}
//-------------------------------------------------------------------------------
// Draw a box beginning at the picked points of the plot canvas and the plot curve
//-------------------------------------------------------------------------------
void CustomQwtPicker::drawRubberBand(QPainter* painter) const
{

	// get start value of plot line and calculate delta to current position
//	if( selection().count() > 1 )
//	{
		updEndPoints( invTransform(selection().first()), invTransform(selection().last()) ) ;
		
		// draw vertical line at start position	
		painter->drawLine( selection().first().x(), selection().first().y(), 
						  transform(qFst).x(), transform(qFst).y() );
		
		
		// draw horizontal line
		painter->drawLine( selection().first().x(), selection().first().y(), 
						  selection().last().x(), selection().first().y() );
		
		// draw vertical lines at end position
		painter->drawLine( selection().last().x(), selection().first().y(), 
						  transform(qCur).x(), transform(qCur).y() );
//	}
}
//-------------------------------------------------------------------------------
// Display timestamp and delta of first plot line as tracker text
//-------------------------------------------------------------------------------
QwtText CustomQwtPicker::trackerTextF( const QPointF &pos ) const
{	
	float fXPos0, fXPos1, fDX, fDY;
	QString sText;
	uint iDays, iSecs;
	QDateTime qTPos0, qTPos1;
	
	fXPos0 = invTransform(selection().first()).x();
	fXPos1 = pos.x();

	// get start value of plot line and calculate delta to current position
	fDX = (qCur.x() - qFst.x());
	fDY = (qCur.y() - qFst.y());

	// calulate hours to add to current time position from plot position
	qTPos0 = dtCurPos;
	iDays = (uint)fXPos0 % 24;
	fXPos0 -= iDays * 24;
	iSecs = (uint)(fXPos0 * 3600.);
	qTPos0 = qTPos0.addSecs(iSecs);
	qTPos0 = qTPos0.addDays(iDays);
	
	qTPos1 = dtCurPos;
	iDays = (uint)fXPos1 % 24;
	fXPos1 -= iDays * 24;
	iSecs = (uint)(fXPos1 * 3600.);
	qTPos1 = qTPos1.addSecs(iSecs);
	qTPos1 = qTPos1.addDays(iDays);

		
	sText.sprintf( "%%1 %05.2f\n%%2 %05.2f\nDelta: %.2f Step: %.2f", qFst.y(), qCur.y(), fDY/fDX, fDX );
	sText.replace( QString("%1"), qTPos0.toString("dd.MM. hh:mm") );
	sText.replace( QString("%2"), qTPos1.toString("dd.MM. hh:mm") );
	
	return sText;
}
//-------------------------------------------------------------------------------
// Extrakt curve coordinates from picked points on the plot canvas
//-------------------------------------------------------------------------------
void CustomQwtPicker::updEndPoints( const QPointF &qPos0, const QPointF &qPos1 ) const
{
	// get start value of plot line and calculate delta to current position
	QVector<double> myXData = ((QwtPointArrayData *)qwtCurve->data())->xData();
	QVector<double>::iterator iterXData;
	QVector<double> myYData = ((QwtPointArrayData *)qwtCurve->data())->yData();
	QVector<double>::iterator iterYData;
	QPointF qCur0, qCur1, qFst0, qFst1;
	QPointF qCurRes, qFstRes;
	float fDelta, fOffset;
	
	for( iterXData = myXData.begin(), iterYData = myYData.begin(); iterXData != myXData.end(); iterXData++, iterYData++ ) {
		if( (*iterXData) < qPos1.x() ) {
			qCur0.setX( *iterXData );
			qCur1.setX( *(iterXData+1) );
			
			qCur0.setY( *iterYData );
			qCur1.setY( *(iterYData+1) );
		}
		if( (*iterXData) < qPos0.x() ) {
			qFst0.setX( *iterXData );
			qFst1.setX( *(iterXData+1) );
			
			qFst0.setY( *iterYData );
			qFst1.setY( *(iterYData+1) );
		}
	}
	
	fDelta = (qFst1.y()-qFst0.y())/(qFst1.x()-qFst0.x());
	fOffset = qFst0.y() - fDelta * qFst0.x();
	qFst.setY( fDelta * qPos0.x() + fOffset );
	qFst.setX( qPos0.x() );
	
	fDelta = (qCur1.y()-qCur0.y())/(qCur1.x()-qCur0.x());
	fOffset = qCur0.y() - fDelta * qCur0.x();
	qCur.setY( fDelta * qPos1.x() + fOffset );
	qCur.setX( qPos1.x() );
}
