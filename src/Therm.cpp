
#include "Therm.h"

//----------------------------------------------------------------------
double Therm::hpa2m (double hpa)
{
	const double L = -6.5e-3;
	const double Ra = 287.05;
	const double Pb = 101325;
	const double Tb = 288.15;
	const double gb = 9.807;
	double Ps = hpa*100;
	return (Tb * pow(Pb/Ps, L*Ra/gb) - Tb) / L;
}
//------------------------------------------------------
double Therm::m2hpa (double z)
{
	const double L = -6.5e-3;
	const double Ra = 287.05;
	const double Pb = 101325;
	const double Tb = 288.15;
	const double gb = 9.807;
	double Ps = Pb / pow ( (z*L+Tb)/Tb, gb/L/Ra);
	return Ps/100.0;
}
//------------------------------------------------------
double Therm::gByAlt (double z)
{
	const double R = 6378*1e3;
	const double g0 = 9.81;
	return g0*R*R/((R+z)*(R+z));
}
//------------------------------------------------------
double  Therm::vaporPressure (double tempC)
{
	// August-Roche-Magnus formula
	// http://en.wikipedia.org/wiki/Clausius%E2%80%93Clapeyron_relation
	const double A = 17.625;
	const double B = 243.04;
	const double C = 6.1094;
	return C*exp(A*tempC/(tempC+B));	// hPa
}
//------------------------------------------------------
double  Therm::tempFromVaporPressure (double hpa)
{
	// inverse August-Roche-Magnus formula
	// http://en.wikipedia.org/wiki/Clausius%E2%80%93Clapeyron_relation
	const double A = 17.625;
	const double B = 243.04;
	const double C = 6.1094;
	double esc = log (hpa/C);
	return B*esc/(A-esc);
}
//------------------------------------------------------
double Therm::mixingRatio (double tempC, double hpa)
{
	const double eps = 0.622;
	double psat = vaporPressure (tempC);
	return eps*psat/(hpa-psat);
}
//------------------------------------------------------
double Therm::tempFromMixingRatio (double mixr, double hpa)
{
	const double eps = 0.622;
	double psat = hpa*mixr/(eps+mixr);
	return tempFromVaporPressure (psat);
}
//------------------------------------------------------
double Therm::virtualTemperatureC  (double tempC, double hpa)
{
	double r = Therm::mixingRatio (tempC, hpa);
	double tv = (tempC+273.15)*(1.0+0.61*r) - 273.15;
	//DBG ("T=%g TV=%g    r=%g", tempC,tv, r);
	return tv;
}
//------------------------------------------------------
double Therm::specHumidFromRelative (double tempK, double hr)
{
	// Nadeau et Puiggali formula: http://fr.wikipedia.org/wiki/Humidit%C3%A9_sp%C3%A9cifique
	double rap = 472.68/tempK;
	double psat = exp (23.3265-3802.7/tempK-rap*rap);
	hr = hr/100.0;   // 0..100 -> 0..1
	return 0.622*psat*hr/(101325-psat*hr);
}
//------------------------------------------------------
double Therm::relHumidFromSpecific (double tempK, double hs)
{
	double ps = exp (23.3265 - 3802.7/tempK - (472.68*472.68)/(tempK*tempK));
	double hr = 100.0* (101325.0*hs/((0.622+hs)*ps));
	if (hr < 0.0)   
		hr = 0.0;
	else if (hr > 100.0) 
		hr = 100.0;
	return hr;
}
//----------------------------------------------------------------------
double Therm::thetaEfromHR (double tempK, double hpa, double hr)
{
	if (hr==GRIB_NOTDEF || hpa==GRIB_NOTDEF || tempK==GRIB_NOTDEF)
		return GRIB_NOTDEF;
	return thetaEfromHS (tempK, hpa, specHumidFromRelative(tempK, hr));
}
//----------------------------------------------------------------------
double Therm::thetaEfromHS (double tempK, double hpa, double hs)
{
	if (tempK==GRIB_NOTDEF || hpa==GRIB_NOTDEF || hs==GRIB_NOTDEF
		|| hs==1.0 || hpa==0.0
	) {
		return GRIB_NOTDEF;
	}
	double mr = 1000.0 * hs/(1.0-hs);	// mixing ratio (g/kg)
	double Cp = 1004.0;
	double Rd = 287.0;
	double P0 = 1000.0;
	// source: http://en.wikipedia.org/wiki/Latent_heat
	// Lv = (2404.83 kJ/kg {at 40 °C} to 2601.83 kJ/kg {at −40 °C})
	// => cubic interpolation between -40 and 40, else line y=ax+b
	double Lv;
	double a = (2404.83-2601.83)/80.0;
	double b = 2404.83-40.0*a;
	double tc = tempK-273.15;
	if (tc < -40 || tc>40)
		Lv = a*tc + b;
	else
		Lv = -0.0000614342*tc*tc*tc + 0.00158927*tc*tc - 2.36418*tc + 2500.79;	
	double thetae;	// θe
	thetae = (tempK + Lv/Cp*mr) * pow(P0/hpa, Rd/Cp);
	return thetae;
}
//------------------------------------------------------
double  Therm::latentHeatWater (double tempC)    // J/kg
{
	// http://en.wikipedia.org/wiki/Latent_heat
    const double A = -6.14342e-5;
    const double B = 1.58927e-3;
    const double C = -2.36418;
    const double D = 2500.79;
    return 1000.0 * (A*tempC*tempC*tempC + B*tempC*tempC + C*tempC + D);
}
//------------------------------------------------------
double Therm::gammaSaturatedAdiabatic (double tempC, double hpa)
{
	// return saturated adiabatic lapse rate in Kelvin/m
	const double g = 9.8076;   // m/s2
	const double eps = 0.622;
	const double Cpd = 1005.7;	// J kg−1 K−1;
	const double Cpv = 1952;	// J kg−1 K−1;
	const double Rsd = 287.053;	   // J kg−1 K−1;		
	double Hv = latentHeatWater (tempC);
	double T = tempC + 273.15;
	double r = mixingRatio (tempC, hpa);
 	return g * (1.0+r)*(1.0+Hv*r/(Rsd*T))
 		  / (Cpd + r*Cpv + Hv*Hv*r*(eps+r)/(Rsd*T*T));
}
//------------------------------------------------------
double Therm::saturated_dT_dP (double tempC, double hpa)
{
	const double Rd = 287.053;	   // J kg−1 K−1;
	const double Rv = 461.5; 	   // J kg−1 K−1;
	const double g = 9.8076;   // m/s2
	double gamma = gammaSaturatedAdiabatic (tempC,  hpa);
	double es = vaporPressure (tempC);
	double tempK = tempC + 273.15;
	double rho = (hpa-es)/(Rd*tempK) + es/(Rv*tempK);
	return gamma / (g*rho);
}

//------------------------------------------------------
double Therm::saturatedAdiabaticTemperature (double tempC0, double hpa0, double hpa)
{
	double deltap = 0.1;
	double T = tempC0;
	if (hpa < hpa0) {
		for (double P=hpa0; P >= hpa; P -= deltap) {
			double dtdp = saturated_dT_dP (T, P); 
			T -= dtdp*deltap;
		}
	}
	else  {
		for (double P=hpa0; P <= hpa; P += deltap) {
			double dtdp = saturated_dT_dP (T, P); 
			T += dtdp*deltap;
		}
	}
	return T;
}
//------------------------------------------------------
void Therm::curveSaturatedAdiabatic (TPCurve *curve, double tempC0, double hpa0, double hpaLimit, double step)
{
	double T, P;
	curve->clear ();
	T = tempC0;
	curve->addPoint (tempC0, hpa0);
	if (step < 0) {
		for (P=hpa0+step; P >= hpaLimit+step; P += step) {
			T = Therm::saturatedAdiabaticTemperature (tempC0, hpa0, P);
			curve->addPoint (T, P);
		}
	}
	else if (step > 0) {
		for (P=hpa0+step; P <= hpaLimit+step; P += step) {
			T = Therm::saturatedAdiabaticTemperature (tempC0, hpa0, P);
			curve->addPoint (T, P);
		}
	}
}
//------------------------------------------------------
void Therm::curveSaturatedAdiabatic (TPCurve *curve, TPoint &start, double hpaLimit, double step)
{
	Therm::curveSaturatedAdiabatic (curve, start.tempC, start.hpa, hpaLimit, step);
}
//------------------------------------------------------
double Therm::dryAdiabaticTemperature (double hpa0, double t0, double hpa)
{
	const double R = 8.314472;
	const double Ma = 0.029;
	const double Cpa = 1005;
	return (t0+273.15) * pow (hpa/hpa0 , R/(Ma*Cpa)) - 273.15;
}
//------------------------------------------------------
double Therm::dryAdiabaticPressure (double hpa0, double t0, double tempC)
{
	const double R = 8.314472;
	const double Ma = 0.029;
	const double Cpa = 1005;
	return  hpa0 * pow ((tempC+273.15)/(t0+273.15), (Ma*Cpa)/R);
}


//===================================================================
// Sounding
//===================================================================
Sounding::Sounding ()
{
	levelsAreValid = false;
	levelsAreValid = false;
}
//------------------------------------------------------
void Sounding::addSoundingPointC (double hpa, double tempC, double dewpC)
{
	allSounds << SoundingPoint (hpa, tempC, dewpC);
	qSort (allSounds);
	levelsAreValid = false;
}
//------------------------------------------------------
void Sounding::addSoundingPointK (double hpa, double tempK, double dewpK)
{
	addSoundingPointC (hpa, tempK - 273.15, dewpK - 273.15);
}
//------------------------------------------------------
void Sounding::addSoundingPointWind (double hpa, double vx, double vy)
{
	if (vx!=GRIB_NOTDEF && vy!=GRIB_NOTDEF) {
		allSoundsWind << SoundingPointWind (hpa, vx, vy);
		levelsAreValid = false;
	}
}
//------------------------------------------------------
double Sounding::hpaMax ()
{
	if (allSounds.size() > 0)
		return allSounds[allSounds.size()-1].hpa;
	else
		return GRIB_NOTDEF;
}
//------------------------------------------------------
double Sounding::hpaMin ()
{
	if (allSounds.size() > 0)
		return allSounds[0].hpa;
	else
		return GRIB_NOTDEF;
}
//------------------------------------------------------
SoundingPointWind Sounding::getWindByAlt (double hpa)
{
	for (int i=0; i<allSoundsWind.size(); i++) {
		SoundingPointWind pw = allSoundsWind [i];
		if (pw.hpa == hpa) {
			return pw;
		}
	}
	return SoundingPointWind ();
}
//------------------------------------------------------
double Sounding::getTempCByAlt (double hpa)
{
	double res = GRIB_NOTDEF;
	bool found = false;
	int i;
	if (allSounds.size() >= 2) {
		for (i=0; i<allSounds.size()-1; i++) {
			if (hpa>=allSounds[i].hpa && hpa<=allSounds[i+1].hpa) {
				found = true;
				break;
			}
		}
		if (found) {
			double v0 = allSounds[i].tempC;
			double v1 = allSounds[i+1].tempC;
			double k = Therm::hpa2m(allSounds[i+1].hpa) - Therm::hpa2m(allSounds[i].hpa);
			if (k != 0)
				k = (Therm::hpa2m(hpa) - Therm::hpa2m(allSounds[i].hpa))/k;
			res = v0 + k * (v1-v0);
		}
	}
	return res;
}
//------------------------------------------------------
double Sounding::getDewpCByAlt (double hpa)
{
	double res = GRIB_NOTDEF;
	bool found = false;
	int i;
	if (allSounds.size() >= 2) {
		for (i=0; i<allSounds.size()-1; i++) {
			if (hpa>=allSounds[i].hpa && hpa<=allSounds[i+1].hpa) {
				found = true;
				break;
			}
		}
		if (found) {
			double v0 = allSounds[i].dewpC;
			double v1 = allSounds[i+1].dewpC;
			double k = Therm::hpa2m(allSounds[i+1].hpa) - Therm::hpa2m(allSounds[i].hpa);
			if (k != 0)
				k = (Therm::hpa2m(hpa) - Therm::hpa2m(allSounds[i].hpa))/k;
			res = v0 + k * (v1-v0);
		}
	}
	return res;
}
//------------------------------------------------------
double Sounding::getAvgTempCByAlt (double hpa1, double hpa2) 
{
	Util::orderMinMax (hpa1, hpa2);
	int n = 0;
	double res = 0;
	for (double h=hpa1; h<=hpa2; h+=0.5) {
		double t = getTempCByAlt (h);
		if (t != GRIB_NOTDEF) {
			n ++;
			res += t;
		}
	}
	return (n > 0) ? res/n : GRIB_NOTDEF;
}
//------------------------------------------------------
double Sounding::getAvgDewpCByAlt (double hpa1, double hpa2)
{
	Util::orderMinMax (hpa1, hpa2);
	int n = 0;
	double res = 0;
	for (double h=hpa1; h<=hpa2; h+=0.5) {
		double t = getDewpCByAlt (h);
		if (t != GRIB_NOTDEF) {
			n ++;
			res += t;
		}
	}
	return (n > 0) ? res/n : GRIB_NOTDEF;
}
//------------------------------------------------------
double Sounding::getAltByTempC (double tempC)
{
	double res = GRIB_NOTDEF;
	bool found = false;
	int i;
	if (allSounds.size() >= 2) {
		for (i=0; i<allSounds.size()-1; i++) {
			if (   (tempC>=allSounds[i].tempC && tempC<=allSounds[i+1].tempC)
				|| (tempC<=allSounds[i].tempC && tempC>=allSounds[i+1].tempC)
			) {
				found = true;
				break;
			}
		}
		if (found) {
			double v0 = Therm::hpa2m(allSounds[i].hpa);
			double v1 = Therm::hpa2m(allSounds[i+1].hpa);
			double k = allSounds[i+1].tempC - allSounds[i].tempC;
			if (k != 0)
				k = (tempC - allSounds[i].tempC)/k;
			else
				k = 0;
			res = v0 + k * (v1-v0);
		}
	}
	return res;
}
//------------------------------------------------------
double Sounding::getAltByDewpC (double dewpC)
{
	double res = GRIB_NOTDEF;
	bool found = false;
	int i;
	if (allSounds.size() >= 2) {
		for (i=0; i<allSounds.size()-1; i++) {
			if (   (dewpC>=allSounds[i].dewpC && dewpC<=allSounds[i+1].dewpC)
				|| (dewpC<=allSounds[i].dewpC && dewpC>=allSounds[i+1].dewpC)
			) {
				found = true;
				break;
			}
		}
		if (found) {
			double v0 = Therm::hpa2m(allSounds[i].hpa);
			double v1 = Therm::hpa2m(allSounds[i+1].hpa);
			double k = allSounds[i+1].dewpC - allSounds[i].dewpC;
			if (k != 0)
				k = (dewpC - allSounds[i].dewpC)/k;
			else
				k = 0;
			res = v0 + k * (v1-v0);
		}
	}
	return res;
}
//------------------------------------------------------
TPoint Sounding::get_LCL (double hpa0max, double hpa0min)
{
	if (levelsAreValid)
		return LCL;
	compute_convective_levels (hpa0max, hpa0min);
	return LCL;
}
//------------------------------------------------------
TPoint Sounding::get_CCL (double hpa0max, double hpa0min)
{
	if (levelsAreValid)
		return CCL;
	compute_convective_levels (hpa0max, hpa0min);
	return CCL;
}
//------------------------------------------------------
TPoint Sounding::get_LFC (double hpa0max, double hpa0min)
{
	if (levelsAreValid)
		return LFC;
	compute_convective_levels (hpa0max, hpa0min);
	return LFC;
}
//------------------------------------------------------
TPoint Sounding::get_EL (double hpa0max, double hpa0min)
{
	if (levelsAreValid)
		return EL;
	compute_convective_levels (hpa0max, hpa0min);
	return EL;
}
//------------------------------------------------------
TPoint Sounding::compute_LCL (double hpa0, double temp0, double dewp0, double deltap)
{
	// Compute LCL from level hpa0 (follows mixing ratio)
	TPoint lcl;
	double p = hpa0;
	double temp = temp0;
	double mixr = Therm::mixingRatio (dewp0, hpa0);
	double tmixr = dewp0;
	while (tmixr < temp && p>hpaMin()-1)
	{
		p = p - deltap;
		temp = Therm::dryAdiabaticTemperature (hpa0, temp0, p);
		tmixr = Therm::tempFromMixingRatio (mixr, p);
	}
	if (tmixr >= temp)
		lcl = TPoint (temp, p);
	return lcl;
}
//------------------------------------------------------
void Sounding::compute_convective_levels (double hpa0max, double hpa0min)
{
	if (levelsAreValid)
		return;
	levelsAreValid = true;

	LCL = CCL = LFC = EL = TPoint (GRIB_NOTDEF, GRIB_NOTDEF);
	CAPE = 0;
	CIN = 0;
	curveCAPE.clear ();
	curveCIN.clear ();
	
	double hpa0mean = (hpa0min+hpa0max)/2.0;
	if (allSounds.size() < 2)
		return;
	double deltap = 1.5;
	//--------------------------------------------------
	// Compute LCL and CIN (follows mixing ratio) 
	//--------------------------------------------------
	QList <TPoint> curve1;	// right curve (up)
	QList <TPoint> curve2; // left curve (down)
	double p = hpa0mean;
	double temp0 = getAvgTempCByAlt (hpa0min, hpa0max);
	double tempdry = temp0;
	double dewp = getAvgDewpCByAlt (hpa0min, hpa0max);
	double mixr = Therm::mixingRatio (dewp, hpa0mean);
	double tmixr = dewp;
	double temp, z0, z1, dz;
	CIN = 0;
	z0 = Therm::hpa2m (hpa0mean);
	//DBG ("temp0 %g   dewp %g   mixr %g ",temp0,dewp,mixr);
	while (tmixr < tempdry && p>hpaMin()-1)
	{
		p = p - deltap;
		tempdry = Therm::dryAdiabaticTemperature (hpa0mean, temp0, p);
		tmixr = Therm::tempFromMixingRatio (mixr, p);
		temp = getTempCByAlt (p);
		//DBG ("p=%.2f   tempdry %.2f   mixr %.2f ", p,tempdry,tmixr);
		// CIN
		if (tempdry < temp)
		{
			z1 = Therm::hpa2m (p);
			dz = z1 - z0;
			z0 = z1;
			double tv = Therm::virtualTemperatureC  (temp, p);
			double tpv = Therm::virtualTemperatureC (tempdry, p);
			CIN += Therm::gByAlt(z0)*(dz*(tpv - tv)/(tv+273.15));
			curve1.append (TPoint(temp, p));
			curve2.insert (0, TPoint(tempdry, p));
		}
		else
		{
			curve1.append (TPoint(tempdry,p));
			curve2.insert (0, TPoint(tempdry,p));
		}
	}
	if (tmixr >= tempdry) {
		//DBG ("Found LCL : temp=%g   h=%g hpa", tempdry,p);
		LCL = TPoint (tempdry, p);
	}
	//--------------------------------------------------
	if (! LCL.ok()) {
		CAPE = 0;
		CIN = 0;
		curveCAPE.clear ();
		curveCIN.clear ();
		return;
	}
	//-----------------------------------------------
	// Compute CCL  (continues to follow mixing ratio)
	temp = getTempCByAlt (p);
	while (tmixr < temp && p>hpaMin()-1)
	{
		p = p - deltap;
		temp = getTempCByAlt (p);
		tmixr = Therm::tempFromMixingRatio (mixr, p);
	}
	if (tmixr >= temp)
		CCL = TPoint (temp, p);
	//--------------------------------------------------------
	// Compute LFC (follows saturated curve from LCL) and continue CIN
	TPCurve curveSaturatedFromLCL;
	Therm::curveSaturatedAdiabatic (&curveSaturatedFromLCL, LCL, hpaMin()-1, -deltap);
	//DBGN (curveSaturatedFromLCL.points.size());
	//-----------------------------------------------
	z0 = Therm::hpa2m (LCL.hpa);
	int i;
	// continue to append points to curve1 and curve2
	for (i=0; i<curveSaturatedFromLCL.points.size() && !LFC.ok(); i++) 
	{
		TPoint tp = curveSaturatedFromLCL.points [i];
		double temp = getTempCByAlt (tp.hpa);
		if (tp.tempC!=GRIB_NOTDEF && temp!=GRIB_NOTDEF && tp.tempC > temp) {
			LFC = tp;
			//DBG ("found LFC : %g", tp.hpa);
		}
		// CIN
		if (tp.tempC < temp)
		{
			z1 = Therm::hpa2m (tp.hpa);
			dz = z1 - z0;
			z0 = z1;
			double tv = Therm::virtualTemperatureC  (temp, tp.hpa);
			double tpv = Therm::virtualTemperatureC  (tp.tempC, tp.hpa);
			CIN += Therm::gByAlt(z0)*(dz*(tpv - tv)/(tv+273.15));
			curve1.append (TPoint(temp,tp.hpa));
			curve2.insert (0, TPoint(tp.tempC,tp.hpa));
		}
		else
		{
			curve1.append (TPoint(tp.tempC,tp.hpa));
			curve2.insert (0, TPoint(tp.tempC,tp.hpa));
		}
	}
	curveCIN = curve1 + curve2;
	//-----------------------------------------
	int indCurveLFC = i;
	int indCurveEL = i;
	//-----------------------------------------
	// Compute EL (continues to follow saturated curve from LFC)
	for ( ; i<curveSaturatedFromLCL.points.size(); i++) 
	{
		TPoint tp = curveSaturatedFromLCL.points [i];
		//DBG("hpa=%.2f temp=%.2f T=%.2f", tp.hpa, tp.tempC, getTempCByAlt (tp.hpa));
		double temp = getTempCByAlt (tp.hpa);
		if (tp.tempC!=GRIB_NOTDEF && temp!=GRIB_NOTDEF) 
		{
			if (tp.tempC < temp) {
				if (! EL.ok()) 	{
					EL = tp;
					indCurveEL = i;
				}
			}
			else {
				EL =  TPoint (GRIB_NOTDEF, GRIB_NOTDEF);
			}
		}
	}
	if (!EL.ok() && curveSaturatedFromLCL.points.size()>0) {
		// simulated EL at maximal height (FIXME)
		// calculate maximal height, on sounding temp curve or on adiabat
		double pmin = hpaMin ();
		i = curveSaturatedFromLCL.points.size()-1;
		while (i>=0 && curveSaturatedFromLCL.points [i].hpa < pmin) {
			i --;
		}
		if (i>= 0) {
			TPoint tp = curveSaturatedFromLCL.points [i];
			temp = getTempCByAlt (tp.hpa);
			if (tp.tempC > temp) {
				EL = TPoint (temp,tp.hpa);
				indCurveEL = i;
			}
		}
	}
	//-----------------------------------------
	if (!LFC.ok() || !EL.ok())
	{
		CAPE = 0;
		CIN = 0;
		curveCAPE.clear ();
		curveCIN.clear ();
	}
	//--------------------------------------------------------
	// Compute CAPE (follows saturated curve from LFC to EL)
	// Create path arround the area
	//--------------------------------------------------------
	curve1.clear ();
	curve2.clear ();
	CAPE = 0;
	if (LFC.ok() && EL.ok())
	{
		z0 = Therm::hpa2m (LFC.hpa);
		for (i=indCurveLFC ; i<=indCurveEL; i++) 
		{
			TPoint tp = curveSaturatedFromLCL.points [i];
			//DBG("hpa=%.2f temp=%.2f T=%.2f", tp.hpa, tp.tempC, getTempCByAlt (tp.hpa));
			double temp = getTempCByAlt (tp.hpa);
			if (tp.ok() && temp!=GRIB_NOTDEF) 
			{
				if (tp.tempC > temp) {
					double tv = Therm::virtualTemperatureC  (temp, tp.hpa);
					double tpv = Therm::virtualTemperatureC  (tp.tempC, tp.hpa);
					z1 = Therm::hpa2m (tp.hpa);
					dz = z1-z0;
					z0 = z1;
					CAPE += Therm::gByAlt(z0)*(dz*(tpv - tv)/(tv+273.15));
					curve1.append (tp);
					curve2.insert (0, TPoint(temp, tp.hpa));
				}
				else {
					curve1.append (tp); // null width area
					curve2.insert (0, tp);
				}
			}
		}
		curveCAPE = curve1 + curve2;
	}
	//-----------------------------------------
	double t500 = getTempCByAlt (500);
	double t700 = getTempCByAlt (700);
	double t850 = getTempCByAlt (850);
	double dp700 = getDewpCByAlt (700);
	double dp850 = getDewpCByAlt (850);
	//-----------------------------------------
	// KI (K-Index)
	KI = t850 - t500 + dp850 - (t700-dp700);
	//-----------------------------------------
	// TT (Total Totals Index)
	TT = t850 + dp850 - 2*t500;
	//-----------------------------------------
	// LI (follows saturated curve from LCL to 500hpa)
	LI = GRIB_NOTDEF;
	if (LCL.ok()) {
		for (i=0; i<curveSaturatedFromLCL.points.size() && LI==GRIB_NOTDEF; i++) 
		{
			TPoint tp = curveSaturatedFromLCL.points [i];
			if (tp.hpa <= 500) {
				if (t500 != GRIB_NOTDEF) {
					LI =  t500 - tp.tempC;
				}
			} 
		}
	}
	//-----------------------------------------
	// SI Showalter Index (follows adiabatic from 850 to 500hpa)
	SI = GRIB_NOTDEF;
	TPoint lcl850 = compute_LCL (850, getTempCByAlt (850), getDewpCByAlt (850), deltap);
	if (lcl850.ok()) {
		TPCurve curveSaturated;
		Therm::curveSaturatedAdiabatic (&curveSaturated, lcl850, 450, -deltap);
		for (i=0; i<curveSaturated.points.size() && SI==GRIB_NOTDEF; i++) 
		{
			TPoint tp = curveSaturated.points [i];
			if (tp.hpa <= 500) {
				if (t500 != GRIB_NOTDEF) {
					SI =  t500 - tp.tempC;
				}
			}
		}
	}
	//-----------------------------------------
	// SWEAT Index
	SWEAT = GRIB_NOTDEF;
	SoundingPointWind W850 = getWindByAlt (850);
	SoundingPointWind W500 = getWindByAlt (500);
	if (W850.ok() && W500.ok()) {
		SWEAT = 0;
		if (dp850>0)
			SWEAT += 12*dp850;
		if (TT > 49)
			SWEAT += 20*(TT-49);
		double d850 = W850.degrees();
		double d500 = W500.degrees();
		double v850 = W850.speedKts();
		double v500 = W500.speedKts();
		SWEAT += 2*v850 + v500;
		double s;
		if (       (d850>=130 && d850<=250)
				|| (d500>=210 && d500<=310)
				|| (d500>d850)
				|| (v850>=15 && v500>=15)
			)
			s = 0;
		else 
			s = 125*(sin((d500-d850)*M_PI/180)+0.2);
		SWEAT += s;
		SWEAT = qRound (SWEAT);
	}
	
}








