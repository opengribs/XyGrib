/*
 *  BoatSpeed.cpp
 *  zyGrib
 *
 *  Created by Tim Holtschneider on 29.05.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#include "zuFile.h"
#include "BoatSpeed.h"

#include <QFile>

//------------------------------------------------------------------
// Constructor
// build up wind boat speed relation
BoatSpeed::BoatSpeed( QString fileName ) {
	qhWindBoatCard.clear();	
	loadBoatParams( fileName );
}
//------------------------------------------------------------------
// Deconstructor
/*BoatSpeed::~BoatSpeed() {
	delete this;
}*/
//------------------------------------------------------------------
// get quantisized wind degree steps for boat speed parameters
int BoatSpeed::getDegSteps( double dTWindDir, double dBoatDir ) {
	double dDir;
	int iKeyTmp = 0;
	
	// 360 degree periodicity
	// dWDir = (dTWindDir + 180.0) % 360.0;
	dDir = dTWindDir - dBoatDir;
	if( dDir < 0 ) {
		dDir = 360 + dDir;
	}

	QHashIterator<int, tyWindBoatSpeed> iterHash(qhWindBoatCard);
	while (iterHash.hasNext())
	{
		iterHash.next();
		iKeyTmp =iterHash.key();
		if( dDir < iterHash.key() )
			break;
	}
	return iKeyTmp;
}
//------------------------------------------------------------------
// get boat speed by true wind parameters for given boat parameters
double BoatSpeed::getBoatSpeed( double dTWindDir, double dTWindSpeed, double dBoatDir ) {
	tyWindBoatSpeed ptLastMin={0,0};
	tyWindBoatSpeed ptMin={0,0};
	double dDiffSpeed, dBoatSpeed=0;
	double dDiffSpeedMin = 0xFFFFFFFF;
		
	// retrieve quantizied wind direction
	int iWindStep = getDegSteps( dTWindDir, dBoatDir );
//	DBG("BoatSpeedParams (WD/BD/WStep/WS): %f %f %f %i", dTWindDir, dBoatDir, iWindStep, dTWindSpeed );
	if( qhWindBoatCard.contains( iWindStep )) {
		
		// get closest bases for interpolation
		QList<tyWindBoatSpeed> qlWindSpeeds = qhWindBoatCard.values( iWindStep );
		QList<tyWindBoatSpeed>::iterator iter;
		for( iter=qlWindSpeeds.begin(); iter!=qlWindSpeeds.end(); iter++ ) {
			dDiffSpeed = getWindKnots(dTWindSpeed)  - (iter->dWindSpeed);			
			if (dDiffSpeed < 0) {
				dDiffSpeed *= -1;
			}
			// find record for given wind speed
//	DBG("Interpolation WSpeed: %f %f %f", dTWindSpeed, iter->dWindSpeed, dDiffSpeed );
			if( dDiffSpeedMin > dDiffSpeed ) {
				dDiffSpeedMin = dDiffSpeed;
				// set closest bases
				ptLastMin.dWindSpeed = ptMin.dWindSpeed;
				ptMin.dWindSpeed = iter->dWindSpeed;
				ptLastMin.dBoatSpeed = ptMin.dBoatSpeed;
				ptMin.dBoatSpeed = iter->dBoatSpeed;
			}
			else
			{
				break;
			}
		}
		// to be implemented: check if direction was calculated in previous step
		// then just return member fBoatSpeed of last evaluation
//	DBG("Boat ptMin(W/B) ptLastMin(W/B): %f %f %f %f %f", dTWindSpeed ,ptMin.dBoatSpeed, ptMin.dWindSpeed, ptLastMin.dBoatSpeed, ptLastMin.dWindSpeed );
		dBoatSpeed = getInterpolatedSpeed( getWindKnots(dTWindSpeed) , ptMin, ptLastMin );		
	} else {
		dBoatSpeed = 0.;
	}
	return dBoatSpeed;
}
//------------------------------------------------------------------
// get apparent wind speed by true wind parameters for given boat parameters
//float BoatSpeed::getAppWindSpeed( float fBoatDir, float fTWindDir, float fTWindSpeed ) {
//}
//------------------------------------------------------------------
// get apparent wind direction by true wind parameters for given boat parameters
//float BoatSpeed::getAppWindDir( float fBoatDir, float fTWindDir, float fTWindSpeed ) {
//}
//------------------------------------------------------------------
// get interpolattion of boat speed
// currently linear interpolation is implemented
double BoatSpeed::getInterpolatedSpeed( double dWindSpeed, tyWindBoatSpeed ptFst, tyWindBoatSpeed ptLast )
{
	double dDelta, dOffset;
	
	dDelta = (ptFst.dBoatSpeed - ptLast.dBoatSpeed) / (ptFst.dWindSpeed - ptLast.dWindSpeed);
	dOffset = ptLast.dBoatSpeed - dDelta * ptLast.dWindSpeed;
	
	return (dDelta * dWindSpeed + dOffset);
}

//---------------------------------------------------------------------
// load boat wind parameters
//---------------------------------------------------------------------
void BoatSpeed::loadBoatParams( QString fileName )
{
	char *myLine;
    long lLineMax = 10000000;
    myLine = new char[lLineMax];
	

	if( QFile::exists(fileName) )
	{
		ZUFILE *flBoatParams = zu_open( qPrintable(fileName), "r" );
		
		if( flBoatParams != NULL ) {
			long lSize = zu_read( flBoatParams, myLine, lLineMax );
					
			QByteArray barr( myLine, lSize );
			QList<QByteArray> blist = barr.split('\n');
			QByteArray bline = blist.at(0);
			
			// first line holds true wind speeds			
			QList<QByteArray> baWind = bline.split(';');

			// get boat speed at true wind directions
			for (int i=1; i < blist.size(); i++)
			{
				bline = blist.at(i);
				QList<QByteArray> baBoatSpeed = bline.split(';');
				
				for( int j=1; j < baBoatSpeed.size(); j++ ) {
					tyWindBoatSpeed myStruct;
					myStruct.dWindSpeed = baWind.at(j).toDouble();
					myStruct.dBoatSpeed = baBoatSpeed.at(j).toDouble();
					qhWindBoatCard.insertMulti( baBoatSpeed.at(0).toUInt(), myStruct );
				}
			}
		}
		zu_close( flBoatParams );
	}
	delete myLine;
}

//----------------------------------------------------------------------------
// convert wind speed (m/s) into knots, as boat parameters are given in knots
//----------------------------------------------------------------------------
double BoatSpeed::getWindKnots( double &dTWindSpeed ) 
{
	return 3.6 / ZY_MILE * dTWindSpeed; 
}
