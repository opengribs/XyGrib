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

#ifndef GRIBPOINTINFO_H
#define GRIBPOINTINFO_H

#include "GriddedReader.h"

class DataPointInfo
{
    public :
        DataPointInfo (GriddedReader *reader, double x, double y, time_t date);

		bool isOk ()     const {return reader!=NULL;}
		
		float getDataValue (const DataCode &dtc) const;
		
        bool hasPressureMSL() const {return GribDataIsDef(pressureMSL);}
        bool hasTemp()     const {return GribDataIsDef(temp);}
        bool hasWaterTemp()const {return GribDataIsDef(waterTemp);}
        bool hasTempMin()  const {return GribDataIsDef(tempMin);}
        bool hasTempMax()  const {return GribDataIsDef(tempMax);}
        bool hasRain()     const {return GribDataIsDef(rain);}
        bool hasHumidRel()      const {return GribDataIsDef(humidRel);}
        bool hasHumidSpec()     const {return GribDataIsDef(humidSpec);}
        bool hasDewPoint()      const {return GribDataIsDef(dewPoint);}
        bool hasIsotherm0HGT()  const {return GribDataIsDef(isotherm0HGT);}
        bool hasSnowDepth()     const {return GribDataIsDef(snowDepth);}
        bool hasSnowCateg()     const {return GribDataIsDef(snowCateg);}
        bool hasFrzRainCateg()  const {return GribDataIsDef(frzRainCateg);}
        bool hasCAPEsfc()   	const {return GribDataIsDef(CAPEsfc);}
        bool hasCINsfc()   	const {return GribDataIsDef(CINsfc);}
        // added by david
        bool hasCompReflect()   const {return GribDataIsDef(compReflect);}
        bool hasGUSTsfc()   	const {return GribDataIsDef(GUSTsfc);}
        
        bool hasCloudTotal ()   const {return GribDataIsDef(cloudTotal);}
        bool hasCloudLow ()     const {return GribDataIsDef(cloudLow);}
        bool hasCloudMid ()     const {return GribDataIsDef(cloudMid);}
        bool hasCloudHigh ()    const {return GribDataIsDef(cloudHigh);}

        bool hasCloudLowTop ()     const {return GribDataIsDef(cloudLowTop);}
        bool hasCloudMidTop ()     const {return GribDataIsDef(cloudMidTop);}
        bool hasCloudHighTop ()    const {return GribDataIsDef(cloudHighTop);}

        bool hasCloudLowBottom ()     const {return GribDataIsDef(cloudLowBottom);}
        bool hasCloudMidBottom ()     const {return GribDataIsDef(cloudMidBottom);}
        bool hasCloudHighBottom ()    const {return GribDataIsDef(cloudHighBottom);}

		//----------------------------------------------
		// Special case : wind, current, waves
        bool hasWind (const Altitude &alt) const;
        bool hasCurrent () const;
        bool getWindValues (const Altitude &alt, float *speed, float*dir) const;
        bool getCurrentValues (float *speed, float*dir) const;
        void getWindVxVy   (const Altitude &alt, float *vx, float*vy) const;
        void getCurrentCxCy   (const Altitude &alt, float *vx, float*vy) const;
		
        bool getWaveValues (int prvtype, float*ht, float*per, float*dir) const;
        void getWaveWxWy   (int prvtype, float *wx, float*wy) const;

		bool  hasWaveData (int type) const;
		float getWaveData (int type) const;
		
		//----------------------------------------------
        double  x, y;       // position
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
        float   waterTemp;
		
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
        // added by david
        float   compReflect;
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
        float   wave_per;
        float   wave_swl_per;
        float   wave_wnd_per;
        float   wave_pr_per;
        float   wave_scdy_per;
        float   wave_dir;
        float   wave_swl_dir;
        float   wave_wnd_dir;
        float   wave_pr_dir;
        float   wave_scdy_dir;
        float   wave_wcap_prbl;
        float   wave_max_per;
        float   wave_max_dir;
        
		
	private:

	    float getValue (const DataCode &c) const
	    {
            return reader==nullptr ? GRIB_NOTDEF: static_cast<float>(reader->getDateInterpolatedValue (c, x,y,date));
        }

	    float getPercentValue (const DataCode &c) const
	    {
            return Util::inRangeOrNotDef(getValue(c), 0.f, 100.f);
        }

        GriddedReader *reader;
        void initDataPointInfo();
        
};

#endif
