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

/****************************************************************
Decode IAC file from NOAA (fleetcodes)

Analyse:
http://weather.noaa.gov/pub/data/raw/as/asxx21.egrr..txt
Forecast 24h:
http://weather.noaa.gov/pub/data/raw/fs/fsxx21.egrr..txt

****************************************************************/

#ifndef IACREADER_H
#define IACREADER_H

#include <iostream>
#include <cmath>
#include <vector>
#include <locale.h>

#include "DataMeteoAbstract.h"
#include "Util.h"
#include "zuFile.h"


//--------------------------------------------------------
class IAC_Point
{
	public:
		IAC_Point(double lat, double lon)
				{ y=lat; x=lon; }
		double 	x, y;
};

//--------------------------------------------------------
class IAC_Line
{
	public:
		virtual ~IAC_Line()
			{
				Util::cleanVectorPointers(points);
			}
		
		void addPoint(IAC_Point *pt)
					{ points.push_back(pt); }
		
		double	distanceFromPoint(double x, double y);
		
		std::vector <IAC_Point *> points;
};

//--------------------------------------------------------
class IAC_Front: public IAC_Line
{
	public:
		enum IacFrontType {
			IAC_FRONT_STATIONARY,
			IAC_FRONT_COLD,
			IAC_FRONT_WARM,
			IAC_FRONT_OCCLUSION,
			IAC_FRONT_INSTABILITY_LINE,
			IAC_FRONT_INTERTROPICAL,
			IAC_FRONT_CONVERGENCE_LINE,
			IAC_FRONT_UNKNOWN
		};
		
		IAC_Front(std::string t);
		
		IacFrontType type;
		std::string  code;	// 6_6_Ft_Fi_Fc
		
};

//--------------------------------------------------------
class IAC_Isobar: public IAC_Line
{
	public:
		IAC_Isobar(int val)
				{ value = val; }
		int value;
};

//--------------------------------------------------------
class IAC_TroughLine : public IAC_Line
{
	// not different, just a new line type
};


//--------------------------------------------------------
class IAC_HighLowPressurePoint : public IAC_Point
{
	public:
		IAC_HighLowPressurePoint(std::string t, double lat, double lon, int v)
			: IAC_Point(lat, lon)
			{ 	type=t;
				value=v;
				nearestisobar=-1;
				completevalue=-1;
			}
				
		std::string type;		// H or L (don't change it)
		int    		value;		// pressure on 2 digits (00 -> 99)
		int nearestisobar;
		int completevalue;
};


//===============================================================
class IacReader : public DataReaderAbstract
{
    public:
		enum PositionMode {			// yyyyy posiion group mode
				POS_LaLaLoLok_NORTH,
				POS_LaLaLoLok_SOUTH,
				POS_LaLaLoLok_EQUAT,
				POS_iiiD1s1,
				POS_QLaLaLoLo
		};
		enum IacFileType {
			IAC_ANALYSE,
			IAC_FORECAST,
			IAC_UNKNOWN
		};
        
        IacReader(const std::string fname);
        ~IacReader();
		
		virtual FileDataType getReaderFileDataType () 
					{return DATATYPE_IAC;};

		void   display_text();	// for debug
	
		bool   		isOk()     		   {return ok;}
		IacFileType getIacFileType()   {return iacFileType;}
		bool		is_NOAA_IAC_File() {return is_NOAA_File;}
		
		bool getZoneExtension(double *x0,double *y0, double *x1,double *y1);
		
		std::vector <IAC_HighLowPressurePoint *> *getList_HighLowPressurePoints()
					{ return & list_HighLowPressurePoints; }
		std::vector <IAC_TroughLine *>	 *getList_PressureTroughLines()
					{ return & list_TroughLines; }
		std::vector <IAC_Isobar *>	 *getList_PressureIsobars()
					{ return & list_Isobars; }
		std::vector <IAC_Front *>	 *getList_Fronts()
					{ return & list_Fronts; }
				 
		int		getIacFileDay()   {return iacFileDay;}
		int		getIacFileHour()   {return iacFileHour;}
		int		getIacFileMinute()  {return iacFileMinute;}
		int		getIacFileValidHour()   {return iacFileValidHour;}

    private:
		bool 		 ok;
		std::string  fileName;
		ZUFILE 		 *file;
		double 		 xmin,xmax, ymax,ymin;
		bool 		 endOfFile;
		
		IacFileType iacFileType;
		bool		is_NOAA_File;
		int		iacFileDay;
		int		iacFileHour;
		int		iacFileMinute;
		int		iacFileValidHour;
		
		PositionMode 	  yyyyyPositionMode;
		int 			  currentSECTION;
		IAC_TroughLine 	* currentTroughLine;
		IAC_Isobar 		* currentIsobar;
		IAC_Front 		* currentFront;

		std::vector <IAC_HighLowPressurePoint *> list_HighLowPressurePoints;
		std::vector <IAC_TroughLine *>	 list_TroughLines;
		std::vector <IAC_Isobar *>		 list_Isobars;
		std::vector <IAC_Front *>		 list_Fronts;
		
		void cleanAllVectors()  {
				Util::cleanVectorPointers(list_HighLowPressurePoints);
				Util::cleanVectorPointers(list_TroughLines);
				Util::cleanVectorPointers(list_Isobars);
				Util::cleanVectorPointers(list_Fronts);
			}
		
		bool readPosition(std::string word, double *lat, double *lon);
				
		std::vector<std::string> splitLine(std::string &line);
		std::vector<std::string> readAndSplitLine();
		std::string readALine();
		bool 		is_alnum(char c);
		bool 		is_fleetChar(char c);
		
		void  decodeDataLine           (std::vector<std::string> &vline);
		
		void  decodeDataLine_header_NOAA   (std::vector<std::string> &vline);
		void  decodeDataLine_preamble      (std::vector<std::string> &vline);
		void  decodeDataLine_sec_0         (std::vector<std::string> &vline);
		void  decodeDataLine_sec_1_Fronts  (std::vector<std::string> &vline);
		void  decodeDataLine_sec_2_Isobars (std::vector<std::string> &vline);
		
		void   	decodeLine(std::vector<std::string> &vline);
    	
    	void    readIacFileContent();
		void 	openFile(const std::string fname);
		void    display_vline(std::vector<std::string> &vline);	// for debug

};


#endif
