/*
 *  CurveDrawer.h
 *  zyGrib
 *
 *  Created by Tim Holtschneider on 29.05.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef DATAPLOT_H
#define DATAPLOT_H
#include <cmath>
#include <ctime>
#include <QObject>

#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QScrollArea>

#include "GriddedPlotter.h"
#include "DataDefines.h"
#include "POI.h"
#include "DataPointInfo.h"
#include "CustomQwtClasses.h"
//#include "Polar.h"

#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_marker.h>
#include <qwt_legend.h>
#include "qwt_plot.h"
#include <qwt_legend_label.h>


//=====================================================================================
//
//
class CurveDrawerButtonBar : public QToolBar
{ Q_OBJECT
public:
	CurveDrawerButtonBar(QWidget *parent, bool);
	
	inline void setChkCurrent(bool btCheck) { acCurrent->setChecked(btCheck); };
	
signals:
	void signalExitClicked();
	void signalCurrentChecked(bool);
	void signalFileSelector();
	
private:
	
	QAction		*acExit;
	QAction		*acCurrent;
	QAction		*acBoatFile;
	QLabel		*lbData;
	QLabel		*lbWayPoint;
	
public:
	QComboBox	*cbData;
	QComboBox	*cbWayPoints;
	
private slots:
	void slotCommonActions();
	
};


//===================================================================
// DataPlotter : dialog
//===================================================================
class CurveDrawer :	public	QWidget
{ Q_OBJECT

public:
	CurveDrawer( GriddedPlotter *, double, double);
	// OpenCPN
	//		CurveDrawer( GriddedPlotter *, QWidget *);
	~CurveDrawer();
	
private:
	GriddedPlotter	*plotter;
	GriddedReader	*reader;
	double			lonFirst, latStart, lonStart, lonEnd, latEnd;
	time_t			tRefDate, tCurDate;
	bool			bSingle;
	
	// Lists for grib data
	QVector<QString> qvDates;
	QVector<double> qvHoursFromNow;
	QVector<double> qvDist;
	QVector<double> *qvXAxis;
	QVector<double> *qvY2Axis;
	QVector<double>	qvY1Axis;
	QVector<double> qvWindDir;
	QVector<double> qvWindDev;
	QVector<double> qvTempMin;
	QVector<double> qvTempMax;
	int iDataCnt;
	
	// Qwt objects
	QwtScaleEngine  *qwtYScale[2];
	QwtPlotCurve	*qwtCurve[4];
	CustomQwtPicker *qwtPicker;
	QwtLegend		*qwtLegend;
	QwtPlotMarker	*qwtMarker[2];
	
	// OpenCPN, polar boat parameters
	//		Polar			*myPolar;
	
	// methods to create and update plots
	bool	createTable( const int & );
	// OpenCPN
	//		bool	createTablePOI( const int & );
	
	void	initDataPlot( const int & );
	void	createDataPlot( const int &, const QString & );
	
	void	activateLegend( QwtPlotItem *, bool );
	void	setupXAxis( const double&, const double& );
	
	void	initObj();
	void	initInterface();
	bool	privateConstructor();
	
	void	closeEvent(QCloseEvent *) {delete this;};
    
	QString	getPlotUnit( const int & );
	float   convertData( const int&, const float& );
	//-------------------------------------------------------------------------------
	// Calculate inbound angle between two directions
	//		bool parameter controls return of absulte difference
	//-------------------------------------------------------------------------------
	float	getInboundAngle( const float &fAngle1, const float &fAngle2, bool bAbs );
	//-------------------------------------------------------------------------------
	// Append data to the curves based on chosen curvesfor display
	//-------------------------------------------------------------------------------
	void	appendCurves( const int &iDataChosen, const DataPointInfo *pinfo );
	
	// GUI obbjects
	CurveDrawerButtonBar *myToolBar;
	QHBoxLayout			 *myHLayout;
	QVBoxLayout			 *myVLayout;	
	QwtPlot				 *qwtDataPlot;
	QScrollArea			 *myScrollArea;
	
signals:
	void legendClicked( QwtPlotItem * );
	
	private slots:
	void slotExitClicked();
	void slotCurrentClicked(bool);
	void slotFileSelector();
	void slotDataChosen(int);
	void slotSelectTrack(int);
	void slotLegendChecked(const QVariant &itemInfo, bool on, int index);
	
};

#endif
