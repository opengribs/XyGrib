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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <cassert>

#include <QApplication>
#include <QDir>
#include <QObject>
#include <QString>
#include <QSettings>

#include "POI.h"

extern QString GLOB_SettingsDir;
extern QString GLOB_SettingsFilename;
extern QString GLOB_SettingsFilename_POI;

extern QSettings *GLOB_NatSettings;
extern QSettings *GLOB_IniSettings;
extern QSettings *GLOB_IniSettings_POI;

class Settings
{
public:
	static void     initializeSettingsDir ();	// Don't forget to call it
	static void     initializeGribFilesDir ();	// Don't forget to call it
	
	//--------------------------------
	// Globals settings
	//--------------------------------
	static QString  getSettingsDir ()
						{ return GLOB_SettingsDir; }
	static QString  getSettingsFilename ()
						{ return GLOB_SettingsFilename; }
	static QString  getSettingsFilename_POI ()
						{ return GLOB_SettingsFilename_POI; }

    static void     setUserSetting (const QString &key, const QVariant &value);
    static QVariant getUserSetting (const QString &key, const QVariant &defaultValue);
    
    static void     setApplicationNativeSetting
						(const QString &group, const QString &key, const QVariant &value);
    static QVariant getApplicationNativeSetting
						(const QString &group, const QString &key, const QVariant &defaultValue);

	static QStringList getAllKeys();
    static void     removeUserSetting (const QString &key);

	//--------------------------------
	// POI's
	//--------------------------------
    static void  setSettingPOI
    					( uint code, const QString &key, const QVariant &value);    
    static QVariant getSettingPOI
						( uint code, const QString &key, const QVariant &defaultValue,
						  bool fromOldNativeSettings);
    
	static uint        getNewCodePOI();
    static QList<uint> getSettingAllCodesPOIs();
    
    static void        deleteSettingsPOI(uint code);
	


private:
	static void     copyOldNativeSettingsToIniFile ();
	static void     copyOldNativeSettingsToIniFile_POI ();
	// A. Degwerth [Cassidian] added
        static void     checkAndCopyDefaultIni(const QString& strIni, const QString& strIniDefault);
};



#endif
