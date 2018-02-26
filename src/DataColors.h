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

#ifndef DATACOLORS_H
#define DATACOLORS_H

#include <QColor>

#include "DataDefines.h"
#include "DataQString.h"
#include "Util.h"
#include "ColorScale.h"

class DataColors    // inherited by GriddedPlotter
{ 
	public:
		DataColors ();
		
		void setColorDataTypeFunction (const DataCode &dtc);

		//----------------------------
		// Map colors 
		//----------------------------
		void   setCloudsColorMode (QString settingName)
			{ isCloudsColorModeWhite 
				= Util::getSetting(settingName,"white").toString()=="white"; }
		
        QRgb   getWindColor     (double v, bool smooth);
        QRgb   getWindJetColor  (double v, bool smooth);
        QRgb   getAltitudeColor (double v, const Altitude &alt, bool smooth);
        QRgb   getRainColor     (double mm, bool smooth);
        QRgb   getSnowDepthColor(double mm, bool smooth);
        QRgb   getHumidColor    (double v, bool smooth);
        QRgb   getCurrentColor     (double v, bool smooth);
        QRgb   getTemperatureColor (double v, bool smooth);
        QRgb   getPressureColor    (double v, bool smooth);
		QRgb   getDeltaTemperaturesColor (double v, bool smooth);
        QRgb   getCAPEColor    (double v, bool smooth);
        QRgb   getCINColor    (double v, bool smooth);
		QRgb   getCloudColor   (double v, bool smooth);
		QRgb   getBinaryColor  (double v, bool smooth);
        QRgb   getWaveHeightColor    (double v, bool smooth);
        QRgb   getWhiteCapColor    (double v, bool smooth);
        QRgb   getThetaEColor    (double v, bool smooth);

		ColorScale *getColorScale (const DataCode &dtc);
					
		QRgb getDataCodeColor (const DataCode &dtc, double v, bool smooth);
		
		static QColor getContrastedColor (const QColor &base);

	protected:
		ColorScale colors_Wind;
		ColorScale colors_Wind_Jet;
		ColorScale colors_Current;
		ColorScale colors_Temp;
		ColorScale colors_Rain;
		ColorScale colors_SnowDepth;
		ColorScale colors_CloudsWhite;
		ColorScale colors_CloudsBlack;
		ColorScale colors_CAPE;
		ColorScale colors_CIN;
		ColorScale colors_HumidRel;
		ColorScale colors_DeltaTemp;
		ColorScale colors_Binary;
		ColorScale colors_WaveHeight;
		ColorScale colors_WhiteCap;
		ColorScale colors_ThetaE;

        int    mapColorTransp;
		bool   isCloudsColorModeWhite;
		
		QRgb  pasteToWindColorScale (double v, double min, double max, bool smooth);
		
		QRgb (DataColors::*function_getColor) (double v, bool smooth);
};

#endif
