/**********************************************************************
XyGrib: meteorological GRIB file viewer
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
 	colors_Gust.readFile (Util::pathColors()+"colors_gust_kts.txt", 1.852/3.6, 0);
	colors_Wind_Jet.readFile (Util::pathColors()+"colors_wind_jet_kts.txt", 1.852/3.6, 0);
	colors_Current.readFile (Util::pathColors()+"colors_current_kts.txt", 1.852/3.6, 0);
	colors_Temp.readFile (Util::pathColors()+"colors_temp_celcius.txt", 1, 273.15);
	colors_WaterTemp.readFile (Util::pathColors()+"colors_sea_temp_celcius.txt", 1, 273.15);
	colors_Rain.readFile (Util::pathColors()+"colors_rain_mmh.txt", 1, 0);
	colors_SnowDepth.readFile (Util::pathColors()+"colors_snowdepth_m.txt", 1, 0);
	colors_CloudsWhite.readFile (Util::pathColors()+"colors_clouds_white_pc.txt", 1, 0);
	colors_CloudsBlack.readFile (Util::pathColors()+"colors_clouds_black_pc.txt", 1, 0);
	colors_CAPE.readFile (Util::pathColors()+"colors_cape_jkg.txt", 1, 0);
    colors_CIN.readFile (Util::pathColors()+"colors_cin_jkg.txt", 1, 0);
    // added by david
    colors_Reflectivity.readFile (Util::pathColors()+"colors_reflect_dbz.txt", 1, 0);
    colors_HumidRel.readFile (Util::pathColors()+"colors_humidrel_pc.txt", 1, 0);
	colors_DeltaTemp.readFile (Util::pathColors()+"colors_deltatemp_celcius.txt", 1, 0);
	colors_Binary.readFile (Util::pathColors()+"colors_binary.txt", 1, 0);
	colors_WaveHeight.readFile (Util::pathColors()+"colors_waveheight_m.txt", 1, 0);
	colors_WhiteCap.readFile (Util::pathColors()+"colors_whitecap_prb.txt", 1, 0);
	colors_ThetaE.readFile (Util::pathColors()+"colors_thetae_celcius.txt", 1, 273.15);
#if 0
	//test
	double v;
	
	v= 170;
	colors_Temp.getColor (v, true);
#endif
}
//--------------------------------------------------------------------------
QColor DataColors::getContrastedColor (const QColor &base)
{
	double gris = 0.30*base.redF() + 0.59*base.greenF() + 0.11*base.blueF();
	if (gris < 0.35)
		return {230,225,200};
    if (gris < 0.45)
		return {240,235,210};

    return Qt::black;
}
//--------------------------------------------------------------------------
QRgb DataColors::getWindColor (double v, bool smooth)  {
	return colors_Wind.getColor (v, smooth);
}	
//--------------------------------------------------------------------------
QRgb DataColors::getGustColor (double v, bool smooth)  {
	return colors_Gust.getColor (v, smooth);
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
QRgb  DataColors::getWaterTemperatureColor (double v, bool smooth) {
	return colors_WaterTemp.getColor (v, smooth);
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
//-----added by david ------------------------------------------------------
QRgb  DataColors::getReflectColor (double v, bool smooth) {
    return colors_Reflectivity.getColor (v, smooth);
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
		v = Util::inRange(v, 0., 100.);
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
auto DataColors::getFunctionColor(const DataCode &dtc) -> QRgb (DataColors::*)(double v, bool smooth)
{
	switch (dtc.dataType) {
		case GRB_WIND_GUST : return &DataColors::getGustColor;
		case GRB_WIND_SPEED :
		case GRB_PRV_WIND_XY2D :
			return &DataColors::getWindColor;
		case GRB_PRV_WIND_JET : return  &DataColors::getWindJetColor;
		case GRB_PRV_THETA_E :  return &DataColors::getThetaEColor;
		case GRB_CUR_SPEED :
		case GRB_PRV_CUR_XY2D :
			return &DataColors::getCurrentColor;
		case GRB_PRV_DIFF_TEMPDEW : return  &DataColors::getDeltaTemperaturesColor;
		case GRB_CLOUD_TOT :        return &DataColors::getCloudColor;
		case GRB_PRECIP_TOT :
        case GRB_PRECIP_RATE  :
            return &DataColors::getRainColor;
		case GRB_HUMID_REL :     return &DataColors::getHumidColor;
		case GRB_WTMP :          return &DataColors::getWaterTemperatureColor;
		case GRB_TEMP :          return &DataColors::getTemperatureColor;
		case GRB_TEMP_POT :      return &DataColors::getTemperatureColor;
		case GRB_DEWPOINT :      return &DataColors::getTemperatureColor;
		case GRB_SNOW_DEPTH :    return &DataColors::getSnowDepthColor;
		case GRB_SNOW_CATEG :    return &DataColors::getBinaryColor;
		case GRB_FRZRAIN_CATEG : return &DataColors::getBinaryColor;
		case GRB_CAPE :          return &DataColors::getCAPEColor;
		case GRB_CIN :           return &DataColors::getCINColor;
        case GRB_COMP_REFL :     return &DataColors::getReflectColor;
		case GRB_WAV_SIG_HT :
		case GRB_WAV_MAX_HT :
			return &DataColors::getWaveHeightColor;
		case GRB_WAV_WHITCAP_PROB : return &DataColors::getWhiteCapColor;
			break;
		default :
			break;
	}
	return &DataColors::getWindColor;	// why not
}

//--------------------------------------------------------------------------
void DataColors::setColorDataTypeFunction (const DataCode &dtc)
{
	function_getColor = getFunctionColor(dtc);
}
 
//--------------------------------------------------------------------------
QRgb DataColors::getDataCodeColor (const DataCode &dtc, double v, bool smooth)
{
	QRgb (DataColors::*fn) (double v, bool smooth);
	fn = getFunctionColor(dtc);
	return (this->*fn)(v, smooth);
}

//--------------------------------------------------------------------------
ColorScale *DataColors::getColorScale (const DataCode &dtc)
{
// 	DBGQS(DataCodeStr::toString(dtc));
	switch (dtc.dataType) {
		case GRB_WIND_GUST :
			return &colors_Gust;
		case GRB_WIND_SPEED :
		case GRB_PRV_WIND_XY2D :
			return &colors_Wind;
		case GRB_PRV_WIND_JET :
			return &colors_Wind_Jet;
		case GRB_PRV_THETA_E :
			return &colors_ThetaE;
		case GRB_CUR_SPEED :
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
        case GRB_PRECIP_RATE  :
            return &colors_Rain;
		case GRB_HUMID_REL :
			return &colors_HumidRel;
		case GRB_WTMP :
			return &colors_WaterTemp;
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
        // added by david
        case GRB_COMP_REFL :
            return &colors_Reflectivity;
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
