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

#ifndef DATAQSTRING_H
#define DATAQSTRING_H

//----------------------------------------------------------------
// String conversion.
// Need QT for the QString type and "tr" translation tools
//----------------------------------------------------------------

#include <QApplication> 
#include <QStringList> 

#include "Util.h"
#include "DataDefines.h"

class AltitudeStr : public QObject
{ Q_OBJECT 
	public:
		static QString toString (const Altitude &alt);
		static QString toStringShort (const Altitude &alt);
		
		static QString  serialize   (const Altitude &alt);
		static Altitude unserialize (const QString &stringCode);
};

class DataCodeStr : public QObject
{ Q_OBJECT 
	public:
		static QString toString (const DataCode &dtc);
		static QString toString_name  (int dataType);
		static QString toString_name  (const DataCode &dtc);
		static QString toString_level (const DataCode &dtc);
		static QString toString_levelShort (const DataCode &dtc);
		
		static QString  serialize   (const DataCode &dtc);
		static DataCode unserialize (const QString &stringCode);
		
		static QString toString (const DataCenterModel &dcm);
};

#endif
