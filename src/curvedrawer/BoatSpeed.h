/*
 *  BoatSpeed.h
 *  zyGrib
 *
 *  Created by Tim Holtschneider on 29.05.10.
 *  Copyright 2010. All rights reserved.
 *
 */
#ifndef BOATSPEED_H
#define BOATSPEED_H

#define	WINDSTEPS	5
#ifndef ZY_MILE
#define ZY_MILE		1.852
#endif

//#include "CurveDrawer.h"

#include <QObject>
#include <QHash>
#include <QList>

struct tyWindBoatSpeed {
	double dWindSpeed;
	double dBoatSpeed;
};

//----------------------------------------------------------------------
class BoatSpeed
{	
	public :
		BoatSpeed( QString );
//		~BoatSpeed();
	
		double getBoatSpeed( double, double, double );
//		float getAppWindSpeed( float fBoatDir, float fTWindDir, float fTWindDir );
//		float getAppWindDir( float fBoatDir, float fTWindDir, float fTWindDir );

	private :
	
	QHash<int, tyWindBoatSpeed> qhWindBoatCard;
	
	int getDegSteps( double, double );
	double getInterpolatedSpeed( double, tyWindBoatSpeed, tyWindBoatSpeed );
	void loadBoatParams( QString );
	double getWindKnots( double & );
	
};

#endif

