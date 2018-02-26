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

#ifndef GRIB2RECORD_H
#define GRIB2RECORD_H

#include "g2clib/grib2.h"
#include "GribRecord.h"

//----------------------------------------
class Grib2RecordMarker {
	public :
		Grib2RecordMarker(int id, int pdtnum,int paramcat,int paramnumber,int levelType,int levelValue)
		{
			this->id =id;
			this->pdtnum =pdtnum;
			this->paramcat =paramcat;
			this->paramnumber =paramnumber;
			this->levelType =levelType;
			this->levelValue =levelValue;
		}
		Grib2RecordMarker(const Grib2RecordMarker &a)
		{
			this->id =a.id;
			this->pdtnum =a.pdtnum;
			this->paramcat =a.paramcat;
			this->paramnumber =a.paramnumber;
			this->levelType =a.levelType;
			this->levelValue =a.levelValue;
		}
		bool operator== (const Grib2RecordMarker &a)   // same product, ignore altitude
		{
			return 	this->id==a.id && this->pdtnum ==a.pdtnum 
				&& this->paramcat ==a.paramcat && this->paramnumber ==a.paramnumber;
		}
		void dbgRec() {
			return;   // TODO debug
			if (pdtnum >= 0)
				fprintf(stderr, "unknown data: id=%d pdtnum=%d paramcat=%d paramnumber=%d alt=%s\n", 
					id, pdtnum, paramcat, paramnumber, 
					qPrintable(AltitudeStr::toStringShort(Altitude(levelType,levelValue)))
				);
		}
	private:
		int id, pdtnum, paramcat, paramnumber,levelType,levelValue;
};
//----------------------------------------
class Grib2Record : public GribRecord
{
	public:
		Grib2Record ();
		Grib2Record (gribfield  *gfld, int id, int idCenter, time_t refDate);
		~Grib2Record ();
		
		Grib2RecordMarker getGrib2RecordMarker ()
			{ return Grib2RecordMarker(id, pdtnum, paramcat, paramnumber,levelType,levelValue); }

        virtual void  print (const char *title);
	
	private:
		//gribfield  *gfld;
		void analyseProductDefinitionTemplate (gribfield  *gfld);
		int  analyseProductType ();
		void readAltitude (gribfield  *gfld);

		int pdtnum;      // = Product Definition Template Number(see Code Table 4.0)
		int paramcat;    // Parameter category (see Code table 4.1)
		int paramnumber; //Parameter number (see Code table 4.2)

};



#endif
