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

/*************************
Lecture mise en mémoire d'un fichier GRIB

*************************/

#ifndef GRIB2READER_H
#define GRIB2READER_H

#include "RegularGridded.h"
#include "GribReader.h"
#include "Grib2Record.h"
#include "zuFile.h"
#include "g2clib/grib2.h"

//===============================================================
class Grib2Reader : public GribReader
{
    public:
        Grib2Reader ();
        ~Grib2Reader ();
		
        virtual void  openFile (const std::string fname,
						LongTaskProgress *taskProgress, int nbrecs);
		
	private:
        void openFilePriv (const std::string fname, int nbrecs);
		void readGrib2FileContent (int nbrecs);

		void analyseRecords ();
		QList<Grib2RecordMarker> allUnknownRecords;
		
		void seekgb_zu (ZUFILE *lugb,g2int iseek,g2int mseek,g2int *lskip,g2int *lgrib);

};


#endif
