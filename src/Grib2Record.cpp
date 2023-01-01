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

#include "Grib2Record.h"

//----------------------------------------
// david added discipline
Grib2Record::Grib2Record (gribfield  *gfld, int id, int idCenter, time_t refDate, int dscpl)
{
// changed by david to allow ICON gribs
//    if (gfld->locallen != 0) {
//		DBG ("Unsupported local content : locallen=%ld", gfld->locallen);
//        ok = false;
//        return;
//	}
	double laD;
	double loV;
	double latin1, latin2;

	if (gfld->igdtnum == 0) {
		// regular lat/long
		double coefangle = 1e-6;

		if (!(gfld->griddef==0 && gfld->igdtlen>=19)) {
			// ! Latitude/Longitude grid
			DBG ("Unsupported grid type: %ld", gfld->igdtnum);
			ok = false;
			return;
		}
		scanFlags = gfld->igdtmpl[18];
		resolFlags = gfld->igdtmpl[13];
		if (gfld->igdtmpl[9]!=0 && gfld->igdtmpl[10]!=0)
			coefangle = (double)(gfld->igdtmpl[9])/gfld->igdtmpl[10];
		ymin = gfld->igdtmpl[11]*coefangle;
		xmin = gfld->igdtmpl[12]*coefangle;
		ymax = gfld->igdtmpl[14]*coefangle;
		xmax = gfld->igdtmpl[15]*coefangle;
		Di = gfld->igdtmpl[16]*coefangle;
		Dj = gfld->igdtmpl[17]*coefangle;
	}
	else if (gfld->igdtnum == 10) {
		// Mercator
		scanFlags  = gfld->igdtmpl[15];
		ymin = gfld->igdtmpl[9]/1000000.;
		xmin = gfld->igdtmpl[10]/1000000.;
		resolFlags = gfld->igdtmpl[11];
		//laD  = gfld->igdtmpl[12]/1000000.;
		ymax = gfld->igdtmpl[13]/1000000.;
		xmax = gfld->igdtmpl[14]/1000000.;
		Di = gfld->igdtmpl[17];
		Dj = gfld->igdtmpl[18];
	}
	else if( gfld->igdtnum == 20 ) {
		// Polar Stereographic 	 
		ymin = gfld->igdtmpl[9]/1000000.;  //  latitude of first gridpoint
		xmin = gfld->igdtmpl[10]/1000000.; //  longitude of first gridpoint
		resolFlags = gfld->igdtmpl[11];
		laD  = gfld->igdtmpl[12]/1000000.;
		ymax = ymin;
		xmax = xmin;
		loV = gfld->igdtmpl[13]/1000000.;
		Di = gfld->igdtmpl[14]/1000;
		Dj = gfld->igdtmpl[15]/1000;
		scanFlags = gfld->igdtmpl[17];
    }
	else if( gfld->igdtnum == 30 ) {
		// Lambert Conformal
		ymin = gfld->igdtmpl[9]/1000000.;  //  latitude of first gridpoint
		xmin = gfld->igdtmpl[10]/1000000.; //  longitude of first gridpoint
		resolFlags = gfld->igdtmpl[11];
		//laD  = gfld->igdtmpl[12]/1000000.;
		ymax = ymin;
		xmax = xmin;
		loV = gfld->igdtmpl[13]/1000000.;
		Di = gfld->igdtmpl[14]/1000;
		Dj = gfld->igdtmpl[15]/1000;
		scanFlags = gfld->igdtmpl[17];
		latin1 = gfld->igdtmpl[18]/1000000.;
		latin2 = gfld->igdtmpl[19]/1000000.;
    }
    else {
		DBG ("Unsupported grid type: %ld", gfld->igdtnum);
		ok = false;
		return;
	}

	//------------------------------------------------
	// General infos
	//------------------------------------------------
    // changed by david
    this->discipline = dscpl;

	this->id = id;
	this->idCenter = idCenter;
	this->refDate = refDate;
	this->idGrid = gfld->griddef;
	sprintf(strRefDate, "%s", qPrintable(QDateTime::fromSecsSinceEpoch(refDate,Qt::UTC).toString("yyyy-MM-dd HH:mm")));
	ok = true;
	waveData = false;  // TODO
	verticalOrientationIsAmbiguous = false;
	dataType = GRB_TYPE_NOT_DEFINED;
	levelType = LV_TYPE_NOT_DEFINED;
	//------------------------------------------------
	// Grid parameters (table 3.0)
	//------------------------------------------------
	this->idGrid = gfld->igdtmpl[2];
	isEarthSpheric = ( gfld->igdtmpl[0]==0 || gfld->igdtmpl[0]==1
					|| gfld->igdtmpl[0]==5 || gfld->igdtmpl[0]==6
					|| gfld->igdtmpl[0]==8);
	Ni = gfld->igdtmpl[7];
	Nj = gfld->igdtmpl[8];
	if (Ni<=1 || Nj<=1) {
		DBG ("Record %d: Ni=%d Nj=%d",id,Ni,Nj);
		ok = false;
		return;
	}
	savXmin = xmin;
	savXmax = xmax;
	savYmin = ymin;
	savYmax = ymax;
	savDi = Di;
	savDj = Dj;
	if( gfld->igdtnum != 30 && gfld->igdtnum != 20 ) {
	    while ( xmin> xmax   &&  Di >0) {   // horizontal size > 360 °
    	    xmin -= 360.0;
		}
		while ( xmax> 360) {
			xmin -= 360.0;
			xmax -= 360.0;
		}
		// checkOrientation ();
		if (xmin==xmax) {
			if (Di >= 0)
				xmin = xmin-360.0;
			else
				xmin = xmin+360.0;
			Di = 360.0/Ni;
		}
		Di = (xmax-xmin) / (Ni-1);
		Dj = (ymax-ymin) / (Nj-1);

		double v;
		if (xmin > xmax)
		{
			//printf("GribRecord::checkOrientation (): xmin>xmax => must reverse data\n");
			v=xmin;  xmin=xmax;  xmax=v;
			Di = fabs(Di);
		}
		if (ymin > ymax)
		{
			//printf("GribRecord::checkOrientation (): ymin>ymax => must reverse data\n");
			v=ymin;  ymin=ymax;  ymax=v;
			Dj = fabs(Dj);
		}
		while (xmin<=-180) {
        	xmin += 360.0;
        	xmax += 360.0;
		}
	}

	if (gfld->igdtnum == 0) {
	    grid = std::make_shared<PlateCarree>(Ni, Nj, xmin, ymin, Di, Dj);
	}
	else if (gfld->igdtnum == 10) {
	    grid = std::make_shared<Mercator>(Ni, Nj, xmin, ymin, ymax, Di, Dj);
	}
	else if( gfld->igdtnum == 20 ) {
	    grid = std::make_shared<Stereographic>(Ni, Nj, xmin, ymin, Dj/1000., laD, loV);
	}
	else if( gfld->igdtnum == 30 ) {
	    grid = std::make_shared<Lambert>(Ni, Nj, xmin, ymin, Dj/1000., latin1, latin2, loV);
	}

	if( gfld->igdtnum == 30 || gfld->igdtnum == 20 ) {
		double lon, lat, x, y;

	    // XXXX compute xmax, ymax
	    xmin = 10000;
	    xmax = -10000;
	    ymin = 10000;
	    ymax = -10000;
		grid->XY2LonLat(0, 0, lon, lat);
		if (lon < xmin) xmin = lon;
		if (lon > xmax) xmax = lon;
		if (lat < ymin) ymin = lat;
		if (lat > ymax) ymax = lat;

		grid->XY2LonLat(Ni, 0, lon, lat);
		if (lon < xmin) xmin = lon;
		if (lon > xmax) xmax = lon;
		if (lat < ymin) ymin = lat;
		if (lat > ymax) ymax = lat;

		grid->XY2LonLat(Ni, Nj, lon, lat);
		if (lon < xmin) xmin = lon;
		if (lon > xmax) xmax = lon;
		if (lat < ymin) ymin = lat;
		if (lat > ymax) ymax = lat;

		grid->XY2LonLat(0, Nj, lon, lat);
		if (lon > 360)
			lon -= 360;
		if (lon < xmin) xmin = lon;
		if (lon > xmax) xmax = lon;
		if (lat < ymin) ymin = lat;
		if (lat > ymax) ymax = lat;
		if (xmin < loV && loV < xmax) {
			grid->lonLat2XY(loV, ymin, x, y);
			grid->XY2LonLat(x, 0, lon, lat);
			if (lat < ymin) ymin = lat;
			if (lat > ymax) ymax = lat;
			grid->XY2LonLat(x, Nj, lon, lat);
			if (lat < ymin) ymin = lat;
			if (lat > ymax) ymax = lat;
		}
		Di = (xmax-xmin) / (Ni-1);
		Dj = (ymax-ymin) / (Nj-1);
    }
	hasDiDj = (resolFlags&0x10)!=0 && (resolFlags&0x20)!=0;
	isUeastVnorth =  (resolFlags&0x08) ==0;
	isScanIpositive = (scanFlags&0x80) ==0;
	isScanJpositive = (scanFlags&0x40) !=0;
	isAdjacentI     = (scanFlags&0x20) ==0;
	//----------------------------------------
	// BMS
	//----------------------------------------
	hasBMS = (gfld->ibmap == 0 || gfld->ibmap == 254);
	//----------------------------------------
	// Product
	//----------------------------------------
	analyseProductDefinitionTemplate (gfld);
	//----------------------------------------
	// Data
	//----------------------------------------
	size_t size = Ni*Nj;
	auto ptr = new data_t[size];
    this->data = std::shared_ptr<data_t>(ptr, std::default_delete<data_t[]>());

    // Read data in the order given by isAdjacentI
	for (int j=0; j<Nj; j++) {
		for (int i=0; i<Ni; i++) {
			int indgfld;
			switch(scanFlags){
				case 0:   /* 0000 0000 */ indgfld = (Nj -j -1)*Ni +i;          break;
				case 128: /* 1000 0000 */ indgfld = (Nj -j -1)*Ni +(Ni -i -1); break;
				case 64:  /* 0100 0000 */ indgfld =          j*Ni +i;          break;
				case 192: /* 1100 0000 */ indgfld =          j*Ni +(Ni -i -1); break;
				case 32:  /* 0010 0000 */ indgfld =          i*Nj +(Nj -j -1); break;
				case 160: /* 1010 0000 */ indgfld = (Ni -i -1)*Nj +(Ni -i -1); break;
				case 96:  /* 0110 0000 */ indgfld =          i*Nj +j;          break;
				case 224: /* 1110 0000 */ indgfld = (Ni -i -1)*Nj +j;          break;
				case 80:  /* 0101 0000 */ indgfld = ( j % 2 == 0 ?
                                                j*Ni +i :
                                                j*Ni +(Ni -i -1) );            break;
				default:
					ok = false;
					return;
			}
			int ind = j*Ni +i;
			if (!hasBMS || gfld->bmap[indgfld]) {
				data.get()[ind] = gfld->fld[indgfld];
			}
            else {
                data.get()[ind] = GRIB_NOTDEF;
            }
        }
    }
#if 0
	// don't keep BMS around nothing is using it (GRIB_NOTDEF)
	if (ok && hasBMS) { // replace the BMS bits table with a faster bool table
        boolBMStab = new bool [Ni*Nj];
		assert (boolBMStab);
		for (int j=0; j<Nj; j++) {
			for (int i=0; i<Ni; i++) {
				int ind = j*Ni+i;
				boolBMStab [ind] = gfld->bmap[ind];
			}
		}
	}
#endif
	//----------------------------------------
	// end
	//----------------------------------------
	if (ok) {
		editionNumber = 2;
		translateDataType ();
		setDataType (dataType);
		entireWorldInLongitude = (fabs(xmax-xmin)>=360.0)||(fabs(xmax-360.0+Di-xmin) < fabs(Di/20));
		//DBG("%g %g %g %g entireWorldInLongitude=%d ", xmin, xmax, Di, fabs(xmax-360.0+Di-xmin),(int)entireWorldInLongitude);
	}
	//========================
	// this->print("");
	//========================
}
//---------------------------------------------------------------
// https://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_doc/grib2_table4-4.shtml
static int unit_of_time_range(int periodcode)
{
	if (periodcode == 0)
		return 60;
	if (periodcode == 1)
		return 3600;
	if (periodcode == 2)
		return 24*3600;
	if (periodcode == 10)
		return 3*3600;
	if (periodcode == 11)
		return 6*3600;
	if (periodcode == 12)
		return 12*3600;
	if (periodcode == 13)
		return 1;
	return 0;
}

//---------------------------------------------------------------
void Grib2Record::analyseProductDefinitionTemplate (gribfield  *gfld)
{
	if (gfld->ipdtlen < 15) {
//		DBG ("Missing parameters: ipdtlen=%ld", gfld->ipdtlen);
		ok = false;
		return;
	}
	//-----------------------------------
	// debug
	//-----------------------------------
// 	if (id==319 || id==287) {
// 		DBG("gfld->ipdtnum = %ld", gfld->ipdtnum);
// 		for (int i=0; i<gfld->ipdtlen; i++) {
// 			DBG("gfld->ipdtmpl[%2d] = %ld", i, gfld->ipdtmpl[i]);
// 		}
// 	}
	//-----------------------------------
	this->idModel = gfld->ipdtmpl[4];
	pdtnum = gfld->ipdtnum; // = Product Definition Template Number(see Code Table 4.0)
    if (pdtnum!=0 && pdtnum!=2 && pdtnum!=8 && pdtnum!=12 ) {
		DBG ("id=%d: unknown pdtnum: %d", id, pdtnum);
		pdtnum = -1;
		ok = false;
		return;
	}
	//-------------------------
	// forecast date
	//-------------------------
	if (pdtnum == 8) {   // Average, accumulation
	    if (gfld->ipdtlen < 27) {
           DBG ("Missing parameters: ipdtlen=%d", (int)gfld->ipdtlen);
           ok = false;
           return;
        }

		int curyear  = gfld->ipdtmpl[15];  // end of the period
		int curmonth = gfld->ipdtmpl[16];
		int curday   = gfld->ipdtmpl[17];
		int curhour  = gfld->ipdtmpl[18];
		int curminute= gfld->ipdtmpl[19];
		int cursecond= gfld->ipdtmpl[20];
		if (gfld->ipdtmpl[21] != 1) {
            DBG("id=%d: pdtnum: %d more than one time range (%d)", id, pdtnum, (int)gfld->ipdtmpl[21]);
            pdtnum = -1;
            ok = false;
            return;
		}
		this->curDate = DataRecordAbstract::UTC_mktime
							(curyear,curmonth,curday,curhour,curminute,cursecond);
        switch (gfld->ipdtmpl[23]) { // proc_code, table 4.10
		case 0: /* average */
            timeRange = 3;
		    break;
		case 1: /* accumulation */
		    timeRange = 4;
		    break;
	    case 2: // maximum
	    case 3: // minimum
		    timeRange = 2;
		    break;
		case 4: /* difference */
		    timeRange = 5;
		    break;
		default:
            DBG("id=%d: pdtnum: %d unknown proc code %d", id, pdtnum, (int)gfld->ipdtmpl[23]);
            pdtnum = -1;
            ok = false;
            return;
        }
        if (unit_of_time_range(gfld->ipdtmpl[25]) >= 3600) {
            periodP1 = gfld->ipdtmpl[8] *unit_of_time_range(gfld->ipdtmpl[25])/3600;
            periodP2 = periodP1 + gfld->ipdtmpl[26] *unit_of_time_range(gfld->ipdtmpl[25])/3600; // time_length
            resosec = 3600;
        }
        else if (unit_of_time_range(gfld->ipdtmpl[25]) == 60 && gfld->ipdtmpl[8] % 60 == 0 &&  gfld->ipdtmpl[26] % 60 == 0) {
            periodP1 = gfld->ipdtmpl[8] /60;
            periodP2 = periodP1 + gfld->ipdtmpl[26] /60; // time_length
            resosec = 3660;
        }

        else {
            DBG("Can't determine forecast date");
            ok = false;
            return;
        }
    }
	else {
		// 0 Analysis or forecast at a point in time
		// 2 Derived forecasts based on all ensemble members at a horizontal level or in a horizontal layer at a point in time. 
		//12 Derived forecasts based on all ensemble members at ... in a continuous or non-continuous time interval
		int periodcode = gfld->ipdtmpl[7];    
		int periodoffset = gfld->ipdtmpl[8];
		if (unit_of_time_range(periodcode))
			this->curDate = this->refDate + unit_of_time_range(periodcode)*periodoffset;
		else {
			DBG("Can't determine forecast date");
			ok = false;
			return;
		}
	}
	sprintf(strCurDate, "%s", qPrintable(QDateTime::fromSecsSinceEpoch(curDate,Qt::UTC).toString("yyyy-MM-dd HH:mm")));
	//-------------------------
	// Altitude
	//-------------------------
	readAltitude (gfld);
	if (levelType == LV_TYPE_NOT_DEFINED) {
		pdtnum = -1;
		ok = false;
		return;
	}
	//this->print("");

	//-------------------------
	// Product type
	//-------------------------
	paramcat = gfld->ipdtmpl[0];   // Parameter category (see Code table 4.1)
	paramnumber = gfld->ipdtmpl[1];   //Parameter number (see Code table 4.2)
	dataType = analyseProductType ();
	if (dataType == GRB_TYPE_NOT_DEFINED) {
		ok = false;
		return;
	}
	//-------------------------
	knownData = true; 	// type de donnée connu
// 	if (
// 			id==373
// 		// || id==311 || id==384 || id==397
// 	) {
// 		print("");
// 	}
}
//----------------------------------------------
void Grib2Record::readAltitude (gribfield *gfld)
{
	qint64 surfaceType1 = gfld->ipdtmpl[9]; // Type of first fixed surface (see Code table 4.5)
	qint64 surfaceScale1 = gfld->ipdtmpl[10];// Scale factor of first fixed surface
	qint64 surfaceValue1 = gfld->ipdtmpl[11]; //Scaled value of first fixed surface
// 	qint64 surfaceType2 = gfld->ipdtmpl[12]; // Type of first fixed surface (see Code table 4.5)
// 	qint64 surfaceScale2 = gfld->ipdtmpl[13];// Scale factor of first fixed surface
// 	qint64 surfaceValue2 = gfld->ipdtmpl[14]; //Scaled value of first fixed surface
// 	DBG("surfaceType2=%lld surfaceScale2=%lld surfaceValue2=%lld",surfaceType2 ,surfaceScale2, surfaceValue2);
//DBG("Altitude: surfaceType1=%lld surfaceScale1=%lld surfaceValue1=%lld",surfaceType1 ,surfaceScale1, surfaceValue1);
	// cf. table 4.5
	if (   surfaceType1<0 || surfaceType1>255
	) {
//		DBG("Id=%d Altitude: surfaceType1=%lld surfaceScale1=%lld surfaceValue1=%lld",id,surfaceType1 ,surfaceScale1, surfaceValue1);
		levelType = LV_TYPE_NOT_DEFINED;
		levelValue = 0;
		return;
	}
	//-----------------------------------
	levelValue = surfaceValue1;
	if (surfaceType1==1) {
		levelType = LV_GND_SURF;
		levelValue = 0;
	}
	else if (surfaceType1==4)
		levelType = LV_ISOTHERM0;
	else if (surfaceType1==10) {
		levelType = LV_ATMOS_ALL;   // assimiled LV_ATMOS_ENT
		levelValue = 0;
	}
	else if (surfaceType1==100) {
		levelType = LV_ISOBARIC;
		levelValue = surfaceValue1/100.0;   // hPa
	}
	else if (surfaceType1==101) {
		levelType = LV_MSL;
		levelValue = 0;
	}
	else if (surfaceType1==102)
		levelType = LV_ABOV_MSL;
	else if (surfaceType1==103)
		levelType = LV_ABOV_GND;
	else if (surfaceType1==104) {
		levelType = LV_SIGMA;
		levelValue = surfaceValue1 * 10000;  // compatible with grib1 value
	}
	else if (surfaceType1==200) {
		levelType = LV_ATMOS_ALL;
		levelValue = 0;
	}
	else if (surfaceType1==212)
		levelType = LV_CLOUD_LOW_BOTTOM;
	else if (surfaceType1==213)
		levelType = LV_CLOUD_LOW_TOP;
	else if (surfaceType1==214)
		levelType = LV_CLOUD_LOW_LAYER;
	else if (surfaceType1==222)
		levelType = LV_CLOUD_MID_BOTTOM;
	else if (surfaceType1==223)
		levelType = LV_CLOUD_MID_TOP;
	else if (surfaceType1==224)
		levelType = LV_CLOUD_MID_LAYER;
	else if (surfaceType1==232)
		levelType = LV_CLOUD_HIG_BOTTOM;
	else if (surfaceType1==233)
		levelType = LV_CLOUD_HIG_TOP;
	else if (surfaceType1==234)
		levelType = LV_CLOUD_HIG_LAYER;
    // current levels below sea surface
    else if (surfaceType1==160)
        levelType = LV_BLW_SURF;
	// Ignored level types
	else if (surfaceType1==106)
		levelType = LV_TYPE_NOT_DEFINED;  // Depth Below Land Surface
	else if (surfaceType1==108)
		levelType = LV_TYPE_NOT_DEFINED;  // Level at Specified Pressure Difference from Ground to Level
	else if (surfaceType1==109)
		levelType = LV_TYPE_NOT_DEFINED;  // Potential Vorticity Surface
	else if (surfaceType1==220)
		levelType = LV_TYPE_NOT_DEFINED;  // Planetary Boundary Layer
	else if (surfaceType1==6)
		levelType = LV_TYPE_NOT_DEFINED;  // Maximum Wind Level
	else if (surfaceType1==7)
		levelType = LV_TYPE_NOT_DEFINED;  // Tropopause
	else if (surfaceType1==8)
		levelType = LV_TYPE_NOT_DEFINED;  // Nominal Top of the Atmosphere
	else if (surfaceType1==242)
		levelType = LV_TYPE_NOT_DEFINED;  // Convective cloud bottom level 	
	else if (surfaceType1==243)
		levelType = LV_TYPE_NOT_DEFINED;  // Convective cloud top level 	
	else if (surfaceType1==244)
		levelType = LV_TYPE_NOT_DEFINED;  // Convective cloud layer
	else if (surfaceType1==211)
		levelType = LV_TYPE_NOT_DEFINED;  // Boundary layer cloud layer
	else if (surfaceType1==204)
		levelType = LV_TYPE_NOT_DEFINED;  // Highest tropospheric freezing level
    else if (surfaceType1==192)
        levelType = LV_PRIVATE;           //icon ml_cape for instance
	// Unknown level types
	else {
//        DBG("Unknown altitude: surfaceType1=%lld surfaceScale1=%lld surfaceValue1=%ld",surfaceType1 ,surfaceScale1, surfaceValue1);
		levelType = LV_TYPE_NOT_DEFINED;
		levelValue = 0;
	}
	// apply scale
	if (surfaceScale1>0  &&  surfaceScale1<255) {
		levelValue /= pow(10.0,surfaceScale1);
	}
}
//----------------------------------------------
int Grib2Record::analyseProductType ()
{
	if (   pdtnum<0 ||pdtnum>65535
		|| paramcat<0 ||paramcat>65535
	) {
		pdtnum = -1;
		return GRB_TYPE_NOT_DEFINED;
	}
    // added by david to manage wave data in g2
    if (discipline==10){// TABLE 4.10.0
        if (paramcat==0){
            if (paramnumber==3)
                return GRB_WAV_SIG_HT;
            if (paramnumber==4)
                return GRB_WAV_WND_DIR;
            if (paramnumber==5)
                return GRB_WAV_WND_HT;
            if (paramnumber==6)
                return GRB_WAV_WND_PER;
            if (paramnumber==7)
                return GRB_WAV_SWL_DIR;
            if (paramnumber==8)
                return GRB_WAV_SWL_HT;
            if (paramnumber==9)
                return GRB_WAV_SWL_PER;
            if (paramnumber==10)
                return GRB_WAV_PRIM_DIR;
            if (paramnumber==11)
                return GRB_WAV_PRIM_PER;
            if (paramnumber==14)
                return GRB_WAV_DIR;
            if (paramnumber==15)
                return GRB_WAV_PER;
            if (paramnumber==23)
                return GRB_WAV_MAX_PER; //Period of Maximum Individual Wave Height
            if (paramnumber==24)
                return GRB_WAV_MAX_HT; //Maximum Individual Wave Height
        } else if (paramcat==1){
            if (paramnumber==0)
                return GRB_CUR_DIR;
            if (paramnumber==1)
                return GRB_CUR_SPEED;
            if (paramnumber==2)
                return GRB_CUR_VX;
            if (paramnumber==3)
                return GRB_CUR_VY;
        } else if (paramcat==3){ // Surface Properties
            if (paramnumber==0)
                return GRB_WTMP;
        }
    }
    if (discipline != 0)
	    return GRB_TYPE_NOT_DEFINED;

    if (pdtnum==0 || pdtnum== 2  || pdtnum== 12 ) {
		if (paramcat==0) {//TABLE 4.2-0-0
			if (paramnumber==0)
				return GRB_TEMP;
			else if (paramnumber==2)
				return GRB_TEMP_POT;
			else if (paramnumber==4)
				return GRB_TMAX;
			else if (paramnumber==5)
				return GRB_TMIN;
			else if (paramnumber==6)
				return GRB_DEWPOINT;
		}
		else if (paramcat==1) {//TABLE 4.2-0-1
			if (paramnumber==0)
				return GRB_HUMID_SPEC;
			else if (paramnumber==1)
				return GRB_HUMID_REL;
			else if (paramnumber==11)
				return GRB_SNOW_DEPTH;
		}
		else if (paramcat==2) {//TABLE 4.2-0-2
			if (paramnumber==0)
				return GRB_WIND_DIR;
			else if (paramnumber==1)
				return GRB_WIND_SPEED;
			else if (paramnumber==2)
				return GRB_WIND_VX;
			else if (paramnumber==3)
				return GRB_WIND_VY;
			else if (paramnumber==22)
				return GRB_WIND_GUST;
			else if (paramnumber==23)
				return GRB_WIND_GUST_VX;
			else if (paramnumber==24)
				return GRB_WIND_GUST_VY;
		}
		else if (paramcat==3) {//TABLE 4.2-0-3
			if (paramnumber==0)
				return GRB_PRESSURE;
			else if (paramnumber==1)
				return GRB_PRESSURE_MSL;
            else if (paramnumber==4)
                return GRB_GEOPOT; // added by david
            else if (paramnumber==5)
                return GRB_GEOPOT_HGT;
            else if (paramnumber==6)
                return GRB_GEOPOT_HGT; // david, use DIST geometric height as geopotential height (icon hzerocl)
        }
		else if (paramcat==6) {//TABLE 4.2-0-6
			if (paramnumber==1)
				return GRB_CLOUD_TOT;
			else if (paramnumber==3)
				return GRB_CLOUD_LOW;
			else if (paramnumber==4)
				return GRB_CLOUD_MID;
			else if (paramnumber==5)
				return GRB_CLOUD_HIG;
		}
		else if (paramcat==7) {//TABLE 4.2-0-7
			if (paramnumber==6)
				return GRB_CAPE;
			else if (paramnumber==7)
				return GRB_CIN;
		}
        // added by david
        else if (paramcat==16) {//TABLE 4.2-0-16
            if (paramnumber==5 || paramnumber==196)
                return GRB_COMP_REFL;
        }
	}
	else if (pdtnum==8) {
		if (paramcat==0) {//TABLE 4.2-0-0
			if (paramnumber==0)
				return GRB_TEMP;
			else if (paramnumber==2)
				return GRB_TEMP_POT;
			else if (paramnumber==4)
				return GRB_TMAX;
			else if (paramnumber==5)
				return GRB_TMIN;
			else if (paramnumber==6)
				return GRB_DEWPOINT;
		}
		if (paramcat==1) {//TABLE 4.2-0-1
			if (paramnumber==7)
				return GRB_PRECIP_RATE;
			else if (paramnumber==8)
				return GRB_PRECIP_TOT;
			else if (paramnumber==49)
				return GRB_PRECIP_TOT;
			else if (paramnumber==65)
				return GRB_PRECIP_TOT;
			else if (paramnumber==52) {
				/*
				   cf
				   https://www.wmo.int/pages/prog/www/ISS/Meetings/CT-MTDCF-ET-DRC_Montreal2006/Doc2-2-5(1).doc
				   and
				   https://www.wmo.int/pages/prog/www/ISS/Meetings/IPET-DRC_Brasilia2010/Documents/IPETDRC-II_Doc2-3_13.doc
				   ''''
				   According to the official WMO documentation for GRIB2,
				   the parameter psum (number 8) is deprecated.
				   Instead, parameter prate (number 52) together with the statistical processing “accumulation”
				   (Code Table 4.10) and suitable product definition templates (e.g. Template 4.8)
				   is used for total precipitation summed up over a specified time interval.

				   the statistical process “accumulation” changes the units to kg m-2 and the non-normalized
				   total precipitation sum is encoded in prate (number 52).
				   '''
				   But

				   '''
				   "Total precipitation rate" as sum of convective and large-scale precipitation rate is still a rate and 
				   shall be coded with "at a point in time" templates,  parameter number 52 (Total precipitation rate), 
				   category 1 (Moisture), discipline 0 (Meteorological products). It is the instantaneous value at the forecast time.

				   "Total precipitation" shall be coded with type of statistical processing 1 (Accumulation), 
				   parameter number 8 (Total precipitation), category 1 (Moisture), discipline 0 (Meteorological products)."
				   '''

				   For Xygrib that's mean GRB_PRECIP_TOT
				*/
				if (pdtnum == 8) {
					// Used in Meteo France, ICON grib
					// Product Definition Template Number(Table 4.0)
					// 8 = Average, accumulation, etc..
					return GRB_PRECIP_TOT;
				}
				// XXX need a file for testing
				return GRB_PRECIP_RATE;
			}
			else if (paramnumber==193)
				return GRB_FRZRAIN_CATEG;
			else if (paramnumber==195)
				return GRB_SNOW_CATEG;
		}
        // added by david to enable ICON gust field
        else if (paramcat==2) {
            if (paramnumber==22)
                return GRB_WIND_GUST;
            else if (paramnumber==23)
                return GRB_WIND_GUST_VX;
            else if (paramnumber==24)
                return GRB_WIND_GUST_VY;
        }
		else if (paramcat==6) {//TABLE 4.2-0-6
			if (paramnumber==1)
				return GRB_CLOUD_TOT;
		}
	}
// 	DBG("Unknown product: pdtnum=%d paramcat=%d paramnumber=%d alt=%s",
// 				pdtnum, paramcat, paramnumber,
// 				qPrintable(AltitudeStr::toStringShort(Altitude(levelType,levelValue)))
// 			);
    return GRB_TYPE_NOT_DEFINED;
}
//---------------------------------------------
void Grib2Record::print (const char *title)
{
	if (ok) {
		fprintf(stderr,"====== Grib2Record %d : %s\n", id, title);
		fprintf(stderr,"idCenter=%d idModel=%d idGrid=%d\n", idCenter,idModel,idGrid);
		fprintf(stderr,"scanFlags= 0x%x\n", scanFlags);
		fprintf(stderr,"data=%s alt=%s\n", qPrintable(DataCodeStr::toString_name(dataType)), qPrintable(AltitudeStr::toStringShort(Altitude(levelType,levelValue))) );
		fprintf(stderr,"dataType=%d levelType=%d levelValue=%d\n", dataType, levelType,levelValue);
		fprintf(stderr,"hour=%02g  cur=%s  ref=%s\n", (curDate-refDate)/3600.0,strCurDate,strRefDate);
// 		fprintf(stderr,"sav   xmin=%f xmax=%f    ymin=%f ymax=%f\n", savXmin,savXmax,savYmin,savYmax);
		fprintf(stderr,"final xmin=%f xmax=%f    ymin=%f ymax=%f\n", xmin,xmax,ymin,ymax);
		fprintf(stderr,"hasDiDj=%d Ni=%d Nj=%d    entireWorldInLongitude=%d\n", hasDiDj, Ni,Nj, (int)entireWorldInLongitude);
// 		fprintf(stderr,"savDi,savDj=(%f %f)\n", hasDiDj, savDi,savDj);
		fprintf(stderr,"final     Di,Dj=(%f %f)\n", Di,Dj);
		fprintf(stderr,"scanFlags=0x%x\n", scanFlags);
		fprintf(stderr,"hasBMS=%d isScanIpositive=%d isScanJpositive=%d isAdjacentI=%d\n",
							hasBMS, isScanIpositive,isScanJpositive,isAdjacentI );
	}
	else {
		fprintf(stderr,"====== ERROR : GribRecord %d : %s\n", id, title);
	}
}
