#ifndef THERM_H
#define THERM_H

#include <cstdio>
#include <cmath>

#include "Util.h"

// Useful thermodynamic formulas
//
// Documents:
// - Wikipedia
// - Olivier Bouvignies,"L'Émagramme 761 des météorologues", 
//                       Bulletin de l'union des physiciens, vol. 815, no 2, juin 1999
// - skew-t.pdf   Written by Bret D. Whissel, Tallahassee, Florida, November 2011.
//               http://bretwhissel.net/skewt/

#ifndef GRIB_NOTDEF
#define GRIB_NOTDEF -99999
#endif
//-----------------------------------------------------------
class TPoint
{
	public:
		TPoint (double tempC=GRIB_NOTDEF, double hpa=GRIB_NOTDEF)
						{this->tempC=tempC; this->hpa=hpa;}
		TPoint& operator= (const TPoint &other)
						{tempC=other.tempC; hpa=other.hpa; return *this;}
		bool   ok ()  {return hpa!=GRIB_NOTDEF && tempC!=GRIB_NOTDEF;}
		double hpa;      // altitude
		double tempC;    // °C	
};
//-----------------------------------------------------------
class TPCurve
{
	public:
		QList <TPoint> points;
		
		void clear () {points.clear();}
		void addPoint (const TPoint &pt) {points << pt;}
		void addPoint (double T, double P) {points << TPoint(T,P);}
		
		double hpaMin ()
			{if (points.isEmpty()) return GRIB_NOTDEF; else return points.last().hpa;}
		double hpaMax ()
			{if (points.isEmpty()) return GRIB_NOTDEF; else return points.first().hpa;}
};

//-----------------------------------------------------------
class Therm
{
	public:
		
		static double hpa2m (double hpa);	// Conversion altitude/pressure
		static double m2hpa (double z);		
		static double gByAlt (double z);		
		
		static double latentHeatWater (double tempC);    // return J/kg
		static double mixingRatio (double tempC, double hpa);   // return kg/kg
		static double tempFromMixingRatio (double mixr, double hpa);
		static double virtualTemperatureC (double tempC, double hpa);
		
		static double specHumidFromRelative (double tempK, double hr);   // kg/kg
		static double relHumidFromSpecific (double tempK, double hs);   // 0..100
		
		// Compute the equivalent potential temperature (Theta-e) 
		static double thetaEfromHR (double tempK, double hpa, double hr); // hr: 0..100
		static double thetaEfromHS (double tempK, double hpa, double hs); // hs: kg/kg

		static double vaporPressure (double tempC);	     // return hPa
		static double tempFromVaporPressure (double hpa);  // return °C

		static double dryAdiabaticTemperature (double hpa0, double t0, double hpa);
		static double dryAdiabaticPressure (double hpa0, double t0, double tempC);
		
		static double gammaSaturatedAdiabatic (double tempC, double hpa);
		static double saturated_dT_dP (double tempC, double hpa);
		static double saturatedAdiabaticTemperature (double tempC0, double hpa0, double hpa);
		
		static void curveSaturatedAdiabatic (TPCurve *curve, TPoint &start, double hpaLimit, double step);
		static void curveSaturatedAdiabatic (TPCurve *curve, double tempC0, double hpa0, double hpaLimit, double step);
};

//-----------------------------------------------------------
class SoundingPoint
{
	public :
		SoundingPoint (double hpa=1000, double tempC=0, double dewpC=0)
						{this->hpa=hpa; this->tempC=tempC; this->dewpC=dewpC;}
		double hpa;             // altitude
		double tempC, dewpC;    // °C
		bool   ok () const {return hpa!=GRIB_NOTDEF && tempC!=GRIB_NOTDEF && dewpC!=GRIB_NOTDEF;}
		bool operator< (const SoundingPoint &other) const
									{return this->hpa < other.hpa;}
};

//-----------------------------------------------------------
class SoundingPointWind
{
	public :
		SoundingPointWind (double hpa=GRIB_NOTDEF, double vx=GRIB_NOTDEF, double vy=GRIB_NOTDEF)
						{this->hpa=hpa; this->vx=vx; this->vy=vy;}
		double hpa;             // altitude
		double vx, vy;			// wind m/s
		bool   ok () const {return hpa!=GRIB_NOTDEF && vx!=GRIB_NOTDEF && vy!=GRIB_NOTDEF;}
		double speedKts () {return sqrt(vx*vx+vy*vy)*3.6/1.852;}
		double speedMs () {return sqrt(vx*vx+vy*vy);}
		double degrees () {return  - atan2 (-vx, vy) *180.0/M_PI + 180;}
};

//-----------------------------------------------------------
class Sounding
{
	public :
		enum ConvBase {
			LV1000, LV975, LV950, LV925, LV900, LV875, LV850, LV825, LV800,
			AVG25, AVG50, AVG75, AVG100, AVG125, AVG150, AVG175, AVG200
		};
		
		Sounding ();
		
		void addSoundingPointC (double hpa, double tempC, double dewpC);
		void addSoundingPointK (double hpa, double tempK, double dewpK);
		
		void addSoundingPointWind (double hpa, double vx, double vy);
		
		QList <SoundingPoint> * getAllSounds() {return &allSounds;}
		
		QList <SoundingPointWind> * getAllSoundsWind() {return &allSoundsWind;}
		SoundingPointWind  getWindByAlt (double hpa);
		void   compute_convective_levels (double hpa0max, double hpa0min);
		
		double getTempCByAlt (double hpa);
		double getDewpCByAlt (double hpa);

		double getAvgTempCByAlt (double hpa1, double hpa2);
		double getAvgDewpCByAlt (double hpa1, double hpa2);

		double getAltByTempC (double tempC);
		double getAltByDewpC (double dewpC);
		
		double hpaMin ();
		double hpaMax ();
		
		void invalidateConvectiveLevels () 
							{levelsAreValid = false;}
									
		TPoint get_LCL (double hpa0max, double hpa0min);  // Lifted condensation level
		TPoint get_CCL (double hpa0max, double hpa0min);  // Convective condensation level
		TPoint get_LFC (double hpa0max, double hpa0min);  // Level of free convection
		TPoint get_EL  (double hpa0max, double hpa0min);  // Equilibrium level
		
		double LI,  SI, KI, TT, SWEAT, CAPE, CIN;
		QList <TPoint> curveCAPE;
		QList <TPoint> curveCIN;

	private:
		QList <SoundingPoint> allSounds;
		QList <SoundingPointWind> allSoundsWind;
			
		bool   levelsAreValid;
		ConvBase clvl_base;
		
		TPoint LCL, CCL, LFC, EL;
		
		TPoint compute_LCL (double hpa0, double temp0, double dewp0, double deltap);
};

#endif
