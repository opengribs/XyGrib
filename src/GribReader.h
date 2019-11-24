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

/*************************
Lecture mise en mémoire d'un fichier GRIB

*************************/

#ifndef GRIBREADER_H
#define GRIBREADER_H
#include <cstdint>
#include <memory>

#include "RegularGridded.h"
#include "GribRecord.h"
#include "Grib2Record.h"
#include "LongTaskMessage.h"
#include "zuFile.h"
extern "C" {
    #include <grib2.h>
}

//===============================================================
class GribReader : public RegularGridReader, public LongTaskMessage
{
    public:
        GribReader ();
        ~GribReader ();
		
        void  openFile (const QString &fname, int nbrecs);
		
		virtual FileDataType getReaderFileDataType () 
					{return DATATYPE_GRIB;};

        int  getNumberOfGribRecords (DataCode dtc);
        int  getTotalNumberOfGribRecords ();
        
        GribRecord * getFirstGribRecord ();
        GribRecord * getFirstGribRecord (DataCode dtc);
		
        virtual time_t  getFirstRefDate ();
        virtual time_t  getRefDateForData (const DataCode &dtc);
		virtual time_t  getRefDateForDataCenter (const DataCenterModel &dcm);
		
		virtual GriddedRecord *getFirstRecord () 
							{return getFirstGribRecord();};
		
		virtual GribRecord *getRecord (DataCode dtc, time_t date);
		
        
		// Value at a point for an existing date
        virtual double getDateInterpolatedValue (
							DataCode dtc, double px, double py, time_t date);
        
		// Value at a point for a date between 2 existing dates
        double  get2DatesInterpolatedValue (
							DataCode dtc, double px, double py, time_t date);

		int	   getDewpointDataStatus (int levelType,int levelValue);

        // Rectangle de la zone couverte par les données
        bool getZoneExtension (double *x0,double *y0, double *x1,double *y1);

		enum GribFileDataStatus {
				DATA_IN_FILE, NO_DATA_IN_FILE, COMPUTED_DATA
		};

		void  computeAccumulationRecords ();
		void  computeAccumulationRecords (DataCode dtc);

		void  copyFirstCumulativeRecord   ();
		void  removeFirstCumulativeRecord ();
		void  copyFirstCumulativeRecord   (DataCode dtc);
		void  removeFirstCumulativeRecord (DataCode dtc);

		void  copyMissingWaveRecords   ();
		void  removeMissingWaveRecords ();
		void  copyMissingWaveRecords   (DataCode dtc);

		virtual bool hasAltitudeData () const  {return hasAltitude;}
		bool    hasAmbiguousHeader ()  {return ambiguousHeader;}
		
		void   interpolateMissingRecords ();
		void   interpolateMissingRecords (DataCode dtc);
		void   removeInterpolateRecords ();

		int countGribRecords (ZUFILE *f);

	protected:
        ZUFILE *file;
        void clean_vector(std::vector<GribRecord *> &ls);
        void clean_all_vectors();
        void createListDates ();
        //void removeRecordInMap (GribRecord *rec);
		void computeMissingData ();   // RH DewPoint ThetaE
		void analyseRecords ();

		int seekgb_zu (
				ZUFILE *lugb, g2int iseek, g2int mseek, g2int *lskip, g2int *lgrib);
		
    private:
        bool checkAndStoreRecordInMap (GribRecord *rec);
        bool storeRecordInMap (GribRecord *rec);
		void readGribFileContent (int nbrecs);
		bool readGribRecord(int id);
		//bool readGrib2Record(int id);
		bool readGrib2Record(int id, g2int lgrib);

		std::vector<std::shared_ptr<GribRecord>> * getListOfGribRecords (DataCode dtc);
        int	   dewpointDataStatus;
		bool   hasAltitude;
		bool   ambiguousHeader;
		
        std::map <uint64_t, std::vector<std::shared_ptr<GribRecord>>* >  mapGribRecords;

        void   openFilePriv (const QString& fname, int nbrecs);
        
        std::vector<std::shared_ptr<GribRecord>> *  getFirstNonEmptyList();
		
		double   computeDewPoint (double lon, double lat, time_t date);
		double   computeHumidRel (double lon, double lat, time_t date);

		// Interpolation entre 2 GribRecord
		double 	get2GribsInterpolatedValueByDate (
									double px, double py, time_t date,
									GribRecord *before, GribRecord *after);
		// Détermine les GribRecord qui encadrent une date
		void 	findGribsAroundDate (DataCode dtc, time_t date,
									GribRecord **before, GribRecord **after);
};


#endif
