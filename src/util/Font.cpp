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


#include <QDir>
#include <QFontDatabase>
#include "QApplication"
#include "Font.h"
#include "Util.h"

QFont GLOB_Font [FONT_MaxValue];
	
//-------------------------------------------------------------------
QString Font::getFontCodeString (FontCode code) {
	switch (code) {
		case FONT_Default :   return "FONT_Default";
		case FONT_ComboBox :  return "FONT_ComboBox";
		case FONT_StatusBar : return "FONT_StatusBar";
		case FONT_MenuBar : return "FONT_MenuBar";
		case FONT_MapWait :   return "FONT_MapWait";
		case FONT_MapInfo_Big : return "FONT_MapInfo_Big";
		case FONT_MapInfo_Medium :  return "FONT_MapInfo_Medium";
		case FONT_MapInfo_Small :   return "FONT_MapInfo_Small";
		case FONT_MapInfo_DateRef : return "FONT_MapInfo_DateRef";
		case FONT_LonLatGridLabel : return "FONT_LonLatGridLabel";
		case FONT_IsolineLabel : return "FONT_IsolineLabel";
		case FONT_MapCountry :   return "FONT_MapCountry";
		case FONT_POILabel :  return "FONT_POILabel";
		case FONT_MapCity_1 : return "FONT_MapCity_1";
		case FONT_MapCity_2 : return "FONT_MapCity_2";
		case FONT_MapCity_3 : return "FONT_MapCity_3";
		case FONT_MapCity_4 : return "FONT_MapCity_4";
		case FONT_MapCity_5 : return "FONT_MapCity_5";
		case FONT_GRIB_Temp : return "FONT_GRIB_Temp";
		case FONT_GRIB_PressHL : return "FONT_GRIB_PressHL";
		case FONT_IAC_Isobar :   return "FONT_IAC_Isobar";
		case FONT_IAC_PressHLVal : return "FONT_IAC_PressHLVal";
		case FONT_IAC_PressHL : return "FONT_IAC_PressHL";
		case FONT_AltitudeLabel : return "FONT_AltitudeLabel";
		case FONT_AltitudeLabelTitle : return "FONT_AltitudeLabelTitle";
		case FONT_DateChooser :   return "FONT_DateChooser";
		case FONT_ColorScale :   return "FONT_ColorScale";
		default:
			return "FONT_Default";
	}
}

//-------------------------------------------------------------------
QFont Font::getDefaultFont (FontCode code) {
	switch (code) {
		case FONT_Default :   return QFont("Liberation Sans",11,QFont::Normal,false);
		case FONT_ComboBox :  return QFont("Liberation Mono",10,QFont::Normal,false);
		case FONT_StatusBar : return QFont("Liberation Mono",10,QFont::Normal,false);
		case FONT_MenuBar :   return QFont("Liberation Sans",11,QFont::Normal,false);
		case FONT_MapWait :   return QFont("Liberation Sans", 12, QFont::Bold, true);
		case FONT_MapInfo_Big : return QFont("Liberation Mono", 11, QFont::Bold,false);
		case FONT_MapInfo_Medium :  return QFont("Liberation Mono", 10, QFont::Bold,false);
		case FONT_MapInfo_Small :   return QFont("Liberation Mono", 10, QFont::Normal,false);
		case FONT_MapInfo_DateRef : return QFont("Liberation Mono", 9, QFont::Normal,false);
		case FONT_LonLatGridLabel : return QFont("Liberation Sans", 10, QFont::Normal, false);
		case FONT_IsolineLabel : return QFont("Liberation Sans", 8, QFont::Normal, false);
		case FONT_MapCountry :   return QFont("Liberation Sans", 12, QFont::Normal, false);
		case FONT_POILabel :  return QFont("Liberation Serif", 10, QFont::Bold, true);
		case FONT_MapCity_1 : return QFont("Liberation Sans", 11, QFont::Bold, false);
		case FONT_MapCity_2 : return QFont("Liberation Sans", 10, QFont::Bold, false);
		case FONT_MapCity_3 : return QFont("Liberation Sans", 10, QFont::Normal, false);
		case FONT_MapCity_4 : return QFont("Liberation Sans", 9, QFont::Normal, true);
		case FONT_MapCity_5 : return QFont("Liberation Sans", 8, QFont::Normal, true);
		case FONT_GRIB_Temp : return QFont("Liberation Serif", 10, QFont::Bold, true);
		case FONT_GRIB_PressHL : return QFont("Liberation Serif", 16, QFont::Bold, true);
		case FONT_IAC_Isobar :   return QFont("Liberation Serif", 9, QFont::Bold, true);
		case FONT_IAC_PressHLVal : return QFont("Liberation Serif", 10, QFont::Bold, true);
		case FONT_IAC_PressHL : return QFont("Liberation Serif", 14, QFont::Bold, true);
		case FONT_AltitudeLabel :     return QFont("Liberation Mono", 9, QFont::Normal,false);
		case FONT_AltitudeLabelTitle: return QFont("Liberation Mono", 9, QFont::Bold,false);
		case FONT_DateChooser :  return QFont("Liberation Sans", 8, QFont::Bold,true);
		case FONT_ColorScale :  return QFont("Liberation Sans", 8, QFont::Normal,false);
		default:
			return QFont("Liberation Sans",11,QFont::Normal,false);
	}
//    f.setStretch(QFont::Condensed);

}

//----------------------------------------------------------
void Font::initAppFonts ()
{
	//-----------------------------------------------
	// Application default font
	initGlobalFont (FONT_Default);
	qApp->setFont(GLOB_Font[FONT_Default]);

	//-----------------------------------------------
	// Other fonts
	initGlobalFont (FONT_ComboBox);
	initGlobalFont (FONT_StatusBar);
	initGlobalFont (FONT_MenuBar);

	initGlobalFont (FONT_MapWait);
	initGlobalFont (FONT_MapInfo_Big);
	initGlobalFont (FONT_MapInfo_Medium);
	initGlobalFont (FONT_MapInfo_Small);
	initGlobalFont (FONT_MapInfo_DateRef);
	initGlobalFont (FONT_LonLatGridLabel);
	initGlobalFont (FONT_IsolineLabel);
	initGlobalFont (FONT_MapCountry);
	initGlobalFont (FONT_POILabel);

	initGlobalFont (FONT_MapCity_1);
	initGlobalFont (FONT_MapCity_2);
	initGlobalFont (FONT_MapCity_3);
	initGlobalFont (FONT_MapCity_4);
	initGlobalFont (FONT_MapCity_5);
	
	initGlobalFont (FONT_GRIB_Temp);
	initGlobalFont (FONT_GRIB_PressHL);
	
	initGlobalFont (FONT_IAC_Isobar);
	initGlobalFont (FONT_IAC_PressHLVal);
	initGlobalFont (FONT_IAC_PressHL);
	
	initGlobalFont (FONT_AltitudeLabel);
	initGlobalFont (FONT_AltitudeLabelTitle);
	initGlobalFont (FONT_DateChooser);
	initGlobalFont (FONT_ColorScale);
}

//----------------------------------------------------------
void Font::changeGlobalFont (FontCode code, QFont font)
{
	GLOB_Font[code] = font;
	Util::setSetting( getFontCodeString(code), font);
}



//----------------------------------------------------------
bool Font::loadFontFamily(QString fname)
{
	int id = QFontDatabase::addApplicationFont (fname);
	if (id >= 0) {
		QStringList lst = QFontDatabase::applicationFontFamilies (id);
		for (int i=0; i<lst.size(); i++) {
			//printf("%s\n", qPrintable(lst.at(i)));
		}
		return true;
	}
	else
		return false;
}

//----------------------------------------------------------
void Font::loadAllFonts()
{

	QString dirname = Util::pathFonts()+"liberation-fonts/";
	int nb = 0;
	QDir dir(dirname);
	if (dir.exists()) {
		dir.setFilter(QDir::Files);
		QStringList filters;
		filters << "*.ttf";
		dir.setNameFilters(filters);
		QStringList lst=dir.entryList();
		for (int i=0; i<lst.size(); i++) {
			//printf("file: %s\n", qPrintable(lst.at(i)));
			if (loadFontFamily(dirname+lst.at(i)))
				nb++;
		}
	}

	initAppFonts ();
}




