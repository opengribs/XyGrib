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

#ifndef DATADEFINES_H
#define DATADEFINES_H
#include <stdint.h>

#define GRIB_NOTDEF -99999

enum DataCenterModel {
	NOAA_GFS,
	NOAA_NCEP_WW3,
	NOAA_NCEP_SST,
	METEOBLUE_NMM4,
	FNMOC_WW3_GLB,
	FNMOC_WW3_MED,
	FNMOC_WW3_EQAM,
	NORWAY_METNO,
	COAMPS_EUR,
	COAMPS_W_ATL,
	COAMPS_E_PAC,
	COAMPS_C_AM,
	NOGAPS,
	SKIRON,
	OTHER_DATA_CENTER
};
//--------------------------------------------------------
// dataTypes      cf. GribRecord::translateDataType()
//--------------------------------------------------------
#define GRB_PRESSURE        1   /* Pa     */
#define GRB_PRESSURE_MSL    2   /* Pa     */
#define GRB_GEOPOT_HGT      7   /* gpm    */
#define GRB_TEMP           11   /* K      */
#define GRB_TEMP_POT       13   /* K      */
#define GRB_TMAX           15   /* K      */
#define GRB_TMIN           16   /* K      */
#define GRB_DEWPOINT       17   /* K      */
#define GRB_WIND_DIR       31   /* °      */
#define GRB_WIND_SPEED     32   /* m/s    */
#define GRB_WIND_VX        33   /* m/s    */
#define GRB_WIND_VY        34   /* m/s    */
#define GRB_CUR_VX         49   /* m/s    */
#define GRB_CUR_VY         50   /* m/s    */
#define GRB_HUMID_SPEC     51   /* kg/kg  */
#define GRB_HUMID_REL      52   /* %      */
#define GRB_PRECIP_RATE    59   /* l/m2/s */
#define GRB_PRECIP_TOT     61   /* l/m2   */
#define GRB_SNOW_DEPTH     66   /* m      */
#define GRB_CLOUD_TOT      71   /* %      */
#define GRB_CLOUD_LOW      72   /* %      */
#define GRB_CLOUD_MID      73   /* %      */
#define GRB_CLOUD_HIG      74   /* %      */
#define GRB_FRZRAIN_CATEG 141   /* 1=yes 0=no */
#define GRB_SNOW_CATEG    143   /* 1=yes 0=no */
#define GRB_CIN 		  156   /* J/kg   */
#define GRB_CAPE 		  157   /* J/kg   */
#define GRB_WIND_GUST 	  180   /* m/s */
#define GRB_WIND_GUST_VX  181   /* m/s */
#define GRB_WIND_GUST_VY  182   /* m/s */
//----------------------------------------------------
// Waves
//----------------------------------------------------
#define GRB_WAV_SIG_HT    100   /* m */
#define GRB_WAV_WND_DIR   101   /* deg */
#define GRB_WAV_WND_HT    102   /* m */
#define GRB_WAV_WND_PER   103   /* s */
#define GRB_WAV_SWL_DIR   104   /* deg */
#define GRB_WAV_SWL_HT    105   /* m */
#define GRB_WAV_SWL_PER   106   /* s */
#define GRB_WAV_PRIM_DIR  107   /* deg */
#define GRB_WAV_PRIM_PER  108   /* s */
#define GRB_WAV_SCDY_DIR  109   /* deg */
#define GRB_WAV_SCDY_PER  110   /* s */
#define GRB_WAV_WHITCAP_PROB  155   /* % */
#define GRB_WAV_MAX_DIR   207   /* deg */
#define GRB_WAV_MAX_PER   208   /* s */
#define GRB_WAV_MAX_HT    220   /* m */

#define GRB_PRV_WAV_SIG    242   /* private: all parameter */
#define GRB_PRV_WAV_MAX    243   /* private: all parameter */
#define GRB_PRV_WAV_SWL    244   /* private: all parameter */
#define GRB_PRV_WAV_WND    245   /* private: all parameter */
#define GRB_PRV_WAV_PRIM   246   /* private: all parameter */
#define GRB_PRV_WAV_SCDY   247   /* private: all parameter */

//----------------------------------------------------
#define GRB_PRV_WIND_JET      248   /* private: wind jet stream */
#define GRB_PRV_WIND_DIR      249   /* private: wind direction in degrees */
#define GRB_PRV_WIND_XY2D     250   /* private: GRB_WIND_VX+GRB_WIND_VX */
#define GRB_PRV_DIFF_TEMPDEW  251   /* private: GRB_TEMP-GRB_DEWPOINT */
#define GRB_PRV_THETA_E   	  252   /* K   */
#define GRB_PRV_CUR_DIR       253   /* private: current direction in degrees */
#define GRB_PRV_CUR_XY2D      254   /* private: GRB_CUR_VX+GRB_CUR_VX */

#define GRB_TYPE_NOT_DEFINED 0   /* private */

//--------------------------------------------------------
// Levels types (altitude reference)
//--------------------------------------------------------
#define LV_GND_SURF    1
#define LV_ISOTHERM0   4
#define LV_ISOBARIC  100    // hPa
#define LV_MSL       102
#define LV_ABOV_MSL  103    // m
#define LV_ABOV_GND  105    // m
#define LV_SIGMA     107
#define LV_ATMOS_ENT  10
#define LV_ATMOS_ALL 200

#define LV_CLOUD_LOW_BOTTOM 212
#define LV_CLOUD_MID_BOTTOM 222
#define LV_CLOUD_HIG_BOTTOM 232
#define LV_CLOUD_LOW_TOP    213
#define LV_CLOUD_MID_TOP    223
#define LV_CLOUD_HIG_TOP    233
#define LV_CLOUD_LOW_LAYER  214
#define LV_CLOUD_MID_LAYER  224
#define LV_CLOUD_HIG_LAYER  234

#define LV_TYPE_NOT_DEFINED 0   /* private */

// altitude index in tables
#define H925 0
#define H850 1
#define H700 2
#define H500 3
#define H300 4
#define H200 5

#define GEOPOTsize   8
#define GEOPOTidx(h) ((h)==925?0:(h)==850?1:(h)==700?2:(h)==600?3:(h)==500?4:(h)==400?5:(h)==300?6:(h)==200?7:-1)
#define GEOPOThgt(i) ((i)==0?925:(i)==1?850:(i)==2?700:(i)==3?600:(i)==4?500:(i)==5?400:(i)==6?300:(i)==7?200:-1)

//--------------------------------------------------------
// Data definition
//--------------------------------------------------------
class Altitude
{
	public:
		Altitude (int levelType=-1, int levelValue=-1)
			{  this->levelType  = levelType; 
			   this->levelValue = levelValue;  }
			
		int levelType;
		int levelValue;
		
		void set (int levelType=-1, int levelValue=-1)
			{  this->levelType  = levelType; 
			   this->levelValue = levelValue;  }
			   
		int index () const  { if (levelType==LV_ISOBARIC) 
								return GEOPOTidx(levelValue);
							  else return -1; }
			
		bool equals (int levelType, int levelValue) const
			{  return this->levelType==levelType && 
					  this->levelValue==levelValue;  }
		bool operator== (const Altitude &alt) const
			{ return alt.levelType==levelType
						&& alt.levelValue==levelValue; }
		bool operator!= (const Altitude &alt) const
			{ return alt.levelType!=levelType
						|| alt.levelValue!=levelValue; }
		bool operator< (const Altitude &alt) const
			{ return alt.levelType<levelType
						|| 
					(alt.levelType==levelType && alt.levelValue<levelValue); }
};
//--------------------------------------------------------
class DataCode
{
	public:
		DataCode (int dataType=GRB_TYPE_NOT_DEFINED, int levelType=-1, int levelValue=-1)
			{  this->dataType   = dataType; 
			   this->levelType  = levelType; 
			   this->levelValue = levelValue;  }
			
		DataCode (int dataType, const Altitude &alt)
			{  this->dataType   = dataType; 
			   this->levelType  = alt.levelType; 
			   this->levelValue = alt.levelValue;  }
			
		DataCode (uint32_t v)   { fromInt32 (v); }
			
		int dataType;
		int levelType;
		int levelValue;
		
		// int32 = #aabbccdd    aabb=levelValue cc=levelType dd=dataCode
		uint32_t toInt32 () const {
			return ((levelValue&0xFFFF)<<16)+((levelType&0xFF)<<8)+(dataType&0xFF);
		}
		void fromInt32 (uint32_t v) {
			levelValue = (v>>16) & 0xFFFF;
			levelType  = (v>>8) & 0xFF;
			dataType   =  v     & 0xFF;
		}
		
		void set (int dataType=GRB_TYPE_NOT_DEFINED, int levelType=-1, int levelValue=-1)
			{  this->dataType   = dataType; 
			   this->levelType  = levelType; 
			   this->levelValue = levelValue;  }
			   
		Altitude getAltitude () const
			{ return Altitude(levelType, levelValue); }
			
		bool equals (int dataType, int levelType, int levelValue) const
			{  return this->dataType==dataType && 
			          this->levelType==levelType && 
					  this->levelValue==levelValue;  }
		bool operator== (const DataCode &dtc) const
			{ return dtc.dataType==dataType 
						&& dtc.levelType==levelType
						&& dtc.levelValue==levelValue; }
		bool operator!= (const DataCode &dtc) const
			{ return dtc.dataType!=dataType 
						|| dtc.levelType!=levelType
						|| dtc.levelValue!=levelValue; }
		bool operator< (const DataCode &dtc) const
			{ return dataType<dtc.dataType
						|| 
					(dtc.dataType==dataType && getAltitude()<dtc.getAltitude()); }
};

#endif
