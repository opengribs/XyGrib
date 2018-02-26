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

#include "Grib2Record.h"

//----------------------------------------
Grib2Record::Grib2Record () : GribRecord ()
{	
	ok = false;
	verticalOrientationIsAmbiguous = false;
}
//----------------------------------------
Grib2Record::~Grib2Record ()
{	
}
//----------------------------------------
Grib2Record::Grib2Record (gribfield  *gfld, int id, int idCenter, time_t refDate)
		: GribRecord ()
{
	if (gfld->locallen != 0) {
		DBG ("Unsupported local content : locallen=%ld", gfld->locallen);
		ok = false;
		return;
	}
	if (!(gfld->griddef==0 && gfld->igdtnum==0 && gfld->igdtlen>=19)) {
		// ! Latitude/Longitude grid
		DBG ("Unsupported grid type: %ld", gfld->igdtnum);
		ok = false;
		return;
	}	
	//------------------------------------------------
	// General infos
	//------------------------------------------------
	this->id = id;
	this->idCenter = idCenter;
	this->refDate = refDate;
	this->idGrid = gfld->griddef;
	sprintf(strRefDate, "%s", qPrintable(QDateTime::fromTime_t(refDate,Qt::UTC).toString("yyyy-MM-dd HH:mm")));
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
	double coefangle = 1e-6;
	if (gfld->igdtmpl[9]!=0 && gfld->igdtmpl[10]!=0)
		coefangle = (double)(gfld->igdtmpl[9])/gfld->igdtmpl[10];
	ymin = gfld->igdtmpl[11]*coefangle;
	xmin = gfld->igdtmpl[12]*coefangle;
	ymax = gfld->igdtmpl[14]*coefangle;
	xmax = gfld->igdtmpl[15]*coefangle;
	savXmin = xmin;
	savXmax = xmax;
	savYmin = ymin;
	savYmax = ymax;
	Di = gfld->igdtmpl[16]*coefangle;
	Dj = gfld->igdtmpl[17]*coefangle;
	savDi = Di;
	savDj = Dj;
    while ( xmin> xmax   &&  Di >0) {   // horizontal size > 360 °
        xmin -= 360.0;
    }
     while ( xmax> 360) {
		xmin -= 360.0;
        xmax -= 360.0;
    }
	resolFlags = gfld->igdtmpl[13];
	hasDiDj = (resolFlags&0x10)!=0 && (resolFlags&0x20)!=0;
	isUeastVnorth =  (resolFlags&0x08) ==0;
	scanFlags = gfld->igdtmpl[18];
	isScanIpositive = (scanFlags&0x80) ==0;
	isScanJpositive = (scanFlags&0x40) !=0;
	isAdjacentI     = (scanFlags&0x20) ==0;
	bool allRowsScanSameDir = (scanFlags&0x10) ==0;
	if (! allRowsScanSameDir) {
		erreur ("Unsupported grid type: allRowsScanSameDir=%d", (int)allRowsScanSameDir);
		ok = false;
	}
	if (Ni<=1 || Nj<=1) {
		DBG ("Record %d: Ni=%d Nj=%d",id,Ni,Nj);
		ok = false;
	}
	else { 
		Di = (xmax-xmin) / (Ni-1);
		Dj = (ymax-ymin) / (Nj-1);
	}
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
	this->data = new double[size];
	assert (this->data);

    // Read data in the order given by isAdjacentI
    int i, j;
    int ind, indgfld=0;
    if (isAdjacentI) {
        for (j=0; j<Nj; j++) {
            for (i=0; i<Ni; i++, indgfld++) {
                if (!hasDiDj && !isScanJpositive) {
                    ind = (Nj-1 -j)*Ni+i;
                }
                else {
                    ind = j*Ni+i;
                }
                if (!hasBMS || (hasBMS && gfld->bmap[ind])) {
                    data[ind] = gfld->fld[indgfld];
                }
                else {
                    data[ind] = GRIB_NOTDEF;
                }
            }
        }
    }
    else {
        for (i=0; i<Ni; i++) {
            for (j=0; j<Nj; j++, indgfld++) {
                if (!hasDiDj && !isScanJpositive) {
                    ind = (Nj-1 -j)*Ni+i;
                }
                else {
                    ind = j*Ni+i;
                }
                if (!hasBMS || (hasBMS && gfld->bmap[ind])) {
                    data[ind] = gfld->fld[indgfld];
                }
                else {
                    data[ind] = GRIB_NOTDEF;
                }
            }
        }
    }
	if (ok && hasBMS) { // replace the BMS bits table with a faster bool table
        boolBMStab = new bool [Ni*Nj];
		assert (boolBMStab);
		for (int i=0; i<Ni; i++) {
			for (int j=0; j<Nj; j++) {
				ind = j*Ni+i;
				boolBMStab [ind] = gfld->bmap[ind];
			}
		}
	}
	//----------------------------------------
	// end
	//----------------------------------------
	checkOrientation ();
	if (ok) {
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
void Grib2Record::analyseProductDefinitionTemplate (gribfield  *gfld)
{
	if (gfld->ipdtlen < 15) {
		DBG ("Missing parameters: ipdtlen=%ld", gfld->ipdtlen);
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
	if (pdtnum!=0 && pdtnum!=8) {
		DBG ("id=%d: unknown pdtnum: %d", id, pdtnum);
		pdtnum = -1;
		ok = false;
		return;
	}
	//-------------------------
	// forecast date
	//-------------------------
	else if (pdtnum == 0) {    // Analysis or forecast at a point in time
		int periodcode = gfld->ipdtmpl[7];    
		int periodoffset = gfld->ipdtmpl[8];
		if (periodcode == 0)
			this->curDate = this->refDate + 60*periodoffset;
		else if (periodcode == 1)
			this->curDate = this->refDate + 3600*periodoffset;
		else if (periodcode == 2)
			this->curDate = this->refDate + 24*3600*periodoffset;
		else if (periodcode == 10)
			this->curDate = this->refDate + 3*3600*periodoffset;
		else if (periodcode == 11)
			this->curDate = this->refDate + 6*3600*periodoffset;
		else if (periodcode == 12)
			this->curDate = this->refDate + 12*3600*periodoffset;
		else if (periodcode == 13)
			this->curDate = this->refDate + periodoffset;
		else {
			DBG("Can't determine forecast date");
			ok = false;
			return;
		}
	}
	else if (pdtnum == 8) {   // Average, accumulation  TODO: period of time
		int curyear  = gfld->ipdtmpl[15];  // end of the period
		int curmonth = gfld->ipdtmpl[16];
		int curday   = gfld->ipdtmpl[17];
		int curhour  = gfld->ipdtmpl[18];
		int curminute= gfld->ipdtmpl[19];
		int cursecond= gfld->ipdtmpl[20];
		this->curDate = DataRecordAbstract::UTC_mktime
							(curyear,curmonth,curday,curhour,curminute,cursecond);
	}
	sprintf(strCurDate, "%s", qPrintable(QDateTime::fromTime_t(curDate,Qt::UTC).toString("yyyy-MM-dd HH:mm")));
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
		DBG("Id=%d Altitude: surfaceType1=%lld surfaceScale1=%lld surfaceValue1=%lld",id,surfaceType1 ,surfaceScale1, surfaceValue1);
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
	// Unknown level types
	else {
		DBG("Unknown altitude: surfaceType1=%lld surfaceScale1=%lld surfaceValue1=%lld",surfaceType1 ,surfaceScale1, surfaceValue1);
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
	if (pdtnum==0) {
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
			else if (paramnumber==5)
				return GRB_GEOPOT_HGT;
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
			else if (paramnumber==52)
				return GRB_PRECIP_RATE;
			else if (paramnumber==193)
				return GRB_FRZRAIN_CATEG;
			else if (paramnumber==195)
				return GRB_SNOW_CATEG;
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
		fprintf(stderr,"data=%s alt=%s\n", qPrintable(DataCodeStr::toString_name(dataType)), qPrintable(AltitudeStr::toStringShort(Altitude(levelType,levelValue))) );
		fprintf(stderr,"dataType=%d levelType=%d levelValue=%d\n", dataType, levelType,levelValue);
		fprintf(stderr,"hour=%02g  cur=%s  ref=%s\n", (curDate-refDate)/3600.0,strCurDate,strRefDate);
// 		fprintf(stderr,"sav   xmin=%f xmax=%f    ymin=%f ymax=%f\n", savXmin,savXmax,savYmin,savYmax);
		fprintf(stderr,"final xmin=%f xmax=%f    ymin=%f ymax=%f\n", xmin,xmax,ymin,ymax);
		fprintf(stderr,"hasDiDj=%d Ni=%d Nj=%d    entireWorldInLongitude=%d\n", hasDiDj, Ni,Nj, (int)entireWorldInLongitude);
// 		fprintf(stderr,"savDi,savDj=(%f %f)\n", hasDiDj, savDi,savDj);
		fprintf(stderr,"final     Di,Dj=(%f %f)\n", Di,Dj);
		fprintf(stderr,"hasBMS=%d isScanIpositive=%d isScanJpositive=%d isAdjacentI=%d\n",
							hasBMS, isScanIpositive,isScanJpositive,isAdjacentI );
	}
	else {
		fprintf(stderr,"====== ERROR : GribRecord %d : %s\n", id, title);
	}
}




