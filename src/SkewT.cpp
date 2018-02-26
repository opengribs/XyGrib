
#include "SkewT.h"

//------------------------------------------------------
SkewT::SkewT (int W, int H, QWidget *parent) : QWidget (parent)
{
    setFocusPolicy(Qt::StrongFocus);
	if (W < 0)
		W = Util::getSetting ("skewt_sizeW", 800).toDouble();
	if (H < 0)
		H = Util::getSetting ("skewt_sizeH", 800).toDouble();
	
	this->W = W;
	this->H = H;
	setFixedSize (W, H);
	globalPixmap = NULL;
	reader = NULL;
	printerRendering = false;
	hasSurfaceData = false;
	
	hpa0max = 1000;
	hpa0min = 900;
	ModelCAPE = GRIB_NOTDEF;
	ModelCIN = GRIB_NOTDEF;
	
	setTempPressLimits (40, 90);
		 
	QColor bistre (133, 109, 77);
	QColor vert   (80, 170, 60);
	pressureScaleColor = bistre;
	temperatureScaleColor = bistre;
	mixingRatioColor = bistre;
	dryAdiabatColor = vert;
	saturatedAdiabatColor = vert;
	soundingTempColor = QColor (255,80,80);	  // red
	soundingDewpColor = QColor (100,100,255);  // blue
	linesCAPEColor = QColor (210,160,0);     // orange
	
	showConvArea = true;
	fillCAPEColor = QColor (240,240,150);
	fillCINColor = QColor (240,150,150);
	surfaceLevelColor = QColor (60,60,60);
}
//------------------------------------------------------
SkewT::~SkewT ()
{
	Util::cleanMapPointers (mapSaturatedAdiabats);
}
//------------------------------------------------------
void SkewT::paintGlobalImage (QPainter &pnt)
{
	sounding.compute_convective_levels (hpa0max, hpa0min);
	
	pnt.setClipRect (clipRect);
	pnt.setClipping (false);	
	pnt.setFont (mainFont);

	pnt.setRenderHint(QPainter::Antialiasing, true);
	pnt.fillRect (0,0, W,H, Qt::white);

	showConvArea = Util::getSetting ("skewt_showConvectiveCurves",true).toBool();
	
	draw_comments (pnt);	
	if (showConvArea) {
		fill_plain_area (pnt, sounding.curveCAPE, fillCAPEColor);
		fill_plain_area (pnt, sounding.curveCIN, fillCINColor);
	}
	pnt.drawRect (getDiagramRectangle ());
	if (hasSurfaceData) {
		draw_surface_level (pnt);
	}
	draw_altitudeScale (pnt);
	draw_pressureScale (pnt);
	draw_temperatureScale (pnt);
	draw_dryAdiabats (pnt);
	draw_mixingRatio (pnt);
	draw_saturatedAdiabats (pnt);
	
	if (hasSurfaceData) {
		draw_surface_level (pnt);
	}
	if (showConvArea) {
		draw_linesCAPE (pnt);
	}
	draw_sounding  (pnt);
	draw_windArrows (pnt);
}
//------------------------------------------------------
void SkewT::setSkewTSize (int W, int H)
{
	this->W = W;
	this->H = H;
	setFixedSize (W, H);
	setTempPressLimits (tempCMax, hpaMin);
}
//------------------------------------------------------
void SkewT::setConvectiveBase (double hpa0max, double hpa0min)
{
	this->hpa0max = hpa0max;
	this->hpa0min = hpa0min;
	setTempPressLimits (tempCMax, hpaMin);
}
//------------------------------------------------------
void SkewT::setConvectiveBase (QString cbase) 
{ 
	double hmax, hmin;
	QString p1, p2;
	bool ok;
	if (cbase == "surface") {
		if (hasSurfaceData) {
			this->hpa0max = surfaceHpa;
			this->hpa0min = surfaceHpa;
			setTempPressLimits (tempCMax, hpaMin);
		}
	}
	else if (cbase.startsWith ("surface-")) 
	{ // format "surface-50"
		QRegExp reg ("surface-(\\d+)");
		if (reg.indexIn (cbase) != -1) {
			p2 = reg.cap(1);
			hmax = surfaceHpa;
			hmin = surfaceHpa - p2.toDouble (&ok);
			if (ok) {
				this->hpa0max = hmax;
				this->hpa0min = hmin;
				setTempPressLimits (tempCMax, hpaMin);
			}
		}
	}
	else { // format "1000-950"
		QRegExp reg ("(\\d+)-(\\d+)");
		if (reg.indexIn (cbase) != -1) {
			p1 = reg.cap(1);
			p2 = reg.cap(2);
			hmax = p1.toDouble (&ok);
			if (ok) {
				hmin = p2.toDouble (&ok);
				if (ok) {
					this->hpa0max = hmax;
					this->hpa0min = hmin;
					setTempPressLimits (tempCMax, hpaMin);
				}
			}
		}
	}
}
//------------------------------------------------------
void SkewT::initFromGriddedReader (GriddedReader *reader, double lon, double lat, time_t date)
{
	if (!reader || !reader->isOk())
			return;
	this->reader = reader;
	time_t dateref = 0;
	QString datacenter = "";
	QVector <int> allAlts;
	double rh, temp, dewp;
	allAlts <<1000<<975<<950<<925<<900<<875<<850<<825<<800
				<<775<<750<<725<<700<<650<<600<<550<<500<<400<<300<<200<<100;

	foreach (int alt, allAlts) {
		GriddedRecord *rect = reader->getRecord (DataCode(GRB_TEMP,LV_ISOBARIC,alt), date);
		GriddedRecord *rech = reader->getRecord (DataCode(GRB_HUMID_REL,LV_ISOBARIC,alt), date);
		if (rect && rech) {
			temp = rect->getInterpolatedValue (DataCode(GRB_TEMP,LV_ISOBARIC,alt), lon, lat);
			rh = rech->getInterpolatedValue (DataCode(GRB_HUMID_REL,LV_ISOBARIC,alt), lon, lat);
			dewp = DataRecordAbstract::dewpointHardy (temp, rh);
			this->addSoundingPoint (alt, temp, dewp);
			if (dateref == 0)
				dateref = reader->getRefDateForData (DataCode(GRB_TEMP,LV_ISOBARIC,alt));
			if (datacenter == "")
				datacenter = DataCodeStr::toString (rect->getDataCenterModel ());
		}
	}
	//-----------------------------
	this->setLocation (Util::formatPosition (lon, lat));
	this->setReference (datacenter+" "+Util::formatDateTimeLong (dateref));
	this->setCurrentDate (Util::formatDateTimeLong (date));
	this->datacenter = datacenter;
	this->dateref = dateref;
	this->date = date;
	//-----------------------------
	// Wind
	foreach (int alt, allAlts) {
		GriddedRecord *recvx = reader->getRecord (DataCode(GRB_WIND_VX,LV_ISOBARIC,alt), date);
		GriddedRecord *recvy = reader->getRecord (DataCode(GRB_WIND_VY,LV_ISOBARIC,alt), date);
		if (recvx && recvy) {
			double vx = recvx->getInterpolatedValue (DataCode(GRB_WIND_VX,LV_ISOBARIC,alt), lon, lat);
			double vy = recvy->getInterpolatedValue (DataCode(GRB_WIND_VY,LV_ISOBARIC,alt), lon, lat);
			// DBG("%d : %g %g", alt, vx,vy);
			this->addSoundingPointWind (alt, vx, vy);
		}
	}
	//---------------------------------------------------
	// CAPE / CIN from NOAA GFS data
	GriddedRecord *rec;
	rec = reader->getRecord (DataCode(GRB_CAPE,LV_GND_SURF,0), date);
	if (rec) {
		ModelCAPE = rec->getInterpolatedValue (DataCode(GRB_CAPE,LV_GND_SURF,0), lon, lat);
	}
	else
		ModelCAPE = GRIB_NOTDEF;
	rec = reader->getRecord (DataCode(GRB_CIN,LV_GND_SURF,0), date);
	if (rec) {
		ModelCIN = rec->getInterpolatedValue (DataCode(GRB_CIN,LV_GND_SURF,0), lon, lat);
	}
	else
		ModelCIN = GRIB_NOTDEF;
	//---------------------------------------------------
	// Surface pressure if present
	hasSurfaceData = false;
	rec = reader->getRecord (DataCode(GRB_PRESSURE,LV_GND_SURF,0), date);
	if (rec) {
		surfaceHpa = rec->getInterpolatedValue (DataCode(GRB_PRESSURE,LV_GND_SURF,0), lon, lat);
		if (surfaceHpa != GRIB_NOTDEF)
		{
			surfaceHpa /= 100.0;	// hpa
			
			rec = reader->getRecord (DataCode(GRB_TEMP,LV_ABOV_GND,2), date);
			temp = rec ? rec->getInterpolatedValue (DataCode(GRB_TEMP,LV_ABOV_GND,2), lon, lat)
					: GRIB_NOTDEF;
			rec = reader->getRecord (DataCode(GRB_DEWPOINT,LV_ABOV_GND,2), date);
			dewp = rec ? rec->getInterpolatedValue (DataCode(GRB_DEWPOINT,LV_ABOV_GND,2), lon, lat)
					: GRIB_NOTDEF;
			if (temp != GRIB_NOTDEF && dewp != GRIB_NOTDEF) {
				this->addSoundingPoint (surfaceHpa, temp, dewp);
				hasSurfaceData = true;
			}
		}
	}
	else {
		surfaceHpa = GRIB_NOTDEF;
	}
}
//------------------------------------------------------
PersPath * SkewT::newPath_saturatedAdiabat (double tempC)
{
	double P, hpa0 = 1000;
	QPointF pt;
	
	// search start point: down from hpa0 to hpaMax
	double T = tempC;
	double deltap = 0.25;
	double dtdp;
	for (P=hpa0+deltap; P <= hpaMax; P += deltap) 
	{
		dtdp = Therm::saturated_dT_dP (T, P); 
		T += dtdp*deltap;
	}
	// Compute all points, from P to hpaMin
	pt = tempPressPoint (T, P);
	PersPath *path = new PersPath (this);
	int nb;
	for (nb=0, P=P-deltap ; P >= hpaMin-deltap; P -= deltap, nb++) 
	{
		dtdp = Therm::saturated_dT_dP (T, P); 
		T -= dtdp*deltap;
		if ( (nb%40)==0 || P <= hpaMin) {
			pt = tempPressPoint (T, P);
			path->addPoint (pt);
		}
	}
	return path;
}
//------------------------------------------------------
void  SkewT::draw_saturatedAdiabats (QPainter &pnt)
{
	QPen pen (saturatedAdiabatColor);
	pen.setStyle (Qt::DashLine);
	pnt.setPen (pen);
	PersPath *path;
	for (int t0 = 50; t0 >= -50; t0 -= 5) 
	{
		if (t0 == 0) {
			pen.setWidthF (1.0);
			pnt.setPen (pen);
		}
		else if (t0%10 == 0) {
			pen.setWidthF (0.8);
			pnt.setPen (pen);
		}
		else {
			pen.setWidthF (0.4);
			pnt.setPen (pen);
		}
		std::map <int, PersPath *>::iterator it = mapSaturatedAdiabats.find (t0);
		if (it != mapSaturatedAdiabats.end()) {
			path = (*it).second;
		}
		else {
			path = newPath_saturatedAdiabat (t0);
			mapSaturatedAdiabats [t0] = path;
		}
		pnt.drawPath (*path);
		if (path->length() > 100)
			path->drawLabelPixel (pnt, -80, QString("%1").arg(t0));
	}
	pen.setStyle (Qt::SolidLine);
	pnt.setPen (pen);
}
//-------------------------------------------------------
void PersPath::drawLabelPixel (QPainter &pnt, double pixel, const QString &txt)
{
	double pos;
	if (pixel >= 0)
		pos = pixel;
	else
		pos = this->length()+pixel;
	this->drawLabelPercent (pnt, this->percentAtLength(pos), txt);
}
//-------------------------------------------------------
void PersPath::drawLabelPercent (QPainter &pnt, double percent, const QString &txt)
{
	QPointF pt = this->pointAtPercent (percent);
	double angle = this->angleAtPercent (percent);
	QColor bgcolor (255,255,255,100);
	pnt.save ();
		pnt.translate (pt);
		if (angle<90)
			pnt.rotate (-angle);
		else
			pnt.rotate (180-angle);
		QFontMetrics fm (pnt.font());
		QRectF rect = fm.boundingRect (txt);
		rect.adjust (-1,-5, 1,5);
		rect.moveCenter (QPointF(0,0));
		pnt.fillRect (rect, bgcolor);
		pnt.drawText (rect, Qt::AlignCenter, txt);	
	pnt.restore ();
}
//------------------------------------------------------
void  SkewT::draw_dryAdiabats (QPainter &pnt)
{
	QPen pen (dryAdiabatColor);
	pnt.setPen (pen);
	double temp;
	double p0 = 1000;
	PersPath *path;
 	for (int t0 = 200; t0 > -200 ; t0 -= 5) 
	{
		path = new PersPath (this);
		if (t0 == 0) {
			pen.setWidthF (1.0);
			pnt.setPen (pen);
		}
		else if (t0%10 == 0) {
			pen.setWidthF (0.8);
			pnt.setPen (pen);
		}
		else {
			pen.setWidthF (0.4);
			pnt.setPen (pen);
		}
		for (double p = p0+50; p >= hpaMin-50; p-=10)	// start from 1050 hpa, and pass by 1000 hpa
		{
			temp = Therm::dryAdiabaticTemperature (1000, t0, p);
			QPointF pt = tempPressPoint (temp, p);
			path->addPoint (pt);
		}
		pnt.drawPath (*path);
		if (path->length() > 50)
			path->drawLabelPercent (pnt, path->percentAtLength(path->length()-40), 
							 QString("%1").arg(t0));
		delete path;
	}
}
//------------------------------------------------------
void  SkewT::draw_mixingRatio (QPainter &pnt)
{
	QPen pen (mixingRatioColor);
	pen.setStyle (Qt::DashLine);
	pnt.setPen (pen);
	pnt.setClipping (true);
	  draw_oneMixingRatioCurve (0.1, pnt);
	  draw_oneMixingRatioCurve (0.2, pnt);
	  draw_oneMixingRatioCurve (0.4, pnt);
	  draw_oneMixingRatioCurve (0.6, pnt);
	  draw_oneMixingRatioCurve (1, pnt);
	  draw_oneMixingRatioCurve (2, pnt);
	  draw_oneMixingRatioCurve (4, pnt);
	  draw_oneMixingRatioCurve (6, pnt);
	  draw_oneMixingRatioCurve (10, pnt);
	  draw_oneMixingRatioCurve (15, pnt);
	  draw_oneMixingRatioCurve (20, pnt);
	  draw_oneMixingRatioCurve (30, pnt);
	  draw_oneMixingRatioCurve (40, pnt);
	  draw_oneMixingRatioCurve (50, pnt);
	pnt.setClipping (false);
	pen.setStyle (Qt::SolidLine);
	pnt.setPen (pen);
}
//------------------------------------------------------
void  SkewT::draw_oneMixingRatioCurve (double mixr, QPainter &pnt)
{
	double hpa, tempC;
	mixr = mixr / 1000.0;		// g/kg => kg/kg
	PersPath path (this);
	hpa = hpaMax;
	QPointF pt;
	for (hpa=hpaMax; hpa > hpaMin; hpa -= 50) 
	{
		tempC = Therm::tempFromMixingRatio (mixr, hpa);
		QPointF pt = tempPressPoint (tempC, hpa);
		path.addPoint (pt);
	} 
	pnt.drawPath (path);
	if (path.length() > 100)
		path.drawLabelPercent (pnt, path.percentAtLength(path.length()-100), 
						QString("%1").arg(1000*mixr));
}
//------------------------------------------------------
void  SkewT::draw_temperatureScale (QPainter &pnt, bool withLabels)
{
	QFontMetrics fmet (mainFont);
	double fh = fmet.lineSpacing ();
	QPen pen (temperatureScaleColor);
	pnt.setPen (pen);
	int temp;
	int i0,j0, i1,j1;
	QLineF lineLeft (DX1, DY1, DX1, H-DY2);
	for (temp = tempCMax; temp > -150 ; temp -= 5) 
	{
		if (temp == 0) {
			pen.setWidthF (1.2);
			pnt.setPen (pen);
		}
		else if (temp%10 == 0) {
			pen.setWidthF (0.8);
			pnt.setPen (pen);
		}
		else {
			pen.setWidthF (0.4);
			pnt.setPen (pen);
		}
		i0 = W-DX2 - (tempCMax-temp)/deltaTemp;
		j0 = H-DY2;
		i1 = W-DX2;
		j1 = H-DY2 - (tempCMax-temp)/deltaTemp;
		pnt.setClipping (true);
		pnt.drawLine (i0, j0, i1, j1);
		pnt.setClipping (false);
		// Axis labels
		if (i0 > DX1) {	// line starts on X axis
			pnt.drawLine (i0, j0, i0, j0+4);
			if (withLabels) {
				QString txt = QString("%1").arg(temp);
				pnt.drawText (i0-fmet.width(txt)/2, j0+fh, txt);
			}
		}
		else {  // line starts outside of X axis
			QLineF lineTemp (i0, j0, i1, j1);
			QPointF intertop, interleft;
			if (lineTemp.intersect (lineLeft, &interleft) == QLineF::BoundedIntersection
			) {
				QPoint pl = interleft.toPoint ();
				int i = pl.x();
				int j = pl.y();
				pnt.drawLine (i, j, i-4, j);
				if (withLabels) {
					QString txt = QString("%1").arg(temp);
					pnt.drawText (DX1-fmet.width(txt)-5, j+fh/3, txt);
				}
			}
		}
	}
	if (withLabels) {
		QString txt = "°C";
		pnt.drawText (DX1-fmet.width(txt)-5, H-DY2+fh, txt);
	}
}
//------------------------------------------------------
void  SkewT::draw_pressureScale (QPainter &pnt, bool withLabels)
{
	QFontMetrics fmet (mainFont);
	QPen pen (pressureScaleColor);
	pnt.setPen (pen);
	double hpa;
	int j;
	int i0 = DX1;
	int i1 = W -DX2;
	QRect rect;
	pen.setWidthF (1);
	pnt.setPen (pen);
	for (hpa = 1000; hpa>=hpaMin; hpa -= 100) {
		if (hpa >= 1000)
			rect = pnt.boundingRect (0,0,0,0, Qt::AlignRight, "9999");
		else
			rect = pnt.boundingRect (0,0,0,0, Qt::AlignRight, "999");
		j = (int) (hpa2pix (hpa) + 0.5);
		pnt.drawLine (i0, j, i1, j);
		if (withLabels) {
			// label right
			QColor bgcolor (255,255,255,100);
			rect.moveBottomRight (QPoint (i1-10, j+rect.height()/2));
			pnt.fillRect (rect.x()-2,rect.y(),rect.width()+4,rect.height(), bgcolor);
			pnt.drawText (rect, Qt::AlignRight, QString("%1").arg(hpa));
			// label left
			rect.moveBottomLeft (QPoint (i0+8, j+rect.height()/2));
			pnt.fillRect (rect.x()-2,rect.y(),rect.width()+4,rect.height(), bgcolor);
			pnt.drawText (rect, Qt::AlignRight, QString("%1").arg(hpa));
		}
	}
	if (withLabels)
		pnt.drawText (i1-fmet.width("hPa")-6, DY1-6, "hPa");
	// intermediate lines
	pen.setWidthF (0.4);
	pnt.setPen (pen);
	for (hpa = 950; hpa>=hpaMin; hpa -= 100) {
		j = (int) (hpa2pix (hpa) + 0.5);
		pnt.drawLine (i0, j, i1, j);
	}
}
//------------------------------------------------------
void  SkewT::draw_surface_level (QPainter &pnt)
{
	QFontMetrics fmet (mainFont);
	QPen pen (surfaceLevelColor);
	pen.setStyle (Qt::DashLine);
	pen.setWidthF (0.8);
	pnt.setPen (pen);
	int j;
	int i0 = DX1;
	int i1 = W -DX2;
	j = (int) (hpa2pix (surfaceHpa) + 0.5);
	pnt.drawLine (i0, j, i1, j);
}
//------------------------------------------------------
void  SkewT::draw_altitudeScale (QPainter &pnt)
{
	QPen pen (pressureScaleColor);
	pnt.setPen (pen);
	double kmmax = Therm::hpa2m(hpaMin)/1000.0;
	double km; 
	int i = W-DX2;
	int j;
	for (km = 0; km<=kmmax; km++) {
		j = (int) (m2pix (1000.0*km) + 0.5);
		pnt.drawLine (i, j, i+5, j);
		pnt.drawLine (i, j+1, i+5, j+1);
		pnt.drawText (i+8, j+4, QString("%1").arg(km));
	}
	for (km = 0.5; km<kmmax; km++) {
		j = (int) (m2pix (1000.0*km) + 0.5);
		pnt.drawLine (i, j, i+6, j);
	}
	for (km = 0.1; km<kmmax; km+= 0.1) {
		j = (int) (m2pix (1000.0*km) + 0.5);
		pnt.drawLine (i, j, i+3, j);
	}
	pnt.drawText (i+4, DY1-6, "km");
}
//------------------------------------------------------
// Conversions
//------------------------------------------------------
double SkewT::hpa2pix (double hpa)
{
	return m2pix (Therm::hpa2m (hpa));
}
//------------------------------------------------------
double SkewT::m2pix (double z)
{
	double p0 = H - DY2;    // level 0 m
	double pm = DY1;        // level max
	double r = (altmMax-z)/(altmMax-altmMin) * (p0 - pm) + pm;
	return r;
}
//--------------------------------------------------------
QPointF SkewT::tempPressPoint (double tempC, double hpa)
{
	double jp = hpa2pix (hpa);
	double i0 = W-DX2 - (tempCMax-tempC)/deltaTemp;   // temp coordinate on x-axis
	// 45 degrees projection
	double hp = H - jp - DY2;
	double i1 = i0 + hp;
	return QPointF (i1, jp);
}
//--------------------------------------------------------
QPointF SkewT::tempPressPoint (const TPoint &tp)
{
	return tempPressPoint (tp.tempC, tp.hpa);
}
//------------------------------------------------------
PersPath::PersPath (SkewT *skewt)
{
	this->skewt = skewt;
	rectglob = skewt->getDiagramRectangle ();
	isFirstPoint = true;
}
//------------------------------------------------------
void  PersPath::addPoint (QPointF &pt)
{
	if (rectglob.contains (pt)) {
		if (isFirstPoint) {
			this->moveTo (pt);
			isFirstPoint = false;
		} else {
			this->lineTo (pt);
		}
	}
	else
		isFirstPoint = true;
}
//------------------------------------------------------
void  PersPath::addPointNoClip (QPointF &pt)
{
	if (isFirstPoint) {
		this->moveTo (pt);
		isFirstPoint = false;
	} else {
		this->lineTo (pt);
	}
}
//------------------------------------------------------
void  PersPath::fromTPCurve (TPCurve *curve)
{
	foreach (TPoint pt, curve->points)
	{
		QPointF pf = skewt->tempPressPoint (pt.tempC, pt.hpa);
		addPoint (pf);
	}
}
//------------------------------------------------------
QRect SkewT::getDiagramRectangle () const
{
	return QRect(DX1,DY1, W-DX1-DX2, H-DY1-DY2);
}
//------------------------------------------------------
void SkewT::addSoundingPoint (double hpa, double tempK, double dewpK)
{
	sounding.addSoundingPointK (hpa, tempK, dewpK);
}
//------------------------------------------------------
void SkewT::addSoundingPointWind (double hpa, double vx, double vy)
{
	sounding.addSoundingPointWind (hpa, vx, vy);
}

//------------------------------------------------------
// MiniSkewT
//------------------------------------------------------
MiniSkewT::MiniSkewT (int W,int H, QWidget *parent)
				: SkewT (W, H, parent)
{
	DX1 = 0.04*W;
	DX2 = 0.04*W;
	DY1 = 0.04*H;
	DY2 = 0.04*H;
	clipRect = getDiagramRectangle ();
	
	double m1 = Therm::hpa2m (hpaMax);	// altitude min/max
	double m2 = Therm::hpa2m (hpaMin);
	double k = 4.8;
	deltaTemp = k*(m2-m1)/1000.0;	// xxx °C/hpa
	deltaTemp = deltaTemp/(H-DY1-DY2);  // degrees/pixel
}
//----------------------------------------------
void  MiniSkewT::paintEvent (QPaintEvent *)
{
    QPainter pnt(this);
	paintPixmap (pnt);
}
//------------------------------------------------------
void  MiniSkewT::paintPixmap (QPainter &pnt)
{
	pnt.setClipRect (clipRect);
	pnt.setClipping (false);
	pnt.fillRect (0,0, W,H, Qt::white);

	pnt.setRenderHint(QPainter::Antialiasing, true);
	pnt.drawRect (getDiagramRectangle ());
	
	draw_pressureScale (pnt, false);
	draw_temperatureScale (pnt, false);
	draw_sounding (pnt);
}
//------------------------------------------------------
QPixmap  MiniSkewT::createPixmap ()
{
	QPixmap pixmap (W,H);
	QPainter pnt (&pixmap);
	paintPixmap (pnt);
	return pixmap;
}
//===========================================================
// Paint SkewT
//===========================================================
void  SkewT::paintEvent (QPaintEvent *)
{
	if (! globalPixmap) {
		globalPixmap = new QPixmap (W, H);
		QPainter pnt (globalPixmap);
		paintGlobalImage (pnt);
	}
	
    QPainter pnt (this);
	if (printerRendering)
		paintGlobalImage (pnt);
	else
		pnt.drawPixmap (0,0, *globalPixmap);
}
//------------------------------------------------------
void SkewT::resetGraphic ()
{
	setTempPressLimits (tempCMax, hpaMin);
}
//------------------------------------------------------
void SkewT::setTempPressLimits (double tempCMax, double hpaMin)
{
	sounding.invalidateConvectiveLevels ();
	this->hpaMax = 1013.25;
	this->hpaMax = 1050;
	this->hpaMin = hpaMin;
	this->tempCMax = tempCMax;
	
	mainFontSize = qMax (qMin(W/100.0, H/100.0), 6.9);
	mainFont = QFont ("Helvetica", mainFontSize);
	QFontMetrics fmet (mainFont);
	
	double fh = fmet.lineSpacing ();
	DX1 = 0.04*W;
	DX2 = 0.065*W;
	DY1 = ceil (2.5*fh);
	DY2 = ceil (3.3*fh);
	
	clipRect = getDiagramRectangle ();
	altmMax = Therm::hpa2m (hpaMin);
	altmMin = Therm::hpa2m (hpaMax);
	
	double m1 = Therm::hpa2m (hpaMax);	// altitude min/max
	double m2 = Therm::hpa2m (hpaMin);
	double k = 4.8;
	deltaTemp = k*(m2-m1)/1000.0;	// xxx °C/hpa
	deltaTemp = deltaTemp/(H-DY1-DY2);  // degrees/pixel
	
	Util::cleanMapPointers (mapSaturatedAdiabats);
	if (globalPixmap) {
		delete globalPixmap;
		globalPixmap = NULL;
	}
	update ();
}
//------------------------------------------------------
void SkewT::fill_plain_area (QPainter &pnt, const QList<TPoint> &pts, const QColor &color)
{
	QPainterPath path;
	TPoint pt;
	bool start = true;
	foreach (pt, pts) {
		if (start) {
			path.moveTo (tempPressPoint(pt));
			start = false;
		}
		else {
			path.lineTo (tempPressPoint(pt));
		}
	}
	path.closeSubpath ();
	pnt.setClipping (true);
	pnt.fillPath (path, color);
	pnt.setClipping (false);
}

//------------------------------------------------------
void  SkewT::draw_windArrows (QPainter &pnt)
{
	foreach (SoundingPointWind sw, *sounding.getAllSoundsWind()) 
	{
		QPointF pt = tempPressPoint (0, sw.hpa);
		GriddedPlotter::drawWindArrowWithBarbs_static (
					pnt,
					W-0.4*DX2,  pt.y(),
					sw.vx, sw.vy,
					false,
        			Qt::black,
					qBound (24.0, DX2*0.4, 32.0),
					true
					);
	}	
}
//------------------------------------------------------
void  SkewT::draw_linesCAPE (QPainter &pnt)
{
	if (! hasSoundingData())
		return;
	
	double hpa0mean = (hpa0min+hpa0max)/2.0;
	
	TPoint LCL = sounding.get_LCL (hpa0max, hpa0min);
	TPoint CCL = sounding.get_CCL (hpa0max, hpa0min);
	TPoint LFC = sounding.get_LFC (hpa0max, hpa0min);
	TPoint EL  = sounding.get_EL  (hpa0max, hpa0min);
	
	TPoint T0  (sounding.getAvgTempCByAlt (hpa0min, hpa0max), hpa0mean);
	TPoint DP0 (sounding.getAvgDewpCByAlt (hpa0min, hpa0max), hpa0mean);
	
	if (! LCL.ok())
		return;
	
	TPCurve curve;
	Therm::curveSaturatedAdiabatic (&curve, LCL, hpaMin, -1);
	PersPath pathCape(this);
	pathCape.fromTPCurve (&curve);

	pnt.save ();
	QPen penCape (linesCAPEColor);
	penCape.setWidthF (1);
	penCape.setStyle (Qt::SolidLine);
	pnt.setPen (penCape);
	
	pnt.drawPath (pathCape);
	
	QPointF pLCL = tempPressPoint (LCL);
	QPointF pT0  = tempPressPoint (T0);
	QPointF pDP0 = tempPressPoint (DP0);
	QPointF pEL  = tempPressPoint (EL);
	QPointF pCCL = tempPressPoint (CCL);
	QPointF pLFC = tempPressPoint (LFC);

	if (T0.ok())
		pnt.drawLine (pLCL, pT0);
	if (DP0.ok())
		pnt.drawLine (pLCL, pDP0);
	
	penCape.setStyle (Qt::DashLine);
	pnt.setPen (penCape);
	if (CCL.ok())
		pnt.drawLine (pLCL, pCCL);
	
	//-------------------------------------
	penCape.setStyle (Qt::SolidLine);
	pnt.setPen (penCape);	
	pnt.setBrush (linesCAPEColor);
	double r = 3;
	if (LCL.ok())
		pnt.drawEllipse (pLCL, r,r);
	if (CCL.ok())
		pnt.drawEllipse (pCCL, r,r);
	if (LFC.ok())
		pnt.drawEllipse (pLFC, r,r);
	if (EL.ok())
		pnt.drawEllipse (pEL, r,r);
	if (T0.ok()) {
		pnt.setBrush (soundingTempColor);
		pnt.drawEllipse (pT0, r,r);
	}
	if (DP0.ok()) {
		pnt.setBrush (soundingDewpColor);
		pnt.drawEllipse (pDP0, r,r);
	}
	pnt.restore ();
}
//------------------------------------------------------
void  SkewT::draw_comments (QPainter &pnt)
{
	QFontMetrics fmet (pnt.font());
	double fw = fmet.width ("W");
	double fh = fmet.lineSpacing ();
	pnt.save ();
	QColor commentsColor = QColor (70,70,70);
	
	QPen pen (commentsColor);
	pnt.setPen (pen);
	//-------------------------------------------------------
	TPoint LCL = sounding.get_LCL (hpa0max, hpa0min);
	TPoint CCL = sounding.get_CCL (hpa0max, hpa0min);
	TPoint LFC = sounding.get_LFC (hpa0max, hpa0min);
	TPoint EL  = sounding.get_EL  (hpa0max, hpa0min);
	
	double hpa0mean = (hpa0min+hpa0max)/2.0;
	TPoint T0  (sounding.getTempCByAlt (hpa0mean), hpa0mean);
	TPoint DP0 (sounding.getDewpCByAlt (hpa0mean), hpa0mean);
	
	QString tBase = "---";
	if (hasSoundingData()) {
		if (showConvArea) {
			if (hpa0max == hpa0min) {
				tBase = QString ("%1hPa").arg(qRound(hpa0max));
			}
			else {
				tBase = QString ("Avg%1-%2hPa").arg(qRound(hpa0max)).arg(qRound(hpa0min));
			}
		}
	}
	QString tT0, tDP0, tLCL, tCCL, tLFC, tEL;
	if (LCL.ok() && showConvArea)
		tLCL = QString("LCL: %1hPa,%2°C").arg(qRound(LCL.hpa)).arg(LCL.tempC,0,'f',1);
	else 
		tLCL = QString("LCL: ---");
	if (CCL.ok() && showConvArea)
		tCCL = QString("CCL: %1hPa,%2°C").arg(qRound(CCL.hpa)).arg(CCL.tempC,0,'f',1);
	else 
		tCCL = QString("CCL: ---");
	if (LFC.ok() && showConvArea)
		tLFC = QString("LFC: %1hPa,%2°C").arg(qRound(LFC.hpa)).arg(LFC.tempC,0,'f',1);
	else 
		tLFC = QString("LFC: ---");
	if (EL.ok() && showConvArea)
		tEL = QString("EL: %1hPa,%2°C").arg(qRound(EL.hpa)).arg(EL.tempC,0,'f',1);
	else 
		tEL = QString("EL: ---");
	
	if (T0.ok() && showConvArea)
		tT0 = QString("T: %1°C").arg(T0.tempC,0,'f',1);
	else 
		tT0 = QString("T: ---");
	
	if (DP0.ok() && showConvArea)
		tDP0 = QString("Td: %1°C").arg(DP0.tempC,0,'f',1);
	else 
		tDP0 = QString("Td: ---");
	
	QString tSfcHpa = surfaceHpa==GRIB_NOTDEF ? "Sfc: ---"
						: QString("Sfc: %1hPa").arg(surfaceHpa,0,'f',0);
						
	QString indLI = sounding.LI==GRIB_NOTDEF || !showConvArea ? "LI: ---"
						: QString("LI: %1").arg(sounding.LI,0,'f',0);
	QString indSI = sounding.SI==GRIB_NOTDEF || !showConvArea ? "SI: ---"
						: QString("SI: %1").arg(sounding.SI,0,'f',0);
	QString indKI = sounding.KI==GRIB_NOTDEF || !showConvArea ? "KI: ---"
						: QString("KI: %1").arg(sounding.KI,0,'f',0);
	QString indTT = sounding.TT==GRIB_NOTDEF || !showConvArea ? "TT: ---"
						: QString("TT: %1").arg(sounding.TT,0,'f',0);
	QString indSW = sounding.SWEAT==GRIB_NOTDEF || !showConvArea ? "SWEAT: ---"
						: QString("SWEAT: %1").arg(sounding.SWEAT,0,'f',0);
	QString indCAPE = sounding.CAPE==GRIB_NOTDEF || !showConvArea ? "CAPE: ---"
						: QString("CAPE: %1").arg(sounding.CAPE,0,'f',0);
	QString indCIN = sounding.CIN==GRIB_NOTDEF || !showConvArea ? "CIN: ---"
						: QString("CIN: %1").arg(sounding.CIN,0,'f',0);
	
	QString tModelCAPECIN = tr("Model:");
	if (ModelCAPE!=GRIB_NOTDEF)
		tModelCAPECIN += " CAPE: "+QString("%1").arg(qRound(ModelCAPE));
	else
		tModelCAPECIN += " CAPE: ---";
	if (ModelCIN!=GRIB_NOTDEF)
		tModelCAPECIN += " CIN: "+QString("%1").arg(qRound(ModelCIN));
	else
		tModelCAPECIN += "  CIN: ---";
	
	// General information
	QString line1 = QString("Base: %1  %2  %3      ").arg(tBase).arg(tT0).arg(tDP0)
				+tr("Approx levels:") + QString("  %4   %5   %6   %7")
						.arg(tLCL).arg(tCCL).arg(tLFC).arg(tEL);
	// Indices
	QString line2 = QString("%1          ").arg(tSfcHpa)
			+tr("Approx.indices:") 
			+ QString("  %1   %2   %3   %4   %5     %6  %7          %8")					
				.arg(indLI).arg(indSI).arg(indKI).arg(indTT).arg(indSW)
				.arg(indCAPE).arg(indCIN)
				.arg(tModelCAPECIN);
						
	pnt.drawText (fw, H-fh-5, line1);
	pnt.drawText (fw, H-5, line2);
	
	//-------------------------------------------------------
	QString loc = tr("Location: ")+location;
	pnt.drawText (fw, fh, loc);
	
	QString ref = tr("Reference: ")+reference;
	pnt.drawText (fw, 2*fh, ref);
	
	QString dat = tr("Date: ")+curdate;
	pnt.drawText (W/2, 2*fh, dat);
	//-------------------------------------------------------
	pnt.drawText (W-fmet.width("zyGrib")-5, H-5, "zyGrib");
	//-------------------------------------------------------
	QFont font = pnt.font();
	font.setFamily ("times");
	font.setBold (true);
	font.setStyle (QFont::StyleItalic);
	font.setPointSize (W/5);
	pnt.setFont (font);
	
	QColor watermarkColor = QColor (252,250,248);	
	pen.setColor (watermarkColor);
	pen.setWidthF (4);
	pnt.setPen (pen);
	pnt.translate (W/2, H/2);
	pnt.rotate (-30);
	pnt.translate (-W/2, -H/2);
	QPainterPath pat;
	pat.addText (W/6, 3*H/5, font, "zyGrib");
	pnt.drawPath (pat);
	
	pnt.restore ();
}
//------------------------------------------------------
void  SkewT::draw_sounding (QPainter &pnt)
{
	if (hasSoundingData()) {
		// Sounding
		QPen penTemp (soundingTempColor);	// Temp = red
		QPen penDewp (soundingDewpColor);	// Diewpoint = blue
		penTemp.setWidthF (2);
		penDewp.setWidthF (2);
		penDewp.setStyle (Qt::DashLine);

		QPointF pt;
		PersPath pathTemp(this), pathDewp(this);
		// Temperature and dew-points
		foreach (SoundingPoint sp, *sounding.getAllSounds()) 
		{
			pt = tempPressPoint (sp.tempC, sp.hpa);
			pathTemp.addPointNoClip (pt);
			pt = tempPressPoint (sp.dewpC, sp.hpa);
			pathDewp.addPointNoClip (pt);
		}	
		pnt.setClipping (true);
			pnt.setPen (penTemp);
			pnt.drawPath (pathTemp);
			pnt.setPen (penDewp);
			pnt.drawPath (pathDewp);
		pnt.setClipping (false);
	}
	else {
		// Write message "no data"
		pnt.save ();
		QFont font = pnt.font();
		font.setFamily ("times");
		font.setStyle (QFont::StyleItalic);
		font.setPointSize (W/10);
		QColor messageColor = QColor (100,100,100);	
		QPen pen;
		pen.setColor (messageColor);
		pnt.setPen (pen);
		pnt.setFont (font);
		pnt.drawText (W/5, 3*H/5, "NO DATA");
		pnt.restore ();
	}
}







