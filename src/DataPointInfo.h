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

#ifndef GRIBPOINTINFO_H
#define GRIBPOINTINFO_H

#include "GriddedReader.h"

class DataPointInfo
{
    public :
        DataPointInfo (GriddedReader *reader, float x, float y, time_t date);

		bool isOk ()     const {return reader!=NULL;}
		
		float getDataValue (const DataCode &dtc) const;
		
        bool hasPressureMSL() const {return pressureMSL!=GRIB_NOTDEF;}
        bool hasTemp()     const {return temp!=GRIB_NOTDEF;}
        bool hasTempMin()  const {return tempMin!=GRIB_NOTDEF;}
        bool hasTempMax()  const {return tempMax!=GRIB_NOTDEF;}
        bool hasRain()     const {return rain!=GRIB_NOTDEF;}
        bool hasHumidRel()      const {return humidRel!=GRIB_NOTDEF;}
        bool hasHumidSpec()     const {return humidSpec!=GRIB_NOTDEF;}
        bool hasDewPoint()      const {return dewPoint!=GRIB_NOTDEF;}
        bool hasIsotherm0HGT()  const {return isotherm0HGT!=GRIB_NOTDEF;}
        bool hasSnowDepth()     const {return snowDepth!=GRIB_NOTDEF;}
        bool hasSnowCateg()     const {return snowCateg!=GRIB_NOTDEF;}
        bool hasFrzRainCateg()  const {return frzRainCateg!=GRIB_NOTDEF;}
        bool hasCAPEsfc()   	const {return CAPEsfc!=GRIB_NOTDEF;}
        bool hasCINsfc()   	const {return CINsfc!=GRIB_NOTDEF;}
        bool hasGUSTsfc()   	const {return GUSTsfc!=GRIB_NOTDEF;}
        
        bool hasCloudTotal ()   const {return cloudTotal!=GRIB_NOTDEF;}
        bool hasCloudLow ()     const {return cloudLow!=GRIB_NOTDEF;}
        bool hasCloudMid ()     const {return cloudMid!=GRIB_NOTDEF;}
        bool hasCloudHigh ()    const {return cloudHigh!=GRIB_NOTDEF;}

        bool hasCloudLowTop ()     const {return cloudLowTop!=GRIB_NOTDEF;}
        bool hasCloudMidTop ()     const {return cloudMidTop!=GRIB_NOTDEF;}
        bool hasCloudHighTop ()    const {return cloudHighTop!=GRIB_NOTDEF;}

        bool hasCloudLowBottom ()     const {return cloudLowBottom!=GRIB_NOTDEF;}
        bool hasCloudMidBottom ()     const {return cloudMidBottom!=GRIB_NOTDEF;}
        bool hasCloudHighBottom ()    const {return cloudHighBottom!=GRIB_NOTDEF;}

		//----------------------------------------------
		// Special case : wind, current, waves
        bool hasWind (const Altitude &alt) const;
        bool hasCurrent () const;
        bool getWindValues (const Altitude &alt, float *speed, float*dir) const;
        bool getCurrentValues (float *speed, float*dir) const;
        void getWindVxVy   (const Altitude &alt, float *vx, float*vy) const;
        void getCurrentCxCy   (const Altitude &alt, float *vx, float*vy) const;
		
        bool  getWaveValues (int prvtype, float*ht, float*per, float*dir) const;
		bool  hasWaveData (int type) const;
		float getWaveData (int type) const;
		
		//----------------------------------------------
        float   x, y;       // position
        time_t  date;

        float	cx, cy; // Current
		float currentSpeed;
		float currentDir;

        float   vx_10m, vy_10m;     // wind
        float   vx_gnd, vy_gnd;     // wind
		float   windSpeed_10m;
		float   windDir_10m;
		float   windSpeed_gnd;
		float   windDir_gnd;
        float   pressureMSL;
        float   rain;
        float   tempMin;
        float   tempMax;
		
        float    temp;
		Altitude tempAltitude;
		
		bool    hasCloudLayers;
        float   cloudTotal;
        float   cloudLow;
        float   cloudMid;
        float   cloudHigh;
        float   cloudLowTop;
        float   cloudMidTop;
        float   cloudHighTop;
        float   cloudLowBottom;
        float   cloudMidBottom;
        float   cloudHighBottom;
		
        float   humidRel;
        float   humidSpec;
        float   dewPoint;
        float   isotherm0HGT;
        float   snowDepth;
        float   snowCateg;
        float   frzRainCateg;
        float   CAPEsfc;
        float   CINsfc;
        float   GUSTsfc;

		// data at 925 850 700 600 500 400 300 200 hPa
		bool    hasAltitudeData;
		float  hThetae [GEOPOTsize];
		float  hGeopot [GEOPOTsize];
		float  hTemp   [GEOPOTsize];
		float  hHumidRel  [GEOPOTsize];
		float  hHumidSpec [GEOPOTsize];
		float  hVx  [GEOPOTsize];
		float  hVy  [GEOPOTsize];
		float  hWindSpeed [GEOPOTsize];
		float  hWindDir   [GEOPOTsize];
		
        float   wave_sig_ht;
        float   wave_max_ht;
        float   wave_swl_ht;
        float   wave_wnd_ht;
        float   wave_swl_per;
        float   wave_wnd_per;
        float   wave_pr_per;
        float   wave_scdy_per;
        float   wave_swl_dir;
        float   wave_wnd_dir;
        float   wave_pr_dir;
        float   wave_scdy_dir;
        float   wave_wcap_prbl;
        float   wave_max_per;
        float   wave_max_dir;
        
		
	private:
        GriddedReader *reader;
        void initDataPointInfo();
        
};

#endif
