/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008-2012 - Jacques Zaninetti - http://www.zygrib.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/
#include <QPainter>

#include "ColorScaleWidget.h" 
#include "DataQString.h" 
#include "Font.h" 

//================================================================
ColorScaleGraph::ColorScaleGraph (QWidget *parent) : QWidget(parent)
{
	sc = NULL;
}
//---------------------------------------------------------------
void ColorScaleGraph::setColorScale (DataColors *datacolors, const DataCode& dtc)
{
// 	DBGQS(DataCodeStr::toString(dtc));
	this->datacolors = datacolors;
	this->sc = datacolors->getColorScale (dtc);
	this->dtc = dtc;
	update ();
}
//---------------------------------------------------------------
void ColorScaleGraph::paintEvent (QPaintEvent *)
{
	int W=width(), H=height();		
    QPainter pnt(this);
	QColor bgcolor, fgcolor;
	int dh = 16;
	pnt.fillRect (0,0,W,dh, QColor(210,210,255));
	bgcolor = Util::getSetting("landColor", QColor(200,200,120)).value<QColor>();
	pnt.fillRect (0,dh,W,H-dh, bgcolor);
	int i;
	if (sc) {
		pnt.setPen (QColor(0,0,0));
		QFont font = Font::getFont(FONT_ColorScale);
		font.setBold (true);
		pnt.setFont (font);
		pnt.drawText (0, 0, W,dh, Qt::AlignHCenter|Qt::AlignVCenter, Util::getDataUnit (dtc));
		int nb = sc->colors.size();
		if (nb == 0)
				return;
		pnt.setFont (Font::getFont(FONT_ColorScale));
		int h1;
		int hc = (H-dh)/nb;
		QRgb rgb;
		for (i=0; i<nb; i++) {
			// Background
			double v = sc->colors[i]->vmax;
			rgb = datacolors->getDataCodeColor (dtc, v, true);
			bgcolor.setRgba (rgb);
			h1 = dh + (nb-i-1)*hc;
			pnt.fillRect (0, h1, W,hc, bgcolor);
			//DBG("i=%d H=%d dh=%d : h1=%d : (%d %d %d)", i,H,dh, h1, r,g,b);
			
			// Foreground (value)
			pnt.setPen (DataColors::getContrastedColor (bgcolor));
			QString sval = formatValue (v);
			//pnt.drawText (0, h1, W,hc, Qt::AlignHCenter|Qt::AlignTop, sval);
			pnt.drawText (0, h1, W,hc, Qt::AlignHCenter|Qt::AlignVCenter, sval);
		}
	}
}
//------------------------------------------------------------
QString ColorScaleGraph::formatValue (double v)
{
	float t0, tv;
	switch (dtc.dataType) {
		case GRB_PRV_WIND_XY2D :
		case GRB_PRV_WIND_JET :
			return Util::formatSpeed_Wind (v, false);
		case GRB_PRV_CUR_XY2D :
			return Util::formatSpeed_Current (v, false);
		case GRB_PRV_DIFF_TEMPDEW :
			t0 = Util::convertTemperature (273.15);
			tv = Util::convertTemperature (v+273.15);
			return QString("%1").arg(fabs(tv-t0),0,'f',1);
		case GRB_CLOUD_TOT : 
			return Util::formatPercentValue (v, false);
		case GRB_PRECIP_TOT :
			return QString("%1").arg(v,0,'f',2);
		case GRB_HUMID_REL :
			return Util::formatPercentValue (v, false);
		case GRB_TEMP :
		case GRB_TEMP_POT :
		case GRB_DEWPOINT :
		case GRB_PRV_THETA_E :
			return Util::formatTemperature_short (v, false);
		case GRB_SNOW_DEPTH :
			return Util::formatSnowDepth (v, false);
		case GRB_SNOW_CATEG :
			return QString("%1").arg(v,0,'f',0);
		case GRB_FRZRAIN_CATEG :
			return QString("%1").arg(v,0,'f',0);
		case GRB_CAPE :
			return Util::formatCAPEsfc (v, false);
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
			return Util::formatWaveHeight (v, false);
		case GRB_WAV_WHITCAP_PROB :
			return Util::formatWhiteCap (v, false);
		default :
			return QString("%1").arg(v,0,'f',0);
	}
}

//================================================================
ColorScaleWidget::ColorScaleWidget (QWidget *parent)
    : QDockWidget(parent)
{
	this->setObjectName ("ColorScaleWidget");
	this->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->setWindowTitle (tr("Colors"));
	setMinimumWidth (15);
	setMaximumWidth (40);
	
	graph = new ColorScaleGraph (this);
    assert(graph);
	setWidget (graph);
	
	setVisible (true);
}
//---------------------------------------------------------------
void ColorScaleWidget::setColorScale (DataColors *datacolors, const DataCode& dtc)
{
	graph->setColorScale (datacolors, dtc);
}
