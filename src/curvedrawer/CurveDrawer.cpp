/*
 *  CurveDrawer.cpp
 *  zyGrib
 *
 *  Created by Tim Holtschneider on 29.05.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#include <QMessageBox>
#include <QFileDialog>

#include "Orthodromie.h"
#include "CurveDrawer.h"

/*
 *=========================================================================================
 *   Curve Drawer
 *=========================================================================================
 */
//-------------------------------------------------------------------------------
// CurveDrawer: Constructor for single point
//-------------------------------------------------------------------------------
CurveDrawer::CurveDrawer( GriddedPlotter *plotter, 
						  double lon, double lat)
:	QWidget (NULL, Qt::Window) 
{
	setProperty ("objectType", "CurveDrawer");
	setFocusPolicy(Qt::StrongFocus);
	setWindowIcon (QIcon (Util::pathImg("zyGrib_32.xpm")));
	initObj();

	this->plotter	= plotter;
	this->lonStart	= lon;
	this->latStart	= lat;
	this->lonEnd    = 999.9;
	this->latEnd	= 999.9;
	this->bSingle   = true;

	initInterface();
	if( !privateConstructor() )
		return;
	
	connect( qwtLegend,	SIGNAL(checked(const QVariant &, bool, int)), 
			 this, SLOT( slotLegendChecked(const QVariant &, bool, int) ) );
	
	// display initial plot of wind speeds
	if( createTable( GRB_WIND_SPEED ) ) {
		initDataPlot( GRB_WIND_SPEED );
		this->show();
	}
}
//-------------------------------------------------------------------------------
// CurveDrawer: Constructor for two POIs
//-------------------------------------------------------------------------------
//CurveDrawer::CurveDrawer( GriddedPlotter *plotter, QWidget *parentWindow )
//:	QDialog(parentWindow)
//{	
//	// initialize current object
//	initObj();
//	
//	this->plotter	= plotter;
//	this->lonStart	= GLOB_listSelectedPOI.at(0)->getLongitude();
//	this->latStart	= GLOB_listSelectedPOI.at(0)->getLatitude();
//	this->lonEnd	= GLOB_listSelectedPOI.at(1)->getLongitude();
//	this->latEnd	= GLOB_listSelectedPOI.at(1)->getLatitude();
//	this->bSingle	= false;
//	
//	// initialize GUI
//	initInterface();
//	
//	// initialize boat parameters for calulation of track
//	QString fileName = Util::getSetting("boatParameterFileName", "").toString();
//	myPolar = new Polar( fileName );
//	
//	if( !privateConstructor() )
//		return;
//	
//	// display list of POIs in combo box
//	QList<POI*>::iterator iterPOI;
//	POI *fstPOI, *sndPOI;
//	for( iterPOI = GLOB_listSelectedPOI.begin(); iterPOI != GLOB_listSelectedPOI.end(); iterPOI++ ) {
//		if( (*iterPOI)->isValid() ) {
//			fstPOI = *iterPOI;
//			sndPOI = *(++iterPOI);
//			if( iterPOI != GLOB_listSelectedPOI.end() && (*iterPOI)->isValid() ) {
//				myToolBar->cbWayPoints->addItem( 
//					QString("%1-%2").arg(fstPOI->getName()).arg(sndPOI->getName()), fstPOI->getCode() );
//				iterPOI--;
//			} else {
//				break;
//			}
//		} else {
//		   break;
//		}
//	}
//	// display initial plot of wind speeds
//	if( createTablePOI( GRB_WIND_SPEED ) ) {
//		initDataPlot( GRB_WIND_SPEED );
//		this->show();
//	}
//}
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
CurveDrawer::~CurveDrawer()
{
	for( int i=0; i<4; i++ )
		if( qwtCurve[i] ) { delete qwtCurve[i]; qwtCurve[i] = NULL; }
	for( int i=0; i<2; i++ ) {
		if( qwtMarker[i] ) { delete qwtMarker[i]; qwtMarker[i] = NULL; }
	}
	if( qwtDataPlot ) {delete qwtDataPlot; qwtDataPlot = NULL;};
	// OpenCPN, polar boat parameters
//	if( myPolar ) {delete myPolar;};	
}
//-------------------------------------------------------------------------------
// create data tables for single point
//-------------------------------------------------------------------------------
bool CurveDrawer::createTable( const int &iDataChosen ) 
{
	bool bFirst;
	float dDir, dDirH, dStep;
	dDir = 0; dDirH = 0; bFirst = true;
	time_t tCurDateRec = reader->getClosestDateFromNow();
	
	if( !qvY1Axis.isEmpty() )		{qvY1Axis.clear();};
	if( !qvDates.isEmpty() )		{qvDates.clear();};
	if( !qvHoursFromNow.isEmpty() )	{qvHoursFromNow.clear();};
	if( !qvWindDir.isEmpty() )		{qvWindDir.clear();};
	if( !qvWindDev.isEmpty() )		{qvWindDev.clear();};
	if( !qvTempMin.isEmpty() )		{qvTempMin.clear();};
	if( !qvTempMax.isEmpty() )		{qvTempMax.clear();};

	
	//int iNumDates = reader->getNumberOfDates();
	std::set<time_t> sdates = reader->getListDates();
	std::set<time_t>::iterator iter;
	// calculate step size
	dStep = (*(sdates.rbegin()) - *(sdates.begin()))/3600. / sdates.size();
	
	//-----------------------------------------------
	// Titre 1 : une colonne par date+horaires
	//-----------------------------------------------
	for (iter=sdates.begin(), iDataCnt=0; iter!=sdates.end(); iter++, iDataCnt++) {
		time_t date = *iter;
		// Grib data for this point and this date
		DataPointInfo *pinfo = new DataPointInfo( reader, lonStart,latStart, date );

		// calculate date points for x axis
		qvHoursFromNow << (double) ((date - tCurDateRec)/3600.);
		QDateTime dt = Util::applyTimeZone( date );
		qvDates << dt.toString("dd.MM.yy hh.mm");
		
		// add new data records to curves for display at current step position
		appendCurves( iDataChosen, pinfo );
		
		// evaluate data points for y axis to the right, currently fixed to wind direction
		// and derivative of wind direction
		dDirH = pinfo->windDir_10m;
		qvWindDir << pinfo->windDir_10m;

		// avoid peak in derivative of first data point
		if( bFirst ) { bFirst = false; dDir = dDirH; }
		
		// mangle 360 degree periodicity
		qvWindDev << getInboundAngle(dDirH, dDir, true) / dStep;
		dDir = dDirH;
		delete pinfo;
	}
		
	// set initial display
	qvXAxis  = &qvHoursFromNow;
	qvY2Axis = &qvWindDir;
	return true;
}
//-------------------------------------------------------------------------------
// create data tables for POIs
//-------------------------------------------------------------------------------
// OpenCPN
//bool CurveDrawer::createTablePOI( const int &iDataChosen ) 
//{
//	bool bFirst;
//	double dDir, dDirH, tmpLon, tmpLat;
//	int iZero = 0;
//	dDir = 0; dDirH = 0; bFirst = true; tmpLon = 0; tmpLat = 0;
//	time_t tCurDateRec = reader->getClosestDateFromNow();
//	time_t tLastDate = *reader->getListDates().rbegin();
//	
//	if( !qvY1Axis.isEmpty() )		{qvY1Axis.clear();};
//	if( !qvDates.isEmpty() )		{qvDates.clear();};
//	if( !qvHoursFromNow.isEmpty() )	{qvHoursFromNow.clear();};
//	if( !qvWindDir.isEmpty() )		{qvWindDir.clear();};
//	if( !qvWindDev.isEmpty() )		{qvWindDev.clear();};
//	if( !qvTempMin.isEmpty() )		{qvTempMin.clear();};
//	if( !qvTempMax.isEmpty() )		{qvTempMax.clear();};
//
//	// get initial course and distance between the two POIs
//	Orthodromie *myOrtho = new Orthodromie( lonStart, latStart, lonEnd, latEnd );
//
//	time_t tTimeStep = reader->getClosestDateFromNow();
//	time_t tDataStep = reader->getClosestDateFromNow();
//
//	double px = lonStart;
//	double py = latStart;
//	double dBoatAngle;
//	// fixed time step size of 1 hour
//	double dDist = 0, dDistStep = 6., dTimeStep = 1.;
//	double dRemain;
//	
//	dRemain = myOrtho->getDistance();
//	dBoatAngle = myOrtho->getAzimutDeg();
//	// loop while end of data is not jet reached and remaining distance > 1 mile
//	while( dRemain > 1. && tDataStep < tLastDate ) {
//
//		DataPointInfo *pinfo;
//		dRemain = myOrtho->getDistance();
//		
//		if( reader->getFirstRecord()->isPointInMap(px, py) ) {
//			
//			// get data point info at a valid date
//			pinfo = new DataPointInfo( reader, px, py, tDataStep );
//
//			// start plot at current time		
//			qvHoursFromNow << (double) ((tTimeStep - tCurDateRec) / 3600.);
//			qvDist  << dDist;
//
//			// get way made good in one hour under current wind conditions
//			// get boat speed for current wind boat parameters
//			dDistStep = myPolar->getSpeed( pinfo->windSpeed_10m, getInboundAngle( dBoatAngle, pinfo->windDir_10m, true) );
//			if( dDistStep <= 0 ) {
//				iZero += 1;
//				if (iZero >= 5 ) {
//					QMessageBox::warning(this, tr("Warning"),tr("No sufficient wind to travel waypoint route!"));
//					return false;
//				}
//				dDistStep = 0.00001;
//			}
//			// if end point is close by adjust step size in distance
//			if( dDistStep > myOrtho->getDistance() ) {
//				dDistStep = myOrtho->getDistance();
//			}			
//			dDist   += dDistStep;
//			
//			// add new data records to curves for display at current step position
//			appendCurves( iDataChosen, pinfo );
//			
//			
//			// evaluate data points for y axis to the right, currently fixed to wind direction
//			// and derivative of wind direction
//			dDirH = pinfo->windDir_10m;
//			qvWindDir << pinfo->windDir_10m;
//			
//			// avoid peak in derivative of first data point
//			if( bFirst ) { bFirst = false; dDir = dDirH; }
//			// assume that change of wind direction is always the smaller angle 
//			qvWindDev << getInboundAngle(dDirH, dDir, false) / dTimeStep;
//			dDir = dDirH;
//			
//			delete pinfo;
//		} else {
//			break;
//		}
//
//		// get next data point on path between start and end point		
//		myOrtho->getCoordsForDist(px, py, dDistStep, dBoatAngle, &tmpLon, &tmpLat );
//		myOrtho->setPoints( tmpLon, tmpLat, lonEnd, latEnd );
//		dBoatAngle = myOrtho->getAzimutDeg();
//		px = tmpLon;
//		py = tmpLat;
//
//		// get next date in 1 hour steps
//		tTimeStep += (time_t)(dTimeStep * 3600.);
//		tDataStep = reader->getClosestDateFromDate( tTimeStep );
//	}
//
//	delete myOrtho;
//	
//	// set initial display
//	qvXAxis  = &qvHoursFromNow;
//	qvY2Axis = &qvWindDev;
//	return true;
//}

//-------------------------------------------------------------------------------
// init all data strcture for plotting
//-------------------------------------------------------------------------------
void CurveDrawer::initDataPlot( const int &iDataChosen )
{
	//------------------------------------------------------------------	
	// set up x-axis
	//
	qwtDataPlot->setAxisTitle( QwtPlot::xBottom, tr("hours from now") );
	// check whether historical data is loaded
	if( bSingle ) {
		if( (tCurDate - tRefDate) < (time_t)604800 ) {
			myToolBar->setChkCurrent( true );
			slotCurrentClicked( true );
		} else {
			myToolBar->setChkCurrent( false );
			slotCurrentClicked( false );
		}
	}
	
	// set up bottom and top x-axis
	setupXAxis( qvXAxis->first(), qvXAxis->last() );	
	
	// set up left y-axis
	qwtDataPlot->setAxisTitle( QwtPlot::yLeft, getPlotUnit( iDataChosen ) );
	qwtDataPlot->setAxisScaleEngine( QwtPlot::yLeft, qwtYScale[0] );
	qwtDataPlot->setAxisAutoScale( QwtPlot::yLeft );
	
	// set up right y-axis
	qwtDataPlot->setAxisTitle( QwtPlot::yRight, tr("degree") );
	qwtDataPlot->enableAxis( QwtPlot::yRight, true );
	qwtDataPlot->setAxisScale( QwtPlot::yRight, 0., 360.1, 30.  );
	//	qwtDataPlot->setAxisScaleEngine( QwtPlot::yRight, qwtYScale[1] );
	qwtDataPlot->setAxisAutoScale( QwtPlot::yRight );
		
	//------------------------------------------------------------------
	// set up plot for bottom left plot
	//
	qwtCurve[0]->setPen( QPen( QColor(0,0,255,127) ) );
	qwtCurve[0]->setStyle( QwtPlotCurve::Lines );
	//	qwtCurve[0]->setCurveAttribute( QwtPlotCurve::Fitted );
	qwtCurve[0]->setSamples( *qvXAxis, qvY1Axis );
	qwtCurve[0]->attach( qwtDataPlot );

	//------------------------------------------------------------------	
	// set up plot for top right plot
	//
	qwtCurve[1]->setAxes( QwtPlot::xBottom, QwtPlot::yRight );
	qwtCurve[1]->setPen( QPen( QColor(32,32,0,127) ) );
	qwtCurve[1]->setBrush( QBrush( QColor(32,32,32,20), Qt::SolidPattern ) );
	qwtCurve[1]->setBaseline(0.);
	qwtCurve[1]->setStyle( QwtPlotCurve::Steps );

	// plot distances in waypoint plot (two points given in constructor)	
	if( lonEnd < 999 ) {
		qwtCurve[1]->setSamples( qvDist, *qvY2Axis );
	} else {
		qwtCurve[1]->setSamples( *qvXAxis, *qvY2Axis );
	}
	qwtCurve[1]->attach( qwtDataPlot );
	
	//------------------------------------------------------------------	
	// set up 3rd plot on left y-axis for temperature plot 
	//
	qwtCurve[2]->setAxes( QwtPlot::xBottom, QwtPlot::yLeft );
	qwtCurve[2]->setPen( QPen( QBrush( QColor(255,0,0,132), Qt::SolidPattern ), 1.4, 
							  Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin ) ); /* color changed by francois-x */
	qwtCurve[2]->setStyle( QwtPlotCurve::Lines );
	if( reader->hasDataType( GRB_WIND_GUST )) {
		qwtCurve[2]->setTitle( tr("Wind Gust") );
		qwtCurve[2]->setSamples( *qvXAxis, qvTempMin );
		qwtCurve[2]->attach( qwtDataPlot );
	}
	
	//------------------------------------------------------------------	
	// set up 4th plot on left y-axis for temperature plot
	//
	qwtCurve[3]->setAxes( QwtPlot::xBottom, QwtPlot::yLeft );
	qwtCurve[3]->setPen( QPen( QColor(255,0,0,128) ) );
	qwtCurve[3]->setStyle( QwtPlotCurve::Lines );
	
		
	//------------------------------------------------------------------
	// set up marker and picker in plot
	//
	// draw marker line at current date
	QDateTime dt = Util::applyTimeZone( reader->getClosestDateFromNow() );
	QPen myPen = QPen( QColor(2,2,0,10) );
	myPen.setDashOffset( 1. );
	qwtMarker[0]->setLineStyle( QwtPlotMarker::VLine );
	qwtMarker[0]->setLinePen( QPen( Qt::DashDotDotLine ) );
	qwtMarker[0]->setLabelAlignment( Qt::AlignTop | Qt::AlignRight );

	QString sPos = dt.toString( "dd.MM. hh:mm\n" );
	sPos += Util::formatLongitude( lonStart ).replace("'", "''");
	sPos += "\n";
	sPos += Util::formatLatitude( latStart ).replace("'", "''");
	qwtMarker[0]->setLabel( sPos );
	qwtMarker[0]->setXValue(0);
	qwtMarker[0]->attach( qwtDataPlot );

	// draw marker line at end point
	if( !bSingle ) {
		sPos = "\n";
		sPos += Util::formatLongitude( lonEnd ).replace("'", "''");
		sPos += "\n";
		sPos += Util::formatLatitude( latEnd ).replace("'", "''");
		qwtMarker[1]->setLabel( sPos );
		qwtMarker[1]->setXValue( qvHoursFromNow.last() );
		qwtMarker[1]->setLineStyle( QwtPlotMarker::VLine );
		qwtMarker[1]->setLinePen( QPen() );
		qwtMarker[1]->setLabelAlignment( Qt::AlignTop | Qt::AlignLeft );
		qwtMarker[1]->attach( qwtDataPlot );
	}
	
	// setup picker
	qwtPicker = new CustomQwtPicker(QwtPlot::xBottom, QwtPlot::yLeft,
									QwtPlotPicker::CrossRubberBand,
									new QwtPickerClickRectMachine,
									QwtPicker::ActiveOnly,									
									qwtDataPlot->canvas(), 
									dt,
									qwtCurve[0]);
	// connect signals
//	connect(qwtPicker, SIGNAL(selected(const QwtDoublePoint &)), qwtPicker, SLOT(selected(const QwtDoublePoint &)));
	
	//------------------------------------------------------------------
	// set up legend, activate curves
	//
	qwtLegend->setDefaultItemMode( QwtLegendData::Checkable );
	qwtLegend->setLineWidth(10);
	qwtDataPlot->insertLegend( qwtLegend, QwtPlot::BottomLegend );
	activateLegend( qwtCurve[0], true );
	activateLegend( qwtCurve[1], false );
	activateLegend( qwtCurve[2], true );	

	qwtDataPlot->replot();
}

//-------------------------------------------------------------------------------
// Create data plot
// todo: method must be redone
//-------------------------------------------------------------------------------
void CurveDrawer::createDataPlot( const int &iDataChosen, const QString &sTitle )
{
	QString sUnitYLeft;
	
	qwtCurve[0]->setSamples( *qvXAxis, qvY1Axis );
	qwtCurve[0]->itemChanged();
	qwtCurve[0]->setTitle( sTitle );
	sUnitYLeft = getPlotUnit( iDataChosen );
	qwtDataPlot->setAxisTitle( QwtPlot::yLeft, sUnitYLeft );
	activateLegend( qwtCurve[0], true );
	
	// add min and max curve for temperature plot
	if( iDataChosen == GRB_TEMP ) {
	  	  qwtCurve[1]->detach(); 		  		  /* ADDED BY francois-x */
		  qwtCurve[0]->setPen( QPen( QColor(0,0,255,255) ) ); /* ADDED BY francois-x */
        if( reader->hasDataType( GRB_TMIN ) ) {
	    qwtCurve[2]->setTitle( tr("Temp Min") );
	    qwtCurve[2]->setPen( QPen( QColor(0,0,255,128) ) );
            qwtCurve[2]->setSamples( *qvXAxis, qvTempMin );
            qwtCurve[2]->attach( qwtDataPlot );
            activateLegend( qwtCurve[2], true );
        } else {
		qwtCurve[2]->detach(); 	              		/* ADDED BY francois-x */
	}
        if( reader->hasDataType( GRB_TMAX ) ) {
            qwtCurve[3]->setSamples( *qvXAxis, qvTempMax );
            qwtCurve[3]->attach( qwtDataPlot );
			activateLegend( qwtCurve[3], true );
        } else {
		qwtCurve[3]->detach();					/* ADDED BY francois-x */
	}
	
	
	} else if( iDataChosen == GRB_WIND_SPEED ) {
	qwtCurve[0]->setPen( QPen( QColor(0,0,255,128) ) );
	qwtCurve[3]->detach();						/* ADDED BY francois-x */
	qwtCurve[1]->setTitle( tr("Wind Deviation") );		/* ADDED BY francois-x */
	qwtCurve[1]->setSamples( *qvXAxis, *qvY2Axis );		/* ADDED BY francois-x */
	qwtCurve[1]->attach( qwtDataPlot );		   		/* ADDED BY francois-x */
	activateLegend( qwtCurve[1], false );	   	   		/* ADDED BY francois-x */
	
        if( reader->hasDataType( GRB_WIND_GUST ) ) {
	    qwtCurve[2]->setTitle( tr("Wind Gust") );
	    qwtCurve[2]->setPen( QPen( QBrush( QColor(255,0,0,132), Qt::SolidPattern ), 1.4, 
							  Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin ) ); /* ADDED BY francois-x */
            qwtCurve[2]->setSamples( *qvXAxis, qvTempMin );
            qwtCurve[2]->attach( qwtDataPlot );
            activateLegend( qwtCurve[2], true );
        }
        
	} else {
		qwtCurve[0]->setPen( QPen( QColor(0,0,0,255) ) );	/* ADDED BY francois-x */
		qwtCurve[1]->detach(); 		   		/* ADDED BY francois-x */
		qwtCurve[2]->detach();
		qwtCurve[3]->detach();
	}

	qwtDataPlot->replot();
}
//-------------------------------------------------------------------------------
// slots for button events
//-------------------------------------------------------------------------------
void CurveDrawer::slotExitClicked()		{ delete this; }
//-------------------------------------------------------------------------------
// Change from current time to date display on x-axis
//-------------------------------------------------------------------------------
void CurveDrawer::slotFileSelector() 
{
	QString fileName = Util::getSetting("boatParameterFileName", "").toString();
	QFileInfo finfo(fileName);
	fileName = QFileDialog::getOpenFileName( NULL,
									tr("Choose a boat parameter file"), finfo.absolutePath(), "");

    if (fileName != "" && fileName != Util::getSetting("boatParameterFileName", "").toString() )
	{
		// save new file in settings
    	Util::setSetting( "boatParameterFileName", fileName );
		// update polar data from new file
    }
}
//-------------------------------------------------------------------------------
// Change from current time to date display on x-axis
//-------------------------------------------------------------------------------
void CurveDrawer::slotCurrentClicked( bool bState )
{
	if (bState) {
		// display graph starting from current time
		//		double dTimeStep = getXAxisStep( (*qvXAxis).first(), (*qvXAxis).last());
		qwtDataPlot->setAxisScale( QwtPlot::xBottom, qvXAxis->first(), qvXAxis->last(), 12. );
		qwtDataPlot->setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw() );
	} else {
		// display graph with historical data (x-axis in date format)
		qwtDataPlot->setAxisScale( QwtPlot::xBottom, qvXAxis->first(), qvXAxis->last(), 24. );			
		qwtDataPlot->setAxisScaleDraw(QwtPlot::xBottom, new CustomQwtScale( *qvXAxis, qvDates ) );
	}
	qwtDataPlot->replot();	
}
//-------------------------------------------------------------------------------
// slot for new data is chosen from drop-down box
//-------------------------------------------------------------------------------
void CurveDrawer::slotDataChosen( int iDataChosen )
{
	int iData;
	QString sTitle;
	if( iDataChosen==0 ) {
		iData	= myToolBar->cbData->itemData( myToolBar->cbData->currentIndex() ).toInt();
		sTitle	= myToolBar->cbData->itemText( myToolBar->cbData->currentIndex() );
	} else {
		iData	= myToolBar->cbData->itemData( iDataChosen ).toInt();
		sTitle	= myToolBar->cbData->itemText( iDataChosen );
	}
	
	createTable( iData );
	createDataPlot( iData, sTitle );
}
//-------------------------------------------------------------------------------
// slot: new track in drop-down box selected
//-------------------------------------------------------------------------------
void CurveDrawer::slotSelectTrack( int iSelectedTrack )
{
	int iData;	
	QString sTitle;
	iData = myToolBar->cbData->itemData( myToolBar->cbData->currentIndex() ).toInt();
	sTitle = myToolBar->cbData->itemText( myToolBar->cbData->currentIndex() );

	if( iSelectedTrack == 0 ) {
		this->lonStart	= GLOB_listSelectedPOI.at(0)->getLongitude();
		this->latStart	= GLOB_listSelectedPOI.at(0)->getLatitude();
		this->lonEnd	= GLOB_listSelectedPOI.at(1)->getLongitude();
		this->latEnd	= GLOB_listSelectedPOI.at(1)->getLatitude();		
	} else {
		QList<POI*>::iterator iterPOI;
		POI *fstPOI = NULL, *sndPOI = NULL;
		for( iterPOI = GLOB_listSelectedPOI.begin(); iterPOI != GLOB_listSelectedPOI.end(); iterPOI++ ) {
			fstPOI = *iterPOI;
			if( fstPOI->getCode() == myToolBar->cbWayPoints->itemData(iSelectedTrack, Qt::UserRole).toUInt() ) {
				sndPOI = *(++iterPOI);
				break;
			}
		}
		if( sndPOI == NULL ) {
			fstPOI = GLOB_listSelectedPOI.at(0);
			sndPOI = GLOB_listSelectedPOI.at(1);
		}
			
		this->lonStart	= fstPOI->getLongitude();
		this->latStart	= fstPOI->getLatitude();
		this->lonEnd	= sndPOI->getLongitude();
		this->latEnd	= sndPOI->getLatitude();
	}
	
	// OpenCPN
//	createTablePOI( iData );
	setupXAxis( qvXAxis->first(), qvXAxis->last() );
	createDataPlot( iData, sTitle );
}
//-------------------------------------------------------------------------------
// slot for legend item checking
// toggle legend item check box and do replot
//-------------------------------------------------------------------------------
void CurveDrawer::slotLegendChecked(const QVariant &itemInfo, bool on, int /*index*/)
{
	QwtPlotItem *item = qwtDataPlot->infoToItem(itemInfo);
	activateLegend( item, on );
	qwtDataPlot->replot();	
}
//-------------------------------------------------------------------------------
// check / decheck legend without a replot
//-------------------------------------------------------------------------------
void CurveDrawer::activateLegend( QwtPlotItem *pItem, bool on )
{
	// switch between wind direction and derivateive of direction for Y2 axis
	if( pItem == qwtCurve[1] ) {
		if( qvY2Axis == &qvWindDir ) {
			qvY2Axis = &qvWindDev;
			qwtCurve[1]->setTitle( tr("Wind Deviation") );
			qwtDataPlot->setAxisTitle( QwtPlot::yRight, tr("deg/h") );
			qwtDataPlot->setAxisAutoScale( QwtPlot::yRight );
		} else {
			qvY2Axis = &qvWindDir;
			qwtCurve[1]->setTitle( tr("Wind Direction") );
			qwtDataPlot->setAxisTitle( QwtPlot::yRight, tr("degree") );
			qwtDataPlot->setAxisScale( QwtPlot::yRight, 0., 360.1, 30.  );
			qwtDataPlot->setAxisScaleEngine( QwtPlot::yRight, NULL );
		}
		qwtCurve[1]->setSamples( *qvXAxis, *qvY2Axis );
		on = false;
	// toggle legend item check box
	} else {
		pItem->setVisible( on );
	}
	
	// check legend button
	QWidget *w = qwtLegend->legendWidget(qwtDataPlot->itemToInfo(pItem));
	QwtLegendLabel *lgdItem = dynamic_cast<QwtLegendLabel*>(w);
	if (lgdItem) {
		lgdItem->setChecked( on );
	}
}
//-------------------------------------------------------------------------------
// get data unit for axis title
//-------------------------------------------------------------------------------
QString CurveDrawer::getPlotUnit( const int &iDataChosen )
{
	QString sUnit;
	switch( iDataChosen ) {
		case GRB_WIND_SPEED:
		case GRB_WIND_GUST:
			sUnit = Util::getSetting( "unitsWindSpeed", "" ).toString();
			break;
		case GRB_TEMP:
			sUnit = Util::getSetting( "unitsTemp", "" ).toString();
			break;
		case GRB_PRESSURE_MSL:
			sUnit = tr("hPa"); 
			break;
		case GRB_CLOUD_TOT:
			sUnit = tr("Percent [%]");
			break;
		case GRB_PRECIP_RATE:
			sUnit = tr("mm/h");
			break;
		case GRB_PRECIP_TOT:
			sUnit = tr("mm/h");
			break;			
		default:
			sUnit = "";
	}	
	return sUnit;
}
//-------------------------------------------------------------------------------
// Append data to the curves based on chosen curvesfor display
//-------------------------------------------------------------------------------
void CurveDrawer::appendCurves( const int &iDataChosen, const DataPointInfo *pinfo )
{
	if( iDataChosen == GRB_WIND_SPEED ) {
		qvY1Axis << convertData( GRB_WIND_SPEED,pinfo->windSpeed_10m);
		if( reader->hasDataType( GRB_WIND_GUST ) ) {
			qvTempMin << convertData( GRB_WIND_GUST, pinfo->getDataValue( DataCode(GRB_WIND_GUST,LV_ABOV_GND,0) ));
		}
	} else if( iDataChosen == GRB_TEMP ) {
		qvY1Axis  << convertData( GRB_TEMP, pinfo->getDataValue( DataCode(GRB_TEMP,LV_ABOV_GND,2) ));
		if( reader->hasDataType( GRB_TMAX ) ) {
			qvTempMax << convertData( GRB_TMAX, pinfo->getDataValue( DataCode(GRB_TMAX,LV_ABOV_GND,2) ));
		}
		if( reader->hasDataType( GRB_TMIN )) {
			qvTempMin << convertData( GRB_TMIN, pinfo->getDataValue( DataCode(GRB_TMIN,LV_ABOV_GND,2) ));
		}
	} else {
		qvY1Axis  << convertData( iDataChosen, pinfo->getDataValue( DataCode(iDataChosen,LV_ABOV_GND,0) ));
	}
}
//-------------------------------------------------------------------------------
// Convert data to display plot corespondingly to user settings
//-------------------------------------------------------------------------------
float CurveDrawer::convertData( const int &iDataKey, const float &fDataVal ) {
    float   fVal;
    QString sUnit;
    bool bConv;
    
    switch( iDataKey ) {
        case GRB_WIND_SPEED:
		case GRB_WIND_GUST:
			fVal = Util::formatSpeed_Wind( fDataVal, false).toFloat( &bConv );
			return bConv ? fVal : fDataVal;
            
        case GRB_TEMP:
        case GRB_TMIN:
        case GRB_TMAX:            
            return Util::convertTemperature( fDataVal );
            
        case GRB_PRESSURE_MSL:
            fVal = Util::formatPressure( fDataVal, false, 0 ).toFloat( &bConv );
            return bConv ? fVal : fDataVal;
            
        default:
            return fDataVal;
    }
}
//-------------------------------------------------------------------------------
// Calculate inbound angle between two directions
//		bool parameter controls return of absulte difference
//-------------------------------------------------------------------------------
float CurveDrawer::getInboundAngle( const float &fAngle1, const float &fAngle2, bool bAbs )
{
	float fDiff;
	float sfDiff;
	sfDiff = 0.;
	fDiff = bAbs ? qAbs(fAngle1 - fAngle2) : (fAngle1 - fAngle2);
	if (fmod(fAngle1+fDiff,360.) == fAngle2)
	  sfDiff = 1.;
	else
	  sfDiff = -1.;
	if( qAbs(fAngle1 - fAngle2) > 180. )
		return (sfDiff*(360. - fDiff));
	else
		return (sfDiff*fDiff);
}
//-------------------------------------------------------------------------------
// additional constructor
//-------------------------------------------------------------------------------
bool CurveDrawer::privateConstructor()
{
	setWindowTitle(tr("Plot data of chosen point"));
	
    // checks for GRIB data
    if( !plotter->isReaderOk() ) {
        QMessageBox::critical (this, tr("Error"),tr("Can't create dataplot:\n\nNo GRIB file opened."));
		delete this;
		return false;
    }
	reader = plotter->getReader();
	if( reader == NULL ) {
		QMessageBox::critical (this, tr("Error"),tr("Can't create dataplot:\n\nNo GRIB file opened."));
		delete this;
		return false;	
	}
    GriddedRecord *record = reader->getFirstRecord();
	if( record == NULL ) {
        QMessageBox::critical (this, tr("Error"),tr("Can't create dataplot\n\nGRIB area undefined."));
		delete this;
		return false;
    }
	if( !record->isPointInMap(lonStart, latStart) ) {
		QMessageBox::critical (this, tr("Error"),tr("Can't create dataplot:\n\nPoint outside GRIB area."));
		delete this;
		return false;
	}
	
	// get metadata of GRIB data
	tRefDate = record->getRecordRefDate();
	tCurDate = time(NULL);
	
	//----------------------------------------------
	// Constructor of for all used objects
	qwtCurve[0]  = new QwtPlotCurve( tr("Wind Speed") );
	qwtCurve[1]  = new QwtPlotCurve( tr("Wind Direction") );
	qwtCurve[2]  = new QwtPlotCurve( tr("Temp Min") );
	qwtCurve[3]  = new QwtPlotCurve( tr("Temp Max") );	
	qwtYScale[0] = new QwtLinearScaleEngine();
	qwtYScale[1] = new QwtLinearScaleEngine();
	qwtMarker[0] = new QwtPlotMarker();
	qwtMarker[1] = new QwtPlotMarker();	
	qwtLegend	 = new QwtLegend();
	
	// list available data types ion combo box
	if( reader->hasDataType( GRB_WIND_VX ) )		{ myToolBar->cbData->addItem(tr("Wind"), GRB_WIND_SPEED ); };	
	if( reader->hasDataType( GRB_PRESSURE_MSL ) )	{ myToolBar->cbData->addItem(tr("Pressure"), GRB_PRESSURE_MSL ); };
	if( reader->hasDataType( GRB_TEMP ) )			{ myToolBar->cbData->addItem(tr("Temperature"), GRB_TEMP ); };
	if( reader->hasDataType( GRB_CLOUD_TOT ) )		{ myToolBar->cbData->addItem(tr("Cloud cover"), GRB_CLOUD_TOT ); };
	if( reader->hasDataType( GRB_PRECIP_RATE ))		{ myToolBar->cbData->addItem(tr("Precipitation Rate"), GRB_PRECIP_RATE ); };
	if( reader->hasDataType( GRB_PRECIP_TOT ) )		{ myToolBar->cbData->addItem(tr("Precipitation Total"), GRB_PRECIP_TOT ); };
	
	return true;
}

//--------------------------------------------------------------------------------------------
// setup x axis at initialization and replot of new track
//--------------------------------------------------------------------------------------------
void CurveDrawer::setupXAxis( const double &dFirst, const double &dLast )
{
	double dTimeStep = ( abs(dFirst - dLast) > 30) ? 12. : 3.;
	qwtDataPlot->setAxisScale( QwtPlot::xBottom, dFirst, dLast, dTimeStep );		
	qwtDataPlot->setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw() );
	
	// set up top x-axis
	// plot distances on top axis only for waypoint plot (two points given in constructor)
	if( !bSingle ) {
		qwtDataPlot->enableAxis( QwtPlot::xTop, true );	
		qwtDataPlot->setAxisTitle( QwtPlot::xTop, tr("distance [nm]") );
		qwtDataPlot->setAxisScaleDraw(QwtPlot::xTop, new CustomQwtScale( *qvXAxis, qvDist ) );
		qwtDataPlot->setAxisScale( QwtPlot::xTop, dFirst, dLast, dTimeStep );		
		
	} else {
		qwtDataPlot->enableAxis( QwtPlot::xTop, false );
	}
}
//--------------------------------------------------------------------------------------------
// initialize object attributes at creation
//--------------------------------------------------------------------------------------------
void CurveDrawer::initObj( )
{
	for( int i=0; i<4; i++ )
		qwtCurve[i] = NULL;
	for( int i=0; i<2; i++ )
		qwtMarker[i] = NULL;
	qwtDataPlot = NULL;
	// OpenCPN
//	myPolar = NULL;
};

//-------------------------------------------------------------------------------
// Initialize interface by registring signals and call of show
//-------------------------------------------------------------------------------
void CurveDrawer::initInterface()
{
	this->resize(940, 740);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(1);
	sizePolicy.setVerticalStretch(1);
	this->setSizePolicy(sizePolicy);
	this->setBaseSize(QSize(939, 736));
	this->setContentsMargins(0, 0, 0, 0);

	/* Ist funktioniert, bis auf Maximierung des DataPlots
	 Dialog per QDialog
	 - myHlayout
	 -- myVLayout
	 --- ButtonBox
	 --- myScrollArea
	 ---- qwtDataPlot
	 */
	
	myHLayout = new QHBoxLayout( this );
	assert(myHLayout);
	myHLayout->setContentsMargins(0, 0, 0, 0);
    myHLayout->setSpacing(0);
		
    myVLayout = new QVBoxLayout();    
	assert(myVLayout);
	myVLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	myVLayout->setContentsMargins(0, 0, 0, 0);
    myHLayout->addLayout( myVLayout);
	
	myToolBar = new CurveDrawerButtonBar( this, bSingle );	
	assert(myToolBar);	
	myVLayout->addWidget( myToolBar, Qt::AlignCenter );

	qwtDataPlot = new QwtPlot();
	assert(qwtDataPlot);
	QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	sizePolicy2.setHorizontalStretch(1);
	sizePolicy2.setVerticalStretch(1);
	qwtDataPlot->setSizePolicy(sizePolicy2);
	qwtDataPlot->setMinimumSize(QSize(650, 500)); //+200
	qwtDataPlot->setMaximumSize(QSize(16777215, 16777215));
	qwtDataPlot->setCursor(QCursor(Qt::CrossCursor));
	qwtDataPlot->setContentsMargins(5, 5, 5, 5);
	
	myScrollArea = new QScrollArea();
	assert(myScrollArea);
	myScrollArea->setContentsMargins(0, 0, 0, 0);
	myScrollArea->setWidget( qwtDataPlot );
	myScrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	myScrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	myScrollArea->setContentsMargins(0, 0, 0, 0 );
	myScrollArea->setWidgetResizable( true );
//	myScrollArea->setBackgroundRole(QPalette::Dark);
	
	myVLayout->addWidget( myScrollArea );
	
	connect( myToolBar,		SIGNAL(signalExitClicked()),		this, SLOT(slotExitClicked()));
	connect( myToolBar,		SIGNAL(signalCurrentChecked(bool)), this, SLOT(slotCurrentClicked(bool)));
	connect( myToolBar,		SIGNAL(signalFileSelector()),		this, SLOT(slotFileSelector()));	
	connect( myToolBar->cbData,	SIGNAL(activated(int)),			this, SLOT(slotDataChosen(int)) );
	// OpenCPN
//	connect( myToolBar->cbWayPoints, SIGNAL(activated(int)),	this, SLOT(slotSelectTrack(int)) );	
	
// 	connect( qwtDataPlot,	SIGNAL(legendChecked( QwtPlotItem*, bool ) ), this,
//             SLOT( slotLegendChecked( QwtPlotItem*, bool ) ) );
}
/*
 *=========================================================================================
 *   Curve Drawer Tool Bar 
 *=========================================================================================
 */
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
CurveDrawerButtonBar::CurveDrawerButtonBar(QWidget *parent, bool bSingle)
: QToolBar(parent)
{
	this->setFloatable( false );
	this->setMovable( false );
	
    acExit = new QAction(this);
    acExit->setToolTip (tr("Close the window"));
	acExit->setIcon(QIcon(Util::pathImg("exit.png")));
	acExit->setShortcut( tr("Ctrl+Q"));
	acExit->setShortcutContext( Qt::ApplicationShortcut );
	this->addAction(acExit);
	connect(acExit, SIGNAL(triggered()), this, SLOT(slotCommonActions()));
        
    this->addSeparator();
	
	if( bSingle ) {
		acCurrent = new QAction(this);
		acCurrent->setToolTip (tr("Display hours from now or date view"));
		acCurrent->setCheckable ( true );
		acCurrent->setChecked( true );
		acCurrent->setIcon(QIcon(Util::pathImg("time_icon.png")));
		this->addAction(acCurrent);
		connect(acCurrent, SIGNAL(triggered()), this, SLOT(slotCommonActions()));
	} else {
		acBoatFile = new QAction(this);
		acBoatFile->setToolTip( tr("Choose File for Boat Parameters") );
		acBoatFile->setIcon(QIcon(Util::pathImg("fileopen.png")));
		acBoatFile->setShortcut( tr("Ctrl+O"));
		acBoatFile->setShortcutContext( Qt::ApplicationShortcut );
		this->addAction(acBoatFile);
		connect(acBoatFile, SIGNAL(triggered()), this, SLOT(slotCommonActions()));		
	}

	this->addSeparator();
	
	lbData = new QLabel(parent);
	assert(lbData);
	lbData->setText(tr("Data"));
	this->addWidget(lbData);
	
	cbData = new QComboBox(parent);
	assert(cbData);
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(cbData->sizePolicy().hasHeightForWidth());
	cbData->setSizePolicy(sizePolicy);
	cbData->setMinimumSize(QSize(194, 24));
	QFont font;
	font.setPointSize(10);
	cbData->setFont(font);
	this->addWidget(cbData);
	
	lbWayPoint = new QLabel(parent);
	assert(lbWayPoint);
	lbWayPoint->setText(tr("Waypoints"));
    
    QAction *tmpWPLabel = this->addWidget(lbWayPoint);
	
	cbWayPoints = new QComboBox(parent);
	assert(cbWayPoints);
	cbWayPoints->setFont(font);
	QAction *tmpWPBox = this->addWidget(cbWayPoints);

	if( bSingle ) {
		tmpWPBox->setVisible(false);
		tmpWPLabel->setVisible(false);
	}

}
//-------------------------------------------------------------------------------
// Slot for aggregated signal handling
//-------------------------------------------------------------------------------
void CurveDrawerButtonBar::slotCommonActions()
{
	if (sender() == acExit) {
		emit signalExitClicked();
	}
	else if (sender() == acCurrent) {
		emit signalCurrentChecked(acCurrent->isChecked());
	}
	else if (sender() == acBoatFile) {
		emit signalFileSelector();
	}	
}
	
