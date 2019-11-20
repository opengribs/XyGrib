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

/*************************
Lecture mise en mémoire d'un fichier GRIB

*************************/

#ifndef GRIB2READER_H
#define GRIB2READER_H

#include "GribReader.h"
#include "Grib2Record.h"

//===============================================================
class Grib2Reader : public GribReader
{
    public:
        Grib2Reader ();
        ~Grib2Reader ();
		
    protected:
		bool readGribRecord(int id) override;

	private:

		QList<Grib2RecordMarker> allUnknownRecords;
};

#endif
