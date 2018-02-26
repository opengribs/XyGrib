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

#ifndef GRIBRECORD_H
#define GRIBRECORD_H

#include <iostream>
#include <cmath>
#include <stdint.h>

#include "zuFile.h"
#include "RegularGridded.h"

#define DEBUG_INFO    false
#define DEBUG_ERROR   false
#define debug(format, ...)  {if(DEBUG_INFO)  {fprintf(stderr,format,__VA_ARGS__);fprintf(stderr,"\n");}}
#define erreur(format, ...) {if(DEBUG_ERROR) {fprintf(stderr,"ERROR: ");fprintf(stderr,format,__VA_ARGS__);fprintf(stderr,"\n");}}

#define zuint  uint32_t
#define zuchar uint8_t

//----------------------------------------------
class GribRecord : public RegularGridRecord  
{ 
    public:
        GribRecord ();
        GribRecord (ZUFILE* file, int id_);
        GribRecord (const GribRecord &rec);
        ~GribRecord ();
		
        bool  isOk ()  const   		{return ok;}
        bool  isDataKnown ()  const {return knownData;}
        int   getId ()  const   	{return id;}
		bool  isOrientationAmbiguous () const 
						{return verticalOrientationIsAmbiguous;}
		bool  isWaveData () 
						{return waveData;}
        //-----------------------------------------
		DataCode getDataCode () const
						{ return DataCode(dataType,levelType,levelValue); }
		void    setDataCode(const DataCode &dtc);
        zuchar  getDataType() const     { return dataType; }
        void    setDataType(const zuchar t);

        zuchar  getLevelType() const   { return levelType; }
        zuint   getLevelValue() const  { return levelValue; }

        //-----------------------------------------
		void    translateDataType();  // adapte les codes des différents centres météo
        //-----------------------------------------

        virtual int  getIdCenter() const { return idCenter; }
        virtual int  getIdModel()  const { return idModel; }
        virtual int  getIdGrid()   const { return idGrid; }

        //-----------------------------------------
        std::string getKey() const  { return dataKey; }
		static std::string makeKey(int dataType,int levelType,int levelValue);

        //-----------------------------------------
        int    getPeriodP1() const  { return periodP1; }
        int    getPeriodP2() const  { return periodP2; }

        // Nombre de points de la grille
        int     getNi() const    { return Ni; }
        int     getNj() const    { return Nj; }
        double  getDi() const    { return Di; }
        double  getDj() const    { return Dj; }
        double  getDeltaX() const    { return Di; }
        double  getDeltaY() const    { return Dj; }
		
        virtual int    getTotalNumberOfPoints ()  const
						{ return ok ? Ni*Nj : 0; }
        virtual double getAveragePointsDensity () const
						{ return ok ? Ni*Nj/((xmax-xmin)*(-ymin)) : 0; }

        // coordonnées d'un point de la grille
        double  getX(int i) const   { return ok ? xmin+i*Di : GRIB_NOTDEF;}
        double  getY(int j) const   { return ok ? ymin+j*Dj : GRIB_NOTDEF;}

        // Valeur pour un point de la grille
        double getValue (int i, int j) const 
							{ return ok ? data[j*Ni+i] : GRIB_NOTDEF;}
		
        // Valeur pour un point quelconque
        double  getInterpolatedValue (
							double px, double py,
							bool interpolate=true) const;

		virtual double  getInterpolatedValue (
							DataCode dtc,
							double px, double py,
							bool interpolate=true ) const;
		 
 		virtual double getValueOnRegularGrid ( 
						DataCode dtc, int i, int j ) const;

        void setValue (int i, int j, double v)
        		{ if (i>=0 && i<Ni && j>=0 && j<Nj)
        			data[j*Ni+i] = v; }

        // La valeur est-elle définie (grille à trous) ?
        inline bool   hasValue (int i, int j) const;

        // Date de référence (création du fichier)
        time_t getRecordRefDate () const         { return refDate; }
        const char* getStrRecordRefDate () const { return strRefDate; }

        // Date courante des prévisions
        time_t getRecordCurrentDate () const     { return curDate; }
        const char* getStrRecordCurDate () const { return strCurDate; }
        void  setRecordCurrentDate (time_t t);
		
        bool  isEof () const   {return eof;};
        virtual void  print (const char *title);

    protected:
        int    id;    // unique identifiant
        bool   ok;    // validité des données
        bool   knownData; 	// type de donnée connu
        bool   waveData;
		
        bool   eof;   // fin de fichier atteinte lors de la lecture
		std::string dataKey;
		char   strRefDate [32];
		char   strCurDate [32];
		bool   *boolBMStab;

        //---------------------------------------------
        // SECTION 0: THE INDICATOR SECTION (IS)
        //---------------------------------------------
        zuint  fileOffset0;
        zuint  seekStart, totalSize;
        zuchar editionNumber;
        // SECTION 1: THE PRODUCT DEFINITION SECTION (PDS)
        zuint  fileOffset1;
        zuint  sectionSize1;
        zuchar tableVersion;
        zuchar data1[28];
        zuchar idCenter;
        zuchar idModel;
        zuchar idGrid;
        int  dataType;      // octet 9 = parameters and units
        int  levelType;
        int  levelValue;
        bool   hasGDS;
        bool   hasBMS;
        zuint  refyear, refmonth, refday, refhour, refminute;
        zuchar periodP1, periodP2;
        zuchar timeRange;
        zuint  periodsec;    // period in seconds
        time_t refDate;      // Reference date
        time_t curDate;      // Current date
        double  decimalFactorD;
        // SECTION 2: THE GRID DESCRIPTION SECTION (GDS)
        zuint  fileOffset2;
        zuint  sectionSize2;
        zuchar NV, PV;
        zuchar gridType;
        int    Ni, Nj;
        double Di, Dj;
        zuchar resolFlags, scanFlags;
        bool  hasDiDj;
        bool  isEarthSpheric;
        bool  isUeastVnorth;
        bool  isScanIpositive;
        bool  isScanJpositive;
        bool  isAdjacentI;
        // SECTION 3: BIT MAP SECTION (BMS)
        zuint  fileOffset3;
        zuint  sectionSize3;
        zuchar *BMSbits;
        // SECTION 4: BINARY DATA SECTION (BDS)
        zuint  fileOffset4;
        zuint  sectionSize4;
        zuchar unusedBitsEndBDS;
        bool  isGridData;          // not spherical harmonics
        bool  isSimplePacking;
        bool  isFloatValues;
        bool   hasAdditionalFlags;
        int   scaleFactorE;
        double scaleFactorEpow2;
        double refValue;
        zuint  nbBitsInPack;
        double  *data;
        // SECTION 5: END SECTION (ES)

        //---------------------------------------------
        // Lecture des données
        //---------------------------------------------
        bool readGribSection0_IS (ZUFILE* file);
        bool readGribSection1_PDS(ZUFILE* file);
        bool readGribSection2_GDS(ZUFILE* file);
        bool readGribSection3_BMS(ZUFILE* file);
        bool readGribSection4_BDS(ZUFILE* file);
        bool readGribSection5_ES (ZUFILE* file);

        //---------------------------------------------
        // Fonctions utiles
        //---------------------------------------------
        zuchar readChar(ZUFILE* file);
        int    readSignedInt3(ZUFILE* file);
        int    readSignedInt2(ZUFILE* file);
        zuint  readInt2(ZUFILE* file);
        zuint  readInt3(ZUFILE* file);
        double readFloat4(ZUFILE* file);

        zuint  readPackedBits(zuchar *buf, zuint first, zuint nbBits);
        zuint  makeInt3(zuchar a, zuchar b, zuchar c);
        zuint  makeInt2(zuchar b, zuchar c);

        inline bool   hasValueInBitBMS (int i, int j) const;
		zuint  periodSeconds(zuchar unit, zuchar P1, zuchar P2, zuchar range);
        void   multiplyAllData(double k);
		
		void   checkOrientation ();
		void   reverseData (char orientation); // orientation = 'H' or 'V'
		bool   verticalDataAreMirrored ();
		
		// original values (to detect ambiguous headers)
		double savXmin,savXmax, savYmin,savYmax;
		double savDi, savDj;
		bool   verticalOrientationIsAmbiguous;
};

//==========================================================================
inline bool   GribRecord::hasValue (int i, int j) const
{
    // is data present in BMS ?
    if (entireWorldInLongitude) {
		while (i>=Ni)
			i -= Ni;
		while (i<0)
			i += Ni;
	}
    if (!ok) {
        return false;
    }
	if (i<0 || j<0 || i>=Ni || j>=Nj) {
        return false;
    }
    if (!hasBMS) {
        return true;
    }
	return boolBMStab [j*Ni+i];
}
//-----------------------------------------------------------------
inline bool   GribRecord::hasValueInBitBMS (int i, int j) const
{
    // is data present in BMS ?
    if (entireWorldInLongitude) {
		while (i>=Ni)
			i -= Ni;
		while (i<0)
			i += Ni;
	}
    if (!ok) {
        return false;
    }
    if (!hasBMS) {
        return true;
    }
    int bit;
    if (isAdjacentI) {
        bit = j*Ni + i;
    }
    else {
        bit = i*Nj + j;
    }
    zuchar c = BMSbits[bit/8];
    zuchar m = (zuchar)128 >> (bit % 8);
    return (m & c) != 0;
}

#endif



