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

// Analyse Meteoblue data file

#include <iostream>
#include <list>
#include <cmath>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MblueRecord.h"
#include "Util.h"
#include "DataQString.h" 

int comptepts = 0;
int comptedestroy = 0;

//===============================================================================
MbluePoint::MbluePoint () 
{
	ok = false;
}
//--------------------------------------------------------------------------------
MbluePoint::~MbluePoint () 
{
/*comptedestroy ++;
if (comptedestroy%1000 == 0) DBG("destroy %d", comptedestroy);*/
	mapvalues.clear();
}
//--------------------------------------------------------------------------------
MbluePoint::MbluePoint (
				const std::vector <uint32_t> datacodes, 
				const MbzLine *mbzline)
{
// comptepts ++; 
// if (comptepts%1000 == 0) DBG("nb=%d", comptepts);
// mbzline->print();

	ok = true;
	// position
	x = mbzline->x;
	y = mbzline->y;
	if (x<-360 || x>360 || y<-90 || y>90)
		ok = false;
	// data
	//DBGS ("=======================");
	for (size_t i=0; i<datacodes.size(); i++) 
	{
		if (i < mbzline->data.size()) 
		{
			uint32_t code = datacodes [i];
			float    val  = mbzline->data [i];
			DataCode dtc (code);
			//DBG ("%s : %f", qPrintable(DataCodeStr::toString(dtc)), val );
			//DBGN (dtc.dataType);
			
			// adjust some values
			switch (dtc.dataType) {
				case GRB_TEMP:
				case GRB_TEMP_POT:
				case GRB_TMAX:
				case GRB_TMIN:
				case GRB_DEWPOINT:
					val += 273.15;	// kelvin
					break;
				case GRB_HUMID_REL:
				case GRB_CLOUD_TOT:
					if (val<0 || val>100)
							val = GRIB_NOTDEF;
					break;
				case GRB_PRESSURE_MSL:
					if (val<84000 || val>112000)
							val = GRIB_NOTDEF;
					break;
				case GRB_WIND_GUST:
					code = DataCode(GRB_WIND_GUST,LV_GND_SURF,0).toInt32();
					break;
			}
			mapvalues.insert ( std::pair<uint32_t, float>(code,val) );
		}
		else
			ok = false;
	}
	
// DBGN ( getValue(DataCode(GRB_PRESSURE_MSL,LV_MSL,0)) );
	//------------------------------------
	// Computed data
	//------------------------------------
	// dewpoint
	if (ok 
		&& getValue (DataCode(GRB_DEWPOINT,LV_ABOV_GND,2)) == GRIB_NOTDEF) 
	{
		float t  = getValue (DataCode(GRB_TEMP,LV_ABOV_GND,2));
		float rh = getValue (DataCode(GRB_HUMID_REL,LV_ABOV_GND,2));
		if (t != GRIB_NOTDEF && rh != GRIB_NOTDEF) {
			float val = DataRecordAbstract::dewpointHardy (t, rh);
			DataCode dtcd (GRB_DEWPOINT,LV_ABOV_GND,2);
			if (val != GRIB_NOTDEF) {
				mapvalues.insert ( std::pair<uint32_t, float>(dtcd.toInt32(),val) );
			}
		}
	}
	// total cloud cover
	if (ok  
		&& getValue (DataCode(GRB_CLOUD_TOT,LV_ATMOS_ALL,0)) == GRIB_NOTDEF)
	{
		float ch = getValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_HIG_LAYER,0));
		float cm = getValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_MID_LAYER,0));
		float cl = getValue (DataCode(GRB_CLOUD_TOT,LV_CLOUD_LOW_LAYER,0));
		
		if (ch != GRIB_NOTDEF && cm != GRIB_NOTDEF && cl != GRIB_NOTDEF) {
			float val  = 100 * (1 - (1-ch/100)*(1-cm/100)*(1-cl/100));
			if (val>100) val=100;
			else if (val<0) val=0;
			DataCode dtcd (GRB_CLOUD_TOT,LV_ATMOS_ALL,0);
			if (val != GRIB_NOTDEF) {
				mapvalues.insert ( std::pair<uint32_t, float>(dtcd.toInt32(),val) );
			}
		}
	}
}

//--------------------------------------------------------------------------------
MbluePoint::MbluePoint ( double px, double py,
					 int  nbPoints,
					 MbluePoint *p1,
					 MbluePoint *p2,
					 MbluePoint *p3,
					 MbluePoint *p4 )
{
// comptepts ++; 
// if (comptepts%1000 == 0) DBG("nb=%d", comptepts);

	ok = true;
	double d1, d2, d3, d4;
	double v1, v2, v3, v4;
	double k1, k2, k3, k4;
	
	DataCode dtc;
	uint32_t   code;
	float val;
	std::map <uint32_t, float>::iterator iter;
	
	if (nbPoints == 1) {
		ok = false;
		for (iter=p1->mapvalues.begin(); iter!=p1->mapvalues.end(); iter++) {
			code = iter->first;
			dtc = DataCode(code);
			mapvalues.insert ( std::pair<uint32_t, float> (code, GRIB_NOTDEF) );
		}
		return;
	}
	if (nbPoints==4) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			d3 = (px-p3->x)*(px-p3->x)+(py-p3->y)*(py-p3->y);
			d4 = (px-p4->x)*(px-p4->x)+(py-p4->y)*(py-p4->y);
			k1 = 1.0/(d1*d1+1e-12);
			k2 = 1.0/(d2*d2+1e-12);
			k3 = 1.0/(d3*d3+1e-12);
			k4 = 1.0/(d4*d4+1e-12);
			for (iter=p1->mapvalues.begin(); iter!=p1->mapvalues.end(); iter++) {
				dtc = iter->first;
				v1 = p1->getValue (dtc);
				v2 = p2->getValue (dtc);
				v3 = p3->getValue (dtc);
				v4 = p4->getValue (dtc);
				if (v1==GRIB_NOTDEF || v2==GRIB_NOTDEF || v3==GRIB_NOTDEF || v4==GRIB_NOTDEF)
					val = GRIB_NOTDEF;
				else
					val =  (k1*v1 + k2*v2 + k3*v3 + k4*v4)/(k1+k2+k3+k4);
				mapvalues.insert ( std::pair<uint32_t, float>(dtc.toInt32(), val) );
			}
	}
	else if (nbPoints==3) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			d3 = (px-p3->x)*(px-p3->x)+(py-p3->y)*(py-p3->y);
			k1 = 1.0/(d1*d1+1e-12);
			k2 = 1.0/(d2*d2+1e-12);
			k3 = 1.0/(d3*d3+1e-12);
			for (iter=p1->mapvalues.begin(); iter!=p1->mapvalues.end(); iter++) {
				dtc = iter->first;
				v1 = p1->getValue (dtc);
				v2 = p2->getValue (dtc);
				v3 = p3->getValue (dtc);
				if (v1==GRIB_NOTDEF || v2==GRIB_NOTDEF || v3==GRIB_NOTDEF)
					val = GRIB_NOTDEF;
				else
					val =  (k1*v1 + k2*v2 + k3*v3)/(k1+k2+k3);
				mapvalues.insert ( std::pair<uint32_t, float>(dtc.toInt32(), val) );
			}
	}
	else if (nbPoints==2) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			k1 = 1.0/(d1*d1+1e-12);
			k2 = 1.0/(d2*d2+1e-12);
			for (iter=p1->mapvalues.begin(); iter!=p1->mapvalues.end(); iter++) {
				dtc = iter->first;
				v1 = p1->getValue (dtc);
				v2 = p2->getValue (dtc);
				if (v1==GRIB_NOTDEF || v2==GRIB_NOTDEF)
					val = GRIB_NOTDEF;
				else
					val =  (k1*v1 + k2*v2)/(k1+k2);
				mapvalues.insert ( std::pair<uint32_t, float>(dtc.toInt32(), val) );
			}
	}
}
//-------------------------------------------------------------
bool   MbluePoint::hasData  (const DataCode &dtc) const
{
	return (mapvalues.find (dtc.toInt32()) != mapvalues.end());
}	

//-------------------------------------------------------------
double MbluePoint::getValue (const DataCode &dtc) const
{
	std::map <uint32_t, float>::const_iterator iter;
	iter = mapvalues.find (dtc.toInt32());
	if (iter != mapvalues.end())
		return iter->second;
	else
		return GRIB_NOTDEF;
}	



//===========================================================
//===========================================================
//===========================================================
// MblueRecord
//===========================================================
//===========================================================
//===========================================================
MblueRecord::MblueRecord (const MbzFile &mbzfile, 
						  time_t curDate,
						  bool fastInterpolation 
						 )
{
	ok = false;
	this->fastInterpolation = fastInterpolation;
	clusters = NULL;
	regularGrid = NULL;
	smoothPressureGrid = NULL;
	dataCenterModel = METEOBLUE_NMM4;
	Ni = 0;
	Nj = 0;
	if (mbzfile.isOk()) 
	{
		this->refDate = UTC_mktime (mbzfile.year,mbzfile.month,mbzfile.day,mbzfile.href,0,0);
		this->curDate = curDate;
		xmin = mbzfile.xmin;
		xmax = mbzfile.xmax;
		ymin = mbzfile.ymin;
		ymax = mbzfile.ymax;
		ok = true;
	}
}
//--------------------------------------------------------------	
void MblueRecord::finalize ()
{
	if (ok) {
		makeClusters ();
		//DBG("nb points : %d", allPoints.size());
		makeVirtualRegularGrid ();
		makeSmoothPressureGrid ();
// 		if (clusters != NULL) {
// 			delete [] clusters;
// 			clusters = NULL;
// 		}
	}
}
//--------------------------------------------------------------	
void MblueRecord::addMbluePoint (MbluePoint *pt)
{
	allPoints.push_back (pt);
}
//--------------------------------------------------------------	
bool MblueRecord::hasData (const DataCode &dtc) const
{
	if (allPoints.size() == 0)
		return false;
	
	MbluePoint *pt = *(allPoints.begin());
	return pt->hasData (dtc);
	
}
//-----------------------------------------------------
MblueRecord::~MblueRecord ()
{
// DBGS("Destroy MblueRecord");	
	if (clusters != NULL) {
		delete [] clusters;
		clusters = NULL;
	}
	if (regularGrid != NULL) {
		for (int i=0; i<Ni*Nj; i++)
				delete regularGrid [i];
		delete [] regularGrid;
	}
	if (smoothPressureGrid != NULL)
		delete [] smoothPressureGrid;
	
	Util::cleanVectorPointers (allPoints);
}
//-----------------------------------------------------
void MblueRecord::makeSmoothPressureGrid ()
{
	if (!ok)
		return;
	smoothPressureGrid = new float [Ni*Nj];
	assert (smoothPressureGrid);
	int i, j;
	float v0,v1,v2,v3,v4,v5,v6,v7,v8;
	DataCode dtc (GRB_PRESSURE_MSL,LV_MSL,0);

    for (j=1; j<Nj-1; j++) {
        for (i=1; i<Ni-1; i++) {
            v0 = getRawPressureOnRegularGrid (i, j );
            v1 = getRawPressureOnRegularGrid (i-1, j-1 );
            v2 = getRawPressureOnRegularGrid (i  , j-1 );
            v3 = getRawPressureOnRegularGrid (i+1, j-1 );
            v4 = getRawPressureOnRegularGrid (i-1, j );
            v5 = getRawPressureOnRegularGrid (i+1, j );
            v6 = getRawPressureOnRegularGrid (i-1, j+1 );
            v7 = getRawPressureOnRegularGrid (i  , j+1 );
            v8 = getRawPressureOnRegularGrid (i+1, j+1 );
			if (   v0 != GRIB_NOTDEF  && v1 != GRIB_NOTDEF  && v2 != GRIB_NOTDEF
				&& v3 != GRIB_NOTDEF  && v4 != GRIB_NOTDEF  && v5 != GRIB_NOTDEF  
				&& v6 != GRIB_NOTDEF  && v7 != GRIB_NOTDEF  && v8 != GRIB_NOTDEF 
			) {
				smoothPressureGrid [i+j*Ni] = (v0 +v1+v2+v3+v4+v5+v6+v7+v8) / 9.0;
			}
			else {
				smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
			}
		}
	}
    j = 0;
	for (i=1; i<Ni-1; i++) {
		v0 = getRawPressureOnRegularGrid (i, j );
		v4 = getRawPressureOnRegularGrid (i-1, j );
		v5 = getRawPressureOnRegularGrid (i+1, j );
		v6 = getRawPressureOnRegularGrid (i-1, j+1 );
		v7 = getRawPressureOnRegularGrid (i  , j+1 );
		v8 = getRawPressureOnRegularGrid (i+1, j+1 );
		if (v0 != GRIB_NOTDEF  && v4 != GRIB_NOTDEF && v5 != GRIB_NOTDEF  
				&& v6 != GRIB_NOTDEF && v7 != GRIB_NOTDEF  && v8 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v4+v5+v6+v7+v8) / 6.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	}
    j = Nj-1;
	for (i=1; i<Ni-1; i++) {
		v0 = getRawPressureOnRegularGrid (i, j );
		v1 = getRawPressureOnRegularGrid (i-1, j-1 );
		v2 = getRawPressureOnRegularGrid (i  , j-1 );
		v3 = getRawPressureOnRegularGrid (i+1, j-1 );
		v4 = getRawPressureOnRegularGrid (i-1, j );
		v5 = getRawPressureOnRegularGrid (i+1, j );
		if (v0 != GRIB_NOTDEF  && v1 != GRIB_NOTDEF  && v2 != GRIB_NOTDEF
				&& v3 != GRIB_NOTDEF  && v4 != GRIB_NOTDEF  && v5 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v1+v2+v3+v4+v5) / 6.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	}
	i = 0;
    for (j=1; j<Nj-1; j++) {
		v0 = getRawPressureOnRegularGrid (i, j );
		v2 = getRawPressureOnRegularGrid (i  , j-1 );
		v3 = getRawPressureOnRegularGrid (i+1, j-1 );
		v5 = getRawPressureOnRegularGrid (i+1, j );
		v7 = getRawPressureOnRegularGrid (i  , j+1 );
		v8 = getRawPressureOnRegularGrid (i+1, j+1 );
		if (v0 != GRIB_NOTDEF  && v2 != GRIB_NOTDEF  && v3 != GRIB_NOTDEF
				&& v5 != GRIB_NOTDEF  && v7 != GRIB_NOTDEF  && v8 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v2+v3+v5+v7+v8) / 6.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	}
	i = Ni-1;
    for (j=1; j<Nj-1; j++) {
		v0 = getRawPressureOnRegularGrid (i, j );
		v1 = getRawPressureOnRegularGrid (i-1, j-1 );
		v2 = getRawPressureOnRegularGrid (i  , j-1 );
		v4 = getRawPressureOnRegularGrid (i-1, j );
		v6 = getRawPressureOnRegularGrid (i-1, j+1 );
		v7 = getRawPressureOnRegularGrid (i  , j+1 );
		if (v0 != GRIB_NOTDEF  && v1 != GRIB_NOTDEF  && v2 != GRIB_NOTDEF
				&& v4 != GRIB_NOTDEF  && v6 != GRIB_NOTDEF  && v7 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v1+v2+v4+v6+v7) / 6.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	}
	i = 0;
	j = 0;
		v0 = getRawPressureOnRegularGrid (i, j );
		v5 = getRawPressureOnRegularGrid (i+1, j );
		v7 = getRawPressureOnRegularGrid (i  , j+1 );
		v8 = getRawPressureOnRegularGrid (i+1, j+1 );
		if (v0 != GRIB_NOTDEF && v5 != GRIB_NOTDEF && v7 != GRIB_NOTDEF && v8 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v5+v7+v8) / 4.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	i = Ni-1;
	j = 0;
		v0 = getRawPressureOnRegularGrid (i, j );
		v4 = getRawPressureOnRegularGrid (i-1, j );
		v6 = getRawPressureOnRegularGrid (i-1, j+1 );
		v7 = getRawPressureOnRegularGrid (i  , j+1 );
		if (v0 != GRIB_NOTDEF && v4 != GRIB_NOTDEF && v6 != GRIB_NOTDEF && v7 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v4+v6+v7) / 4.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
	i = 0;
	j = Nj-1;
		v0 = getRawPressureOnRegularGrid (i, j );
		v2 = getRawPressureOnRegularGrid (i  , j-1 );
		v3 = getRawPressureOnRegularGrid (i+1, j-1 );
		v5 = getRawPressureOnRegularGrid (i+1, j );
		if (v0 != GRIB_NOTDEF && v2 != GRIB_NOTDEF && v3 != GRIB_NOTDEF && v5 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v2+v3+v5) / 4.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
		
	i = Ni-1;
	j = Nj-1;
		v0 = getRawPressureOnRegularGrid (i, j );
		v1 = getRawPressureOnRegularGrid (i-1, j-1 );
		v2 = getRawPressureOnRegularGrid (i  , j-1 );
		v4 = getRawPressureOnRegularGrid (i-1, j );
		if (v0 != GRIB_NOTDEF && v1 != GRIB_NOTDEF  && v2 != GRIB_NOTDEF && v4 != GRIB_NOTDEF)
			smoothPressureGrid [i+j*Ni] = (v0 +v1+v2+v4) / 4.0;
		else
			smoothPressureGrid [i+j*Ni] = GRIB_NOTDEF;
}
//-----------------------------------------------------
void MblueRecord::makeVirtualRegularGrid ()
{
	if (!ok)
		return;
	double dens = allPoints.size() / ((xmax-xmin)*(ymax-ymin));
	double dt = 1 / sqrt (dens);
// 	dt *= 1.5;		// minimize grid size
	if (dt<0.01)
			dt = 0.01;
	// Virtual grid of size dt x dt
	Ni = (int) ceil ((xmax-xmin)/dt);
	Nj = (int) ceil ((ymax-ymin)/dt);
	if (Ni < 2)
		Ni = 2;
	if (Nj < 2)
		Nj = 2;
	
	Di = (xmax-xmin)/(Ni-1) - 1e-12;
	Dj = (ymax-ymin)/(Nj-1) - 1e-12;
	
// 	DBG("Ni=%d, Nj=%d,  Ni*Nj=%d,   Di=%f Dj=%f ", Ni, Nj, Ni*Nj, Di,Dj);

	regularGrid = new MbluePoint* [Ni*Nj];
	assert (regularGrid);
	
	MbluePoint *p1, *p2, *p3, *p4;
	int  nb;
	double px, py;
	
	for (int i=0; i<Ni; i++) {
		for (int j=0; j<Nj; j++) {
			px = getX(i);
			py = getY(j);
			nb = findNeighbour_clusters (px, py, &p1,&p2,&p3,&p4);
			regularGrid [i+j*Ni] = 
							new MbluePoint (px, py, 
											nb, p1, p2, p3, p4);
		}
	}
}
//-----------------------------------------------------
void MblueRecord::makeClusters ()
{
	int clustersize = 20;	// mean number of points in each cluster
	int nbclusters = allPoints.size()/clustersize;
	double k = (xmax-xmin)/(ymax-ymin);
	clustersNj = (int) (sqrt (nbclusters/k));
	clustersNi = (int) (k*clustersNj);
	if (clustersNi<1)
			clustersNi = 1;
	if (clustersNj<1)
			clustersNj = 1;

	clusterWidth = (xmax-xmin)/clustersNi;
	clusterHight = (ymax-ymin)/clustersNj;
	
// 	DBG("clustersNi=%d   clustersNj=%d  tot=%d", clustersNi,clustersNj, clustersNi*clustersNj)	;
// 	DBG("nb points=%d    per cluster=%.2f", allPoints.size(), (double)allPoints.size()/(clustersNi*clustersNj))	;

	clusters = new std::vector <MbluePoint *> [clustersNi*clustersNj];
	assert (clusters);
	std::vector <MbluePoint *>::iterator it;
	for (it=allPoints.begin(); it !=allPoints.end(); it++)
	{
		MbluePoint *p = *it;
		getCluster (p->x,p->y).push_back(p);
	}
}

//-----------------------------------------------------
std::vector <MbluePoint *> & MblueRecord::getCluster (double x, double y) const
{
	double dx = (xmax-xmin)/clustersNi;
	double dy = (ymax-ymin)/clustersNj;
	int i, j;
	i = (int) floor((x-xmin)/dx);
	j = (int) floor((y-ymin)/dy);
	// avoid rounding errors
	if (i<0) i=0;
	if (j<0) j=0;
	if (i>=clustersNi) i=clustersNi-1;
	if (j>=clustersNj) j=clustersNj-1;
	return clusters [j*clustersNi+i];
}

//-----------------------------------------------------
std::vector <std::vector <MbluePoint *> > * MblueRecord::getClustersList (
					double x, double y) const
{
// 	return getClustersList_9max (x,y);
	return getClustersList_4max (x,y);
}
//-----------------------------------------------------
std::vector <std::vector <MbluePoint *> > * MblueRecord::getClustersList_4max (
					double x, double y) const
{
 	std::vector <std::vector <MbluePoint *> > *listclusters;
	listclusters = new std::vector <std::vector <MbluePoint *> >;
	assert(listclusters);
	listclusters->reserve (4);
	double px = (x-xmin)/clusterWidth;
	double py = (y-ymin)/clusterHight;
	int i, j;
	i = (int) floor (px);
	j = (int) floor (py);
	double dx = px-i;
	double dy = py-j;
	// avoid rounding errors
	if (i<0) i=0;
	else if (i>=clustersNi) i=clustersNi-1;
	if (j<0) j=0;
	else if (j>=clustersNj) j=clustersNj-1;
	//--------------------------------------------------------------
	listclusters->push_back (clusters[j*clustersNi+i]);
	//--------------------------------------------------------------
	double ecmin=0.33;
	double ecmax=1-ecmin;
	if (dx>ecmax && i<clustersNi-1) {
		listclusters->push_back (clusters[j*clustersNi+i+1]);
		if (dy>ecmax && j<clustersNj-1) {
			listclusters->push_back (clusters[(j+1)*clustersNi+i+1]);
		}
		else if (dy<ecmin && j>0) {
			listclusters->push_back (clusters[(j-1)*clustersNi+i+1]);
		}
	}
	else if (dx<ecmin && i>0) {
		listclusters->push_back (clusters[j*clustersNi+i-1]);
		if (dy>ecmax && j<clustersNj-1) {
			listclusters->push_back (clusters[(j+1)*clustersNi+i-1]);
		}
		else if (dy<ecmin && j>0) {
			listclusters->push_back (clusters[(j-1)*clustersNi+i-1]);
		}
	}
	//--------------------------------------------------------------
	if (dy>ecmax && j<clustersNj-1) {
		listclusters->push_back (clusters[(j+1)*clustersNi+i]);
	}
	else if (dy<ecmin && j>0) {
		listclusters->push_back (clusters[(j-1)*clustersNi+i]);
	}
	return listclusters;
}
//-----------------------------------------------------
std::vector <std::vector <MbluePoint *> > * MblueRecord::getClustersList_9max (
					double x, double y) const
{
 	std::vector <std::vector <MbluePoint *> > *listclusters;
	listclusters = new std::vector <std::vector <MbluePoint *> >;
	assert(listclusters);
	listclusters->reserve (9);
	
	int i, j;
	i = (int) floor ((x-xmin)/clusterWidth);
	j = (int) floor ((y-ymin)/clusterHight);
	// avoid rounding errors
	if (i<0) i=0;
	else if (i>=clustersNi) i=clustersNi-1;
	if (j<0) j=0;
	else if (j>=clustersNj) j=clustersNj-1;

	listclusters->push_back (clusters[j*clustersNi+i]);
	if (i>0) 
		listclusters->push_back (clusters[j*clustersNi+(i-1)]);
	if (i<clustersNi-1) 
		listclusters->push_back (clusters[j*clustersNi+(i+1)]);
	if (j>0) 
		listclusters->push_back (clusters[(j-1)*clustersNi+i]);
	if (j<clustersNj-1) 
		listclusters->push_back (clusters[(j+1)*clustersNi+i]);

	if (i>0 && j>0) 
		listclusters->push_back (clusters[(j-1)*clustersNi+(i-1)]);
	if (i<clustersNi-1 && j<clustersNj-1) 
		listclusters->push_back (clusters[(j+1)*clustersNi+(i+1)]);
	if (i>0 && j<clustersNj-1) 
		listclusters->push_back (clusters[(j+1)*clustersNi+(i-1)]);
	if (i<clustersNi-1 && j>0) 
		listclusters->push_back (clusters[(j-1)*clustersNi+(i+1)]);

	return listclusters;
}

//-----------------------------------------------------
int  MblueRecord::findNeighbour_clusters (
					double lon, double lat,
					MbluePoint **p1, MbluePoint **p2, 
					MbluePoint **p3, MbluePoint **p4 ) const
{
	double d1,d2,d3,d4;
	d1 = d2 =  d3 =  d4 = 1e100;
	
	*p1 = *p2 = *p3 = *p4 = NULL;

	std::vector <std::vector <MbluePoint *> > *clusterslist;
	clusterslist = getClustersList (lon,lat);
	std::vector <std::vector <MbluePoint *> >::iterator it; 
	int nb=0;
	if (clusterslist != NULL) 
	{
		for (it=clusterslist->begin(); it!=clusterslist->end(); it++)
		{
			std::vector <MbluePoint *> cluster = *it;
			nb += findBestNeighboursInCluster (
						lon, lat, cluster,
						p1, p2, p3, p4,  
						&d1, &d2, &d3 , &d4 );
		}
		delete clusterslist;
	}
	
	if (nb > 4)
		nb = 4;
	return nb;
}
//-------------------------------------------------------
int MblueRecord::findBestNeighboursInCluster (
						double lon, double lat,
						std::vector <MbluePoint *> cluster,
						MbluePoint **p1, MbluePoint **p2, 
						MbluePoint **p3, MbluePoint **p4, 
						double *d1, double *d2, double *d3, double *d4
			) const
{	
	double d;
	int nb=0;
	std::vector <MbluePoint *>::iterator it;
	for (it=cluster.begin(); it !=cluster.end(); it++)
	{
		MbluePoint *p = *it;
		d = (p->x-lon)*(p->x-lon)+(p->y-lat)*(p->y-lat);
		if (d < *d1) {
			*p4 = *p3;
			*p3 = *p2;
			*p2 = *p1;
			*p1 = p;
			*d4 = *d3;
			*d3 = *d2;
			*d2 = *d1;
			*d1 = d;
			nb ++;
		}
		else if (d < *d2) {
			*p4 = *p3;
			*p3 = *p2;
			*p2 = p;
			*d4 = *d3;
			*d3 = *d2;
			*d2 = d;
			nb ++;
		}
		else if (d < *d3) {
			*p4 = *p3;
			*p3 = p;
			*d4 = *d3;
			*d3 = d;
			nb ++;
		}
		else if (d < *d4) {
			*p4 = p;
			*d4 = d;
			nb ++;
		}
	}
	
	if (nb > 4)
		nb = 4;
	return nb;
}


//--------------------------------------------------------------------
bool MblueRecord::getZoneExtension (double *x0,double *y0, double *x1,double *y1)
{
	*x0 = xmin;
	*y0 = ymin;
	*x1 = xmax;
	*y1 = ymax;
	return ok;
}
//--------------------------------------------------------------------
double  MblueRecord::getInterpolatedValueWithoutGrid (
				DataCode dtc, 
				double px, double py,
				bool interpolateValues) const
{
    if (!ok) {
        return GRIB_NOTDEF;
    }
    if (!isPointInMap(px,py)) {
        px += 360.0;               // tour du monde à droite ?
        if (!isPointInMap(px,py)) {
            px -= 2*360.0;              // tour du monde à gauche ?
            if (!isPointInMap(px,py)) {
                return GRIB_NOTDEF;
            }
        }
    }
	MbluePoint *p1, *p2, *p3, *p4;
	double d1, d2, d3, d4;
	double v1, v2, v3, v4;
	double k1, k2, k3, k4;
	
	int  nb = findNeighbour_clusters (px, py, &p1,&p2,&p3,&p4);
	if (nb < 2)
		return GRIB_NOTDEF;
	
	if (! interpolateValues) {
			return p1->getValue (dtc);
	}
	if (nb==4) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			d3 = (px-p3->x)*(px-p3->x)+(py-p3->y)*(py-p3->y);
			d4 = (px-p4->x)*(px-p4->x)+(py-p4->y)*(py-p4->y);
			k1 = 1.0/(d1+1e-12);
			k2 = 1.0/(d2+1e-12);
			k3 = 1.0/(d3+1e-12);
			k4 = 1.0/(d4+1e-12);
			v1 = p1->getValue (dtc);
			v2 = p2->getValue (dtc);
			v3 = p3->getValue (dtc);
			v4 = p4->getValue (dtc);
			if (v1!=GRIB_NOTDEF && v2!=GRIB_NOTDEF && v3!=GRIB_NOTDEF && v4!=GRIB_NOTDEF)
				return (k1*v1 + k2*v2 + k3*v3 + k4*v4)/(k1+k2+k3+k4);
	}
	else if (nb==3) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			d3 = (px-p3->x)*(px-p3->x)+(py-p3->y)*(py-p3->y);
			k1 = 1.0/(d1+1e-12);
			k2 = 1.0/(d2+1e-12);
			k3 = 1.0/(d3+1e-12);
			v1 = p1->getValue (dtc);
			v2 = p2->getValue (dtc);
			v3 = p3->getValue (dtc);
			if (v1!=GRIB_NOTDEF && v2!=GRIB_NOTDEF && v3!=GRIB_NOTDEF)
				return (k1*v1 + k2*v2 + k3*v3)/(k1+k2+k3);
	}
	else if (nb==2) {
			d1 = (px-p1->x)*(px-p1->x)+(py-p1->y)*(py-p1->y);
			d2 = (px-p2->x)*(px-p2->x)+(py-p2->y)*(py-p2->y);
			k1 = 1.0/(d1+1e-12);
			k2 = 1.0/(d2+1e-12);
			v1 = p1->getValue (dtc);
			v2 = p2->getValue (dtc);
			if (v1!=GRIB_NOTDEF && v2!=GRIB_NOTDEF)
				return (k1*v1 + k2*v2)/(k1+k2);
	}
	return GRIB_NOTDEF;
}

//--------------------------------------------------------------------
double  MblueRecord::getInterpolatedValue (
				DataCode dtc, 
				double px, double py,
				bool interpolate) const
{
	if (fastInterpolation)
			return getInterpolatedValueUsingRegularGrid (
						dtc, 
						px, py, interpolate );
	else
			return getInterpolatedValueWithoutGrid (
						dtc,
						px, py, interpolate );

}

//--------------------------------------------------------------------	
double MblueRecord::getValueOnRegularGrid (DataCode dtc, int i, int j) const
{	
	if (i>=0 && i<Ni && j>=0 && j<Nj) 
	{
		if (dtc.equals (GRB_PRESSURE_MSL, LV_MSL, 0)) {
			return getSmoothPressureMSL (i, j);
		}
		else {
			MbluePoint *pt = regularGrid [i+j*Ni];
			return pt->getValue (dtc); 
		}
	}
	else
		return GRIB_NOTDEF;
} 

//--------------------------------------------------------------------	
double MblueRecord::getRawPressureOnRegularGrid (int i, int j) const
{	
	if (i>=0 && i<Ni && j>=0 && j<Nj) 
	{
		MbluePoint *pt = regularGrid [i+j*Ni];
		return pt->getValue (DataCode(GRB_PRESSURE_MSL,LV_MSL,0)); 
	}
	else
		return GRIB_NOTDEF;
} 

//--------------------------------------------------------------------	
int     MblueRecord::getNi ()  const
{
	return Ni;
}
int     MblueRecord::getNj ()  const
{
	return Nj;
}
double  MblueRecord::getDeltaX ()  const
{
	return Di;
}
double  MblueRecord::getDeltaY ()  const
{
	return Dj;
}
//--------------------------------------------------------------------	
double  MblueRecord::getX (int i)  const
{
	return xmin + i*Di;
}
double  MblueRecord::getY (int j)  const
{
	return ymin + j*Dj;
}

