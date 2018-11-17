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

#include <QDir>
#include <QStandardPaths>
#include <QStringList>
#include <QMessageBox>
#include <QMetaEnum>

#include "Settings.h"
#include "Util.h"

//---------------------------------------------------------------------
// Variables globales... yes I know, it's bad :)
//---------------------------------------------------------------------
QString GLOB_SettingsDir;
QString GLOB_AppDataDir;
QString GLOB_SettingsFilename;
QString GLOB_SettingsFilename_POI;

QSettings *GLOB_NatSettings;
QSettings *GLOB_IniSettings;
QSettings *GLOB_IniSettings_POI;

//---------------------------------------------------------------------
// Priority :
// 1. user app settings location (OS specific)
// 2. under application current directory (data/config/)
// 3. OS system temporary folder if all else fails
//---------------------------------------------------------------------
void Settings::initializeSettingsDir ()
{
	QString path="";
	QDir dir;
	
	if (path == "")
    {	// first option is to locate setting files in user application settings area
        // this should be OK for all systems
        dir = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
        if (! dir.exists()) {
            // create a directory in user home if it don't exists
            dir.mkpath(dir.absolutePath());
        }
        if (Util::isDirWritable(dir)) {
            path = dir.absolutePath();
        }
	}
//    DBGQS("Settings path is: "+path);
//    DBGQS("Colors path is currently (appDataDir not yet set): "+Util::pathColors());

    if (path == "")
    {	// second option is to locate setting files under application current directory
        dir = QDir::current();
        if ( dir.exists(Util::pathGshhs())
                    && dir.exists(Util::pathGis()) )
        {   // seem's zygrib directory, try yo write a file
            if (Util::isDirWritable(dir))
            {
                QString path2 = dir.absolutePath()+"/"+Util::pathConfig();
                QDir dir2 = QDir (path2);
                if (! dir2.exists()) {
                    // try to create the directory
                    dir2.mkpath(dir2.absolutePath());
                }
                if (Util::isDirWritable(dir2)) {
                    path = path2;
                }
            }
        }
    }

    if (path == "")
    {	// third option is to use temp directory
		dir = QDir::temp();
		if (Util::isDirWritable(dir)) {
			path = dir.absolutePath();
		}
	}

	if (path != "") {
		GLOB_SettingsDir = path;
        GLOB_SettingsFilename	 	= GLOB_SettingsDir + "/xygrib.ini";
        GLOB_SettingsFilename_POI	= GLOB_SettingsDir + "/xygrib_poi.ini";

		// A. Degwerth [Cassidian] added to make sure that the user dir contains an updated .ini file

        // TODO remove as there is no default ini file. David
        // TODO methods can also be removed
//        Settings::checkAndCopyDefaultIni(GLOB_SettingsFilename, QDir::current().absolutePath() + "/" + Util::pathConfig() + "/xygrib.ini");
//        Settings::checkAndCopyDefaultIni(GLOB_SettingsFilename_POI, QDir::current().absolutePath() + "/" + Util::pathConfig() + "/xygrib_poi.ini");

        GLOB_IniSettings     = new QSettings (GLOB_SettingsFilename, QSettings::IniFormat);
        GLOB_IniSettings_POI = new QSettings (GLOB_SettingsFilename_POI, QSettings::IniFormat);
    }
	else {
		GLOB_SettingsDir = "";
		GLOB_SettingsFilename	  = "";
		GLOB_SettingsFilename_POI = "";
        GLOB_IniSettings     = nullptr;
        GLOB_IniSettings_POI = nullptr;
	}
    GLOB_NatSettings = new QSettings ("xyGrib");
			
}

//---------------------------------------------------------------------
void  Settings::setApplicationNativeSettings (const QString &group, const QHash <QString, QVariant> &h)
{
	if (h.empty())
		return;

	if (GLOB_NatSettings == nullptr)
		return;

	GLOB_NatSettings->beginGroup(group);
	QHash<QString, QVariant>::const_iterator i = h.constBegin();
	while (i != h.constEnd()) {
		GLOB_NatSettings->setValue(i.key(), i.value());
		++i;
	}
	GLOB_NatSettings->endGroup();
	GLOB_NatSettings->sync();
}

//---------------------------------------------------------------------
void Settings::setApplicationNativeSetting
			(const QString &group, const QString &key, const QVariant &value, bool sync)
{
	if (GLOB_NatSettings == nullptr)
		return;

	GLOB_NatSettings->beginGroup(group);
	GLOB_NatSettings->setValue(key, value);
	GLOB_NatSettings->endGroup();
	if (sync)
		GLOB_NatSettings->sync();
}
//---------------------------------------------------------------------
QVariant Settings::getApplicationNativeSetting
            (const QString &group, const QString &key, const QVariant &defaultValue)
{
    QVariant val;
    if (GLOB_NatSettings != nullptr)
    {
        GLOB_NatSettings->beginGroup (group);
        val = GLOB_NatSettings->value (key, defaultValue);
        GLOB_NatSettings->endGroup();
        GLOB_NatSettings->sync();
    }
    return val;
}


//---------------------------------------------------------------------
void  Settings::setUserSettings (const QHash <QString, QVariant> &h)
{
	if (h.empty())
		return;

	Settings::setApplicationNativeSettings("main", h);

	if (GLOB_IniSettings == nullptr)
		return;

	GLOB_IniSettings->beginGroup("main");
	QHash<QString, QVariant>::const_iterator i = h.constBegin();
	while (i != h.constEnd()) {
		GLOB_IniSettings->setValue(i.key(), i.value());
		++i;
	}
	GLOB_IniSettings->endGroup();
	GLOB_IniSettings->sync();
}

//---------------------------------------------------------------------
void Settings::setUserSetting (const QString &key, const QVariant &value, bool sync)
{
	// save 2 times the settings
	Settings::setApplicationNativeSetting("main", key, value, sync);

	if (GLOB_IniSettings == nullptr)
		return;

	GLOB_IniSettings->beginGroup("main");
	GLOB_IniSettings->setValue(key, value);
	GLOB_IniSettings->endGroup();
	if (sync)
		GLOB_IniSettings->sync();
}
//---------------------------------------------------------------------
QVariant Settings::getUserSetting (const QString &key, const QVariant &defaultValue)
{
	QVariant val;
    if (GLOB_IniSettings != nullptr)
	{
		GLOB_IniSettings->beginGroup("main");
		val = GLOB_IniSettings->value(key, defaultValue);
		GLOB_IniSettings->endGroup();
		GLOB_IniSettings->sync();
	}
	// .ini file corrupted ? Try to read native settings.
	if (! val.isValid()) {
		val = getApplicationNativeSetting("main", key, defaultValue);
	}

	return val.isValid() ?  val  : defaultValue;
}

//---------------------------------------------------------------------
QStringList Settings::getAllKeys()
{
	QStringList list;
    if (GLOB_IniSettings != nullptr)
	{
		GLOB_IniSettings->beginGroup ("main");
		list = GLOB_IniSettings->allKeys();
		GLOB_IniSettings->endGroup();
	}
    else if (GLOB_NatSettings != nullptr)
	{
		GLOB_NatSettings->beginGroup ("main");
		list = GLOB_NatSettings->allKeys();
		GLOB_NatSettings->endGroup();
	}
	return list;
}
//---------------------------------------------------------------------
void Settings::removeUserSetting (const QString &key)
{
    if (GLOB_IniSettings != nullptr)
	{
		GLOB_IniSettings->beginGroup ("main");
		GLOB_IniSettings->remove(key);
		GLOB_IniSettings->endGroup();
	}
    else if (GLOB_NatSettings != nullptr)
	{
		GLOB_NatSettings->beginGroup ("main");
		GLOB_NatSettings->remove(key);
		GLOB_NatSettings->endGroup();
	}
}

//======================================================================
// POI's
//======================================================================
QVariant Settings::getSettingPOI
				( uint code, const QString &key, const QVariant &defaultValue,
				  bool fromOldNativeSettings
				 )
{
	QString poikey = QString::number(code)+"/"+key;
	QVariant val;
	if ( ! fromOldNativeSettings
        &&  GLOB_IniSettings_POI != nullptr)
	{
		GLOB_IniSettings_POI->beginGroup("poi");
		val = GLOB_IniSettings_POI->value(poikey, defaultValue);
		GLOB_IniSettings_POI->endGroup();
		GLOB_IniSettings_POI->sync();
	}
	// .ini file corrupted ? Try to read native settings.
	if (  fromOldNativeSettings
		 ||  ! val.isValid()) {
		val = getApplicationNativeSetting("poi", poikey, defaultValue);
	}
	return val;
}
//---------------------------------------------------------------------
void Settings::setSettingPOI
				(uint code, const QString &key, const QVariant &value, bool sync)
{
	QString poikey = QString::number(code)+"/"+key;
	
	// save 2 times the settings : native and in ini file
	Settings::setApplicationNativeSetting ("poi", poikey, value, sync);
	
	if (GLOB_IniSettings_POI == nullptr)
		return;

	GLOB_IniSettings_POI->beginGroup("poi");
	GLOB_IniSettings_POI->setValue  (poikey, value);
	GLOB_IniSettings_POI->endGroup();
	if (sync)
		GLOB_IniSettings_POI->sync();
}
//---------------------------------------------------------------------
QList<uint> Settings::getSettingAllCodesPOIs()
{
	QList<uint> reslist;
	if (GLOB_SettingsDir != "")
	{
		QSettings settings(GLOB_SettingsFilename_POI, QSettings::IniFormat);
		settings.beginGroup("poi");
		QString poicode;
		QStringList slist = settings.childGroups();
		for (int i=0; i<slist.size(); ++i) {
			poicode = slist.at(i);
			bool ok;
			uint code = poicode.toUInt(&ok);
			if (ok)
				reslist.append (code);
		}
		settings.endGroup();
	}
	else
	{	// try to load from native settings
        QSettings settings("xyGrib");
		settings.beginGroup("poi");
		QString poicode;
		QStringList slist = settings.childGroups();
		for (int i=0; i<slist.size(); ++i) {
			poicode = slist.at(i);
			bool ok;
			uint code = poicode.toUInt(&ok);
			if (ok)
				reslist.append (code);
		}
		settings.endGroup();
	}
	return reslist;
}
//---------------------------------------------------------------------
void Settings::deleteSettingsPOI(uint code)
{
    QSettings natSettings("xyGrib");
	natSettings.beginGroup("poi");
	QStringList allgroups = natSettings.childGroups(); // pois are in groups
	QStringListIterator it(allgroups);
	while (it.hasNext()) {
		bool ok;
		QString gr = it.next();
		uint codepoi = gr.toUInt(&ok);
		if (ok && codepoi==code) {
			natSettings.remove (gr);
		}
	}
	if (GLOB_SettingsDir != "")
	{
		QSettings settings(GLOB_SettingsFilename_POI, QSettings::IniFormat);
		settings.beginGroup("poi");
		QStringList allgroups = settings.childGroups(); // pois are in groups
		QStringListIterator it(allgroups);
		while (it.hasNext()) {
			bool ok;
			QString gr = it.next();
			uint codepoi = gr.toUInt(&ok);
			if (ok && codepoi==code) {
				settings.remove (gr);
			}
		}
	}
}
    
//---------------------------------------------------------------------
uint Settings::getNewCodePOI()
{
    QSettings settings("xyGrib");
	settings.beginGroup("poi");
	uint v, max = 0;
	QStringList slist = settings.childGroups();
 	for (int i=0; i<slist.size(); ++i)
 	{
		bool ok;
  		const QString& poicode = slist.at(i);
  		v = poicode.toUInt (&ok);
  		if (ok  && v > max)
  			max = v;
	}
	settings.endGroup();
	return max+1;
}
    
//============================================================================
void Settings::initializeGribFilesDir ()
{
	QString path="";
	QDir dir;

	// Test if path found in settings is writable
	path = getUserSetting("gribFilePath", "").toString();
	if (path != "") {
		dir = QDir(path);
		if (Util::isDirWritable(dir)) {
			return;			// ok, nothing to do
		}
        path = "";
	}
	if (path == "")
    {	// try xygrib_directory/grib
		dir = QDir::current();
		if ( dir.exists("maps") && dir.exists("img") )
        {   // seem's xygrib directory, try yo write a file
			if (Util::isDirWritable(dir))
			{
				QString path2 = dir.absolutePath()+"/grib";
				QDir dir2 = QDir(path2);
				if (! dir2.exists()) {
					// try to create the directory
					dir2.mkpath(dir2.absolutePath());
				}
				if (Util::isDirWritable(dir2)) {
					path = path2;
				}
			}
		}
	}
	
	if (path == "")
	{	// try user directory
		#ifdef Q_OS_WIN32
            dir = QDir( QDir::homePath()+"/xygrib/grib" );
		#else
            dir = QDir( QDir::homePath()+"/.xygrib/grib" );
		#endif
		if (! dir.exists()) {
			// create a directory in user home if it don't exists
			dir.mkpath(dir.absolutePath());
		}
		if (Util::isDirWritable(dir)) {
			path = dir.absolutePath();
		}
	}
	
	if (path == "")
	{	// try temp directory
		dir = QDir::temp();
		if (dir.exists() && Util::isDirWritable(dir)) {
			path = dir.absolutePath();
		}
	}
	
	// save path (even if path=="")
	setUserSetting("gribFilePath",  path);
}

// check if the file strIniUser exists and if its newer than the file strIniDefault
// if the file doesn't exist or is older than strIniDefault then the strIniUser is
// replaced by strIniDefault
// A. Degwerth [Cassidian]
//void Settings::checkAndCopyDefaultIni(const QString& strIniUser, const QString& strIniDefault)
//{
//    // check if we have a default xyGrib.ini file
//    if (QFile::exists(strIniDefault))
//    {
//        // yes the file exists

//        bool fCopyDefaultIni = false;

//        // check if there exists a xyGrib.ini in the user profile
//        if (!QFile::exists(strIniUser))
//        {
//            fCopyDefaultIni = true;
//        }
//        else {
//            // file exist in the user profile
//            // check if it is older than the default
//            QFileInfo fiIniUser(strIniUser);
//            QFileInfo fiIniDefault(strIniDefault);
//            if (fiIniUser.lastModified() < fiIniDefault.lastModified())
//            {
//                // the user ini file is older
//                // -> replace it
//                fCopyDefaultIni = true;
//            }
//        }

//        if (fCopyDefaultIni == true)
//        {
//            QFile::remove(strIniUser);
//            QFile::copy(strIniDefault, strIniUser);
//            // set the write permission to the copied file
//            QFile::setPermissions(strIniUser, QFile::WriteOwner | QFile::WriteGroup);
//        }
//    }
//}

//---------------------------------------------------------------------
// Priority for appDataLocation:
// 1. check ini file setting for appDataDir
// 2. user app data location (OS specific) WRITABLE
// 3. shared area i.e. /usr/shared....  or c:/ProgramData/
// 4. under the application current directory
//---------------------------------------------------------------------
bool Settings::findAppDataDir ()
{
    QString appDataPathSetting;
    QString path="";
    QDir dir;
    QStringList slist;
    bool retval = true;

    // ----------------------------------------------------
    // before all let's check for valid appDataDir ini setting
    // ----------------------------------------------------
    appDataPathSetting = Settings::getUserSetting("appDataDir", "").toString();
//    DBGQS ("Ini setting is: " + appDataPathSetting);
    if (appDataPathSetting != ""){
        // set dir to ini settings value
        dir = QDir(appDataPathSetting);
        QDir maps = QDir(dir.absolutePath() + "/data/maps");
        QDir gis = QDir(dir.absolutePath() + "/data/gis");

        if (maps.exists() && gis.exists()) { // we have the location
            path = dir.absolutePath();
//            DBGQS("Ini setting was good and is: " + path);
        }
    }


    // ----------------------------------------------------
    // no path to appDataDir - let's look for it
    // ----------------------------------------------------
    if (path == "")
    {	// first search option is to locate setting files in user application data area
        // this should be OK for all systems
#ifdef Q_OS_WIN
        // "AppDataLocation" is new and does not appear in earlier QT5 versions and may crash linux builds
        // in windows binaries it does exist for sure. This will put app data in c:/user/AppData/Roaming...
        dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
#else
        dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#endif
        QDir maps = QDir(dir.absolutePath() + "/data/maps");
        QDir gis = QDir(dir.absolutePath() + "/data/gis");

        DBGQS("Searching in: " + dir.absolutePath());
        if (maps.exists() && gis.exists()) { // we have the location
            path = dir.absolutePath();
            Settings::setUserSetting("appDataDir", path); // store the location in settings
            DBGQS("Writable location search was good and is: " + path);
        }
    }

    if (path == "")
    {	// second option is to locate app data files in shared area

        slist = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
        foreach (QString str, slist)
        {
            DBGQS("Searching in: " + str);

            dir = QDir(str);
            QDir maps = QDir(dir.absolutePath() + "/data/maps");
            QDir gis = QDir(dir.absolutePath() + "/data/gis");

            if (maps.exists() && gis.exists()) { // we have the location
                path = dir.absolutePath();
                Settings::setUserSetting("appDataDir", path); // store the location in settings
                DBGQS("Shared location search was good and is: " + path);
                break;
            }
        }
    }

    if (path == "")
    {	// third option is to look under application current directory
        dir = QDir::current();
        DBGQS("Searching in current dir: " + dir.absolutePath());
        QDir maps = QDir(dir.absolutePath() + "/data/maps");
        QDir gis = QDir(dir.absolutePath() + "/data/gis");

        if (maps.exists() && gis.exists()) { // we have the location
            path = dir.absolutePath();
            Settings::setUserSetting("appDataDir", path); // store the location in settings
            DBGQS("Current dir search was good and is: " + path);
         }

    }

    if (path != "") {
        GLOB_AppDataDir = path;
    }
    else {
        // error message
        retval = false;
    }
//    DBGQS("Final AppDataPath is: "+path);
//    DBGQS("For sanity: Colors path is: "+Util::pathColors());
    return retval;
 }

