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

#include <cstdlib>
#include "DataColors.h"

//--------------------------------------------------------------------
DataColors::DataColors ()
{
	setCloudsColorMode ("cloudsColorMode");	
	mapColorTransp = 255;
	function_getColor = &DataColors::getWindColor;	// why not
	
 	colors_Wind.readFile (Util::pathColors()+"colors_wind_kts.txt", 1.852/3.6, 0);
	colors_Wind_Jet.readFile (Util::pathColors()+"colors_wind_jet_kts.txt", 1.852/3.6, 0);
	colors_Current.readFile (Util::pathColors()+"colors_current_kts.txt", 1.852/3.6, 0);
	colors_Temp.readFile (Util::pathColors()+"colors_temp_celcius.txt", 1, 273.15);
	colors_Rain.readFile (Util::pathColors()+"colors_rain_mmh.txt", 1, 0);
	colors_SnowDepth.readFile (Util::pathColors()+"colors_snowdepth_m.txt", 1, 0);
	colors_CloudsWhite.readFile (Util::pathColors()+"colors_clouds_white_pc.txt", 1, 0);
	colors_CloudsBlack.readFile (Util::pathColors()+"colors_clouds_black_pc.txt", 1, 0);
	colors_CAPE.readFile (Util::pathColors()+"colors_cape_jkg.txt", 1, 0);
	colors_CIN.readFile (Util::pathColors()+"colors_cin_jkg.txt", 1, 0);
	colors_HumidRel.readFile (Util::pathColors()+"colors_humidrel_pc.txt", 1, 0);
	colors_DeltaTemp.readFile (Util::pathColors()+"colors_deltatemp_celcius.txt", 1, 0);
	colors_Binary.readFile (Util::pathColors()+"colors_binary.txt", 1, 0);
	colors_WaveHeight.readFile (Util::pathColors()+"colors_waveheight_m.txt", 1, 0);
	colors_WhiteCap.readFile (Util::pathColors()+"colors_whitecap_prb.txt", 1, 0);
	colors_ThetaE.readFile (Util::pathColors()+"colors_thetae_celcius.txt", 1, 273.15);
	//test
	double v;
	
	v= 170;
	colors_Temp.getColor (v, true);
}
//--------------------------------------------------------------------------
QColor DataColors::getContrastedColor (const QColor &base)
{
	double gris = 0.30*base.redF() + 0.59*base.greenF() + 0.11*base.blueF();
	if (gris < 0.35)
		return QColor(230,225,200);
	else if (gris < 0.45)
		return QColor(240,235,210);
	else
		return Qt::black;
}
//--------------------------------------------------------------------------
QRgb DataColors::getWindColor (double v, bool smooth)  {
	return colors_Wind.getColor (v, smooth);
}	
//--------------------------------------------------------------------------
QRgb DataColors::getWindJetColor (double v, bool smooth)  {
	return colors_Wind_Jet.getColor (v, smooth);
}	
//--------------------------------------------------------------------------
QRgb DataColors::getCurrentColor (double v, bool smooth)  {
	return colors_Current.getColor (v, smooth);
}	
//--------------------------------------------------------------------------
QRgb  DataColors::getTemperatureColor (double v, bool smooth) {
	return colors_Temp.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getThetaEColor (double v, bool smooth) {
	return colors_ThetaE.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getRainColor (double v, bool smooth) {
	return colors_Rain.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getSnowDepthColor (double v, bool smooth) {
	return colors_SnowDepth.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getCAPEColor (double v, bool smooth) {
	return colors_CAPE.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getCINColor (double v, bool smooth) {
	return colors_CIN.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getHumidColor (double v, bool smooth) {
	return colors_HumidRel.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getBinaryColor (double v, bool smooth) {
	return colors_Binary.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getWaveHeightColor (double v, bool smooth) {
	return colors_WaveHeight.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getWhiteCapColor (double v, bool smooth) {
	return colors_WhiteCap.getColor (v, smooth);
} 
//--------------------------------------------------------------------------
QRgb  DataColors::getCloudColor (double v, bool smooth) {
    QRgb rgb;
    int tr;
    if (isCloudsColorModeWhite) {
		rgb = colors_CloudsWhite.getColor (v, smooth);
		tr = (int)(2.5*v);
	}
	else {
		rgb = colors_CloudsBlack.getColor (v, smooth);
		tr = mapColorTransp;
	}
    return qRgba (qRed(rgb), qGreen(rgb), qBlue(rgb), tr);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getDeltaTemperaturesColor (double v, bool smooth) {
	return colors_DeltaTemp.getColor (v, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getAltitudeColor (double v, const Altitude &alt, bool smooth) {
	double k=0.001, vmin=0, vmax=20000, vmoy=10000;
	if (alt == Altitude(LV_ISOTHERM0,0)) {
		vmin = 0;  vmax = 5000;
	}
	else if (alt.levelType == LV_ISOBARIC) {
		switch (alt.levelValue) {
			case 200 :
				vmoy = 11800; k=0.06; break;
			case 300 :
				vmoy = 9200; k=0.08; break;
			case 400 :
				vmoy = 7200; k=0.09;  break;
			case 500 :
				vmoy = 5600; k=0.10;  break;
			case 600 :
				vmoy = 4200; k=0.12;  break;
			case 700 :
				vmoy = 3000; k=0.15; break;
			case 850 :
				vmoy = 1460; k=0.25; break;
			case 925 :
				vmoy = 760; k=0.30; break;
		}
		vmin = vmoy - k*vmoy;
		vmax = vmoy + k*vmoy;
	}
	return pasteToWindColorScale (v, vmin, vmax, smooth);
}
//--------------------------------------------------------------------------
QRgb  DataColors::getPressureColor (double v, bool smooth)
{
	// Même échelle colorée que pour le vent
	double x = v/100.0;	// Pa->hPa
	double t0 = 960;  // valeur mini de l'échelle
	double t1 = 1050;  // valeur maxi de l'échelle
	return pasteToWindColorScale (x, t0, t1, smooth);
} 
//--------------------------------------------------------------------------
QRgb  DataColors::pasteToWindColorScale 
						(double v, double min, double max, bool smooth)
{
	double b0 = 0;    // min beauforts
	double b1 = 12;   // max beauforts
	double eqbeauf = b0 + (v-min)*(b1-b0)/(max-min);
	if (eqbeauf < 0)
		eqbeauf = 0;
	else if (eqbeauf > 12)
		eqbeauf = 12;
	return getWindColor (Util::BeaufortToMs_F(eqbeauf), smooth);
}
//--------------------------------------------------------------------------
void DataColors::setColorDataTypeFunction (const DataCode &dtc)
{
	switch (dtc.dataType) {
		case GRB_PRV_WIND_XY2D :
			function_getColor = &DataColors::getWindColor;
			break;
		case GRB_PRV_WIND_JET :
			function_getColor = &DataColors::getWindJetColor;
			break;
		case GRB_PRV_THETA_E :
			function_getColor = &DataColors::getThetaEColor;
			break;
		case GRB_PRV_CUR_XY2D :
			function_getColor = &DataColors::getCurrentColor;
			break;
		case GRB_PRV_DIFF_TEMPDEW :
			function_getColor = &DataColors::getDeltaTemperaturesColor;
			break;
		case GRB_CLOUD_TOT : 
			function_getColor = &DataColors::getCloudColor;
			break;
		case GRB_PRECIP_TOT :
			function_getColor = &DataColors::getRainColor;
			break;
		case GRB_HUMID_REL :
			function_getColor = &DataColors::getHumidColor;
			break;
		case GRB_TEMP :
			function_getColor = &DataColors::getTemperatureColor;
			break;
		case GRB_TEMP_POT :
			function_getColor = &DataColors::getTemperatureColor;
			break;
		case GRB_DEWPOINT :
			function_getColor = &DataColors::getTemperatureColor;
			break;
		case GRB_SNOW_DEPTH :
			function_getColor = &DataColors::getSnowDepthColor;
			break;
		case GRB_SNOW_CATEG :
			function_getColor = &DataColors::getBinaryColor;
			break;
		case GRB_FRZRAIN_CATEG :
			function_getColor = &DataColors::getBinaryColor;
			break;
		case GRB_CAPE :
			function_getColor = &DataColors::getCAPEColor;
			break;
		case GRB_CIN :
			function_getColor = &DataColors::getCINColor;
			break;
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
			function_getColor = &DataColors::getWaveHeightColor;
			break;
		case GRB_WAV_WHITCAP_PROB :
			function_getColor = &DataColors::getWhiteCapColor;
			break;
		default :
			function_getColor = &DataColors::getWindColor;	// why not
			break;
	}
}
 
//--------------------------------------------------------------------------
QRgb DataColors::getDataCodeColor (const DataCode &dtc, double v, bool smooth)
{
	switch (dtc.dataType) {
		case GRB_PRV_WIND_XY2D :
			return DataColors::getWindColor (v, smooth);
		case GRB_PRV_WIND_JET :
			return DataColors::getWindJetColor (v, smooth);
		case GRB_PRV_THETA_E :
			return DataColors::getThetaEColor (v, smooth);
		case GRB_PRV_CUR_XY2D :
			return DataColors::getCurrentColor (v, smooth);
		case GRB_PRV_DIFF_TEMPDEW :
			return DataColors::getDeltaTemperaturesColor (v, smooth);
		case GRB_CLOUD_TOT : 
			return DataColors::getCloudColor (v, smooth);
		case GRB_PRECIP_TOT :
			return DataColors::getRainColor (v, smooth);
		case GRB_HUMID_REL :
			return DataColors::getHumidColor (v, smooth);
		case GRB_TEMP :
			return DataColors::getTemperatureColor (v, smooth);
		case GRB_TEMP_POT :
			return DataColors::getTemperatureColor (v, smooth);
		case GRB_DEWPOINT :
			return DataColors::getTemperatureColor (v, smooth);
		case GRB_SNOW_DEPTH :
			return DataColors::getSnowDepthColor (v, smooth);
		case GRB_SNOW_CATEG :
			return DataColors::getBinaryColor (v, smooth);
		case GRB_FRZRAIN_CATEG :
			return DataColors::getBinaryColor (v, smooth);
		case GRB_CAPE :
			return DataColors::getCAPEColor (v, smooth);
		case GRB_CIN :
			return DataColors::getCINColor (v, smooth);
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
			return DataColors::getWaveHeightColor (v, smooth);
		case GRB_WAV_WHITCAP_PROB :
			return DataColors::getWhiteCapColor (v, smooth);
		default :
			return DataColors::getWindColor (v, smooth);	// why not
	}
}

//--------------------------------------------------------------------------
ColorScale *DataColors::getColorScale (const DataCode &dtc)
{
// 	DBGQS(DataCodeStr::toString(dtc));
	switch (dtc.dataType) {
		case GRB_PRV_WIND_XY2D :
			return &colors_Wind;
		case GRB_PRV_WIND_JET :
			return &colors_Wind_Jet;
		case GRB_PRV_THETA_E :
			return &colors_ThetaE;
		case GRB_PRV_CUR_XY2D :
			 return &colors_Current;
		case GRB_PRV_DIFF_TEMPDEW :
			return &colors_DeltaTemp;
		case GRB_CLOUD_TOT : 
			if (isCloudsColorModeWhite)
				return &colors_CloudsWhite;
			else
				return &colors_CloudsBlack;
		case GRB_PRECIP_TOT :
			return &colors_Rain;
		case GRB_HUMID_REL :
			return &colors_HumidRel;
		case GRB_TEMP :
			return &colors_Temp;
		case GRB_TEMP_POT :
			return &colors_Temp;
		case GRB_DEWPOINT :
			return &colors_Temp;
		case GRB_SNOW_DEPTH :
			return &colors_SnowDepth;
		case GRB_SNOW_CATEG :
			return &colors_Binary;
		case GRB_FRZRAIN_CATEG :
			return &colors_Binary;
		case GRB_CAPE :
			return &colors_CAPE;
		case GRB_CIN :
			return &colors_CIN;
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
			return &colors_WaveHeight;
		case GRB_WAV_WHITCAP_PROB :
			return  &colors_WhiteCap;
			break;
		default :
			return &colors_Wind;
	}
}

					







