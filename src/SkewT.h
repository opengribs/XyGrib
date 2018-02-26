#ifndef SKEWT_H
#define SKEWT_H

#include <cstdio>
#include <cmath>
#include <map>

#include "Therm.h"
#include "Util.h"
#include "GriddedReader.h"
#include "GriddedPlotter.h"
#include "DataQString.h"
#include "SylkFile.h"

#include <QWidget>
#include <QMainWindow>
#include <QFrame>
#include <QPainter>
#include <QLayout>
#include <QKeyEvent>
#include <QPrintDialog>
#include <QPrinter>
#include <QDialog>

#include <QToolBar>
#include <QAction>
#include <QScrollArea>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QCheckBox>
#include <QThread>
#include <QMessageBox>

class SkewT;

//-----------------------------------------------------------
class PersPath : public QPainterPath
{
	public:
		PersPath (SkewT *skewt);
		
		void drawLabelPercent (QPainter &pnt, double percent, const QString &txt);
		void drawLabelPixel (QPainter &pnt, double pixel, const QString &txt);
		void addPoint (QPointF &pt);
		void addPointNoClip (QPointF &pt);
		void fromTPCurve (TPCurve *curve);
		
	private:
		SkewT *skewt;
		bool isFirstPoint;
		QRectF rectglob;
};

//-----------------------------------------------------------
class SkewT : public QWidget
{ Q_OBJECT
	friend class PersPath;
	friend class SkewTWindow;
	public :
		SkewT (int W=-1, int H=-1, QWidget *parent=NULL);
		~SkewT ();
		
		void addSoundingPoint (double hpa, double tempK, double dewpK);
		void addSoundingPointWind (double hpa, double vx, double vy);
		void initFromGriddedReader (GriddedReader *reader, double lon, double lat, time_t date);
		bool hasSoundingData () 
					{return sounding.getAllSounds()->size() >= 2;}
		
		double hpa2pix (double hpa);	// Conversion altitude/pixel
		double m2pix (double z);
		
		QPointF tempPressPoint (double tempC, double hpa);
		QPointF tempPressPoint (const TPoint &tp);
	
		Sounding * getSounding ()   {return &sounding;}
		
		void   setTempPressLimits (double tempMax, double hpaMin);
		void   setSkewTSize (int W, int H);
		void   resetGraphic ();
		
		void   setConvectiveBase (double hpa0max, double hpa0min);
		void   setConvectiveBase (QString cbase);    // format "1000-950"
		
		double getHpaMin ()  {return hpaMin;}
		double getHpaMax ()  {return hpaMax;}
		double getTempCMax () {return tempCMax;}
		bool   hasSurfaceData;
		
		void  setPrinterRendering (bool b)  {printerRendering = b;}
		void  setLocation (QString loc)     {location=loc;}
		void  setReference (QString ref)    {reference=ref;}
		void  setCurrentDate (QString dat)  {curdate=dat;}
		
		QString getLocation () {return location;}
		
	protected :
		GriddedReader *reader;
		int W, H;
		int DX1, DY1;
		int DX2, DY2;
		QRect clipRect;
		QRect getDiagramRectangle () const;
		
		Sounding sounding;
		double hpa0max, hpa0min;   // convective levels base
		
		double tempCMax;     // temperature in the bottom right corner
		double deltaTemp;    // temperature gradient in degree/pixel
		double hpaMin, hpaMax;
		double altmMin, altmMax;
		double surfaceHpa;
		QString location, reference, curdate;
		time_t  dateref, date;
		QString datacenter;
		double ModelCAPE, ModelCIN;
		
		//--------------------------------------
		void draw_dryAdiabats (QPainter &pnt);
		void draw_temperatureScale (QPainter &pnt, bool withLabels=true);
		void draw_altitudeScale (QPainter &pnt);
		void draw_pressureScale (QPainter &pnt, bool withLabels=true);
		void draw_comments (QPainter &pnt);
		void draw_surface_level (QPainter &pnt);
		
		void draw_mixingRatio (QPainter &pnt);
		void draw_oneMixingRatioCurve (double mixr, QPainter &pnt);
		
		void draw_saturatedAdiabats (QPainter &pnt);
		PersPath * newPath_saturatedAdiabat (double tempC);
		//QMap <int, PersPath *> mapSaturatedAdiabats;
		std::map <int, PersPath *> mapSaturatedAdiabats;
		
		void draw_sounding (QPainter &pnt);
		void draw_windArrows (QPainter &pnt);
		void draw_linesCAPE (QPainter &pnt);
		void fill_plain_area (QPainter &pnt, const QList<TPoint> &pts, const QColor &color);
		
		QColor pressureScaleColor;
		QColor temperatureScaleColor;
		QColor dryAdiabatColor;
		QColor mixingRatioColor;
		QColor saturatedAdiabatColor;
		QColor soundingTempColor;
		QColor soundingDewpColor;
		QColor linesCAPEColor;
		QColor fillCAPEColor;
		QColor fillCINColor;
		QColor surfaceLevelColor;
		
		bool   showConvArea;
		
		double mainFontSize;
		QFont  mainFont;
		
		void paintEvent (QPaintEvent *);
		QPixmap *globalPixmap;
		bool printerRendering;
		void paintGlobalImage (QPainter &pnt);
};
//---------------------------------------------
class MiniSkewT : public SkewT
{ Q_OBJECT
	public:
		MiniSkewT (int W=60, int H=60, QWidget *parent=NULL);
		QPixmap createPixmap ();
	private :
		void paintEvent (QPaintEvent *);
		void paintPixmap (QPainter &pnt);
};

//---------------------------------------------
class SkewTWindow : public QMainWindow
{ Q_OBJECT
	public:
		SkewTWindow (SkewT *skewt);
		~SkewTWindow ();

	public slots:
		void actionsCommonSlot ();
		
	private :
		SkewT *skewt;
		
		QAction *acExit, *acPrint, *acSaveImage, *acExportData;
		QComboBox *cbTempMax, *cbHpaMin, *cbSizeH, *cbSizeW, *cbConvBase;
		QCheckBox *chkShowConv;
		
		void createToolBar ();
		void resizeEvent (QResizeEvent *);
		void closeEvent (QCloseEvent *);
		void saveFileSYLK (SylkFile &slk);
};


#endif
