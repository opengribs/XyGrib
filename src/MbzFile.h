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

#ifndef MBZFILE_H
#define MBZFILE_H

#include "Util.h"
#include "zuFile.h"
#include "LongTaskProgress.h"

//------------------------------------------------------
class MButil
{
	public:
		static bool readInt8   (ZUFILE *f, int *val);
		static bool readInt16  (ZUFILE *f, int *val);
		static bool readInt32  (ZUFILE *f, int *val);
		static bool readFloat32  (ZUFILE *f, float *val);
		
		static bool getDateFromName (const char *fname, 
						  int*year, int*month, int*day, int*href, int*hour);
		static bool substring2int (int *val, const char *str,int start,int size);
		static bool readPosition  (char *line, float *x, float *y);
};

//------------------------------------------------------
class MbzLine
{
	public:
		float x, y;
		int   hour;
		std::vector <float> data;
		void print() const;
};

//---------------------------------------------------
// MBZfile : read a file in MBZ format
//---------------------------------------------------
class MbzFile
{
	public:
		MbzFile ()   
			{ ok = false; }
			
		MbzFile (const char *fname, LongTaskProgress *taskProgress);
		~MbzFile ();
		
		void read_MbzFile  (const char *fname, LongTaskProgress *taskProgress);

		int getDataCodeIndex (uint32_t code);
		int getDataCodeIndex (DataCode dtc)  {return getDataCodeIndex(dtc.toInt32());}
		
		bool isOk () const {return ok;}
		void debugmbz () const;
		
		int   year,month,day,href; 
		float xmin,xmax, ymin,ymax;
		
		std::vector <MbzLine*>  vlines;
		std::vector <uint32_t>  vcodes;		// DataCode
		
	private:
		bool  ok;
		int   version;
		int   nbData, nbLines;
		
		void read_header (ZUFILE *f);
		void read_data_codes  (ZUFILE *f);
		void read_data_lines  (ZUFILE *f, LongTaskProgress *taskProgress);
};


#endif
