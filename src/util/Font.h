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

#ifndef FONT_H
#define FONT_H

#include <QFont>
#include <QFontInfo>

#include "Util.h"

enum FontCode
{
	FONT_Default,
	FONT_ComboBox,
	FONT_StatusBar,
	FONT_MenuBar,

	FONT_MapWait,
	FONT_MapInfo_Big,
	FONT_MapInfo_Medium,
	FONT_MapInfo_Small,
	FONT_MapInfo_DateRef,
	FONT_LonLatGridLabel,
	FONT_IsolineLabel,
	FONT_POILabel,

	FONT_MapCountry,
	FONT_MapCity_1,
	FONT_MapCity_2,
	FONT_MapCity_3,
	FONT_MapCity_4,
	FONT_MapCity_5,
	
	FONT_GRIB_Temp,
	FONT_GRIB_PressHL,
	
	FONT_IAC_Isobar,
	FONT_IAC_PressHLVal,
	FONT_IAC_PressHL,
	
	FONT_AltitudeLabel,
	FONT_AltitudeLabelTitle,
	
	FONT_DateChooser,
	FONT_ColorScale,
	
	FONT_MaxValue // Don't move nor remove, it's the size of fonts table FIXME: use a list
};

extern QFont GLOB_Font [FONT_MaxValue];


class Font {

    public:
        static QFont getFont (int code) {
            return GLOB_Font [code];
        }

        static void loadAllFonts ();
		static void changeGlobalFont (FontCode code, QFont font);

		static QString getFontCodeString (FontCode code);
		static QFont   getDefaultFont (FontCode code);
		
		static QFont getSettingsFont (FontCode code) {
			QString codename = getFontCodeString (code);
			return Util::getSetting(codename, getDefaultFont(code)).value<QFont>();
		}
		
		static void initGlobalFont (FontCode code) {
			GLOB_Font[code] = getSettingsFont (code);
		}
		
	private:
		static bool loadFontFamily (QString fname);
		static void initAppFonts ();
	
};



#endif
