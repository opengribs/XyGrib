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
#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cmath>
#include <cassert>

#include <QApplication>
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QNetworkRequest>
#include <QLocale>
#include <QDir>
#include <QHash>

#include "DataDefines.h"

#define DEBUG

#ifdef DEBUG
#define DBG(...) { 	\
    fprintf(stderr,"DBG %s: %d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr,__VA_ARGS__); fprintf(stderr,"\n");			\
  }	
#define DBGN(num) { 	\
    fprintf(stderr,"DBG %s: %d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr,"%g\n", (double)(num));			\
  }	
#define DBGS(string) { 	\
    fprintf(stderr,"DBG %s: %d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr,"%s\n", (string));			\
  }	
#define DBGQS(qstring) { 	\
    fprintf(stderr,"DBG %s: %d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr,"%s\n", qPrintable(qstring));			\
  }	
#define DBGQRect(blabla,r) { 	\
    fprintf(stderr,"DBG %s: %d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr,"%s X0=%g %g   X1=%g %g    size=%g %g\n",qPrintable(blabla),(double)(r.x()), (double)(r.y()) \
					, (double)(r.x()+r.width()), (double)(r.y()+r.height()) \
					, (double)(r.width()), (double)(r.height()) \
					);	\
  }	
#else 
#define DBG(...)
#define DBGN(...)
#define DBGS(...)
#define DBGQS(...)
#endif

class Util : public QObject
{
    Q_OBJECT
    
    public:
	static QString getSaveFileName (QWidget *parent, const QString &caption, 
							const QString &dir, const QString &filter=QString());
	static QString getOpenFileName (QWidget *parent, const QString &caption, 
							const QString &dir, const QString &filter=QString());
		
	static QString pathData ()   {return "./";};
	static QString pathColors () {return pathData()+"data/colors/";}
	static QString pathConfig () {return pathData()+"data/config/";}
	static QString pathFonts ()  {return pathData()+"data/fonts/";}
	static QString pathGis ()    {return pathData()+"data/gis/";}
	static QString pathGshhs ()  {return pathData()+"data/maps/gshhs/";}
	static QString pathStuff ()  {return pathData()+"data/stuff/";}
	
	static QString pathImg (const char *fname) {return pathData()+"data/img/"+fname;}
	static QString pathTr  (QString lang)   {return pathData()+"data/tr/zyGrib_"+lang;}
	static QString getServerName ();

    static void     setSetting (const QString &key, const QVariant &value);
    static QVariant getSetting (const QString &key, const QVariant &defaultValue);
	static bool     isDirWritable (const QDir &dir);
	static void     setApplicationProxy ();
	static QNetworkRequest makeNetworkRequest (QString url,double x0=0,double y0=0,double x1=0,double y1=0);
	
    //-------------------------------------------------
    static QString formatDateLong  (time_t t, bool localTime=true);
    static QString formatDateShort (time_t t);
    static QString formatTime      (time_t t);
    static QString formatDateTimeLong  (time_t t);
    static QString formatDateTimeShort (time_t t);
    
    static QString   formatDayName (time_t t);
    static QString   formatDayName (QDate dt);
    static QDateTime applyTimeZone (time_t t, QString *suffix=NULL);
    static int       getDayNum (time_t t);
	static QString   getSunMoonAlmanac (time_t t, double lon, double lat);
    //-------------------------------------------------
    static QString formatDegres    (float x,bool inf100=false);   // 123.4 -> 123°24.00'
    static QString formatPosition  (float x, float y);    // 123°24.00'W 45°67.89'N
    static QString formatLongitude (float x);
    static QString formatLatitude  (float y);
	
    static float   convertTemperature (float tempKelvin);
    static QString formatTemperature (float tempKelvin, bool withUnit=true);
    static QString formatTemperature_short (float tempKelvin, bool withUnit=true);
	
    static QString formatGeopotAltitude  (float meter, bool withUnit=true);
    static QString formatDirection (float angle, bool withUnit=true);
    static QString formatDistance (float mille, bool withUnit=true);
    static QString formatPercentValue (float v, bool withUnit=true);
    static QString formatSnowDepth (float meter, bool withUnit=true);
    static QString formatWhiteCap (float prb, bool withUnit=true);
    static QString formatRain (float mmh, bool withUnit=true);
    static QString formatPressure (float pasc, bool withUnit=true, int precision=1);
    static QString formatCAPEsfc (float jkg, bool withUnit=true);
	
    static QString formatSpeed (float meterspersecond, bool withUnit, QString unit="kts");
    static QString formatSpeed_Wind (float meterspersecond, bool withUnit=true);
    static QString formatSpeed_Current (float meterspersecond, bool withUnit=true);
	
    static QString formatWaveHeight (float meter,  bool withUnit=true);
    static QString formatWavePeriod (float second, bool withUnit=true);
    static QString formatWaveDirection (float angle, bool withUnit=true);
	
    static QString formatCategoricalData (float v) { return (v<0.5) ? "0" : "1"; }

    static double  getDataCoef (const DataCode &dtc);
    static QString getDataUnit (const DataCode &dtc);
    static QString getDataUnit (int dataType);
	static QByteArray sha1 (const QByteArray &v);
	static QString encode (const QString &v, const QString &k="");
	static QString decode (const QString &v, const QString &k="");

    static int    kmhToBeaufort   (float v);
    static float  kmhToBeaufort_F (float v);
    static float  BeaufortToKmh_F (float bf);
    static int    msToBeaufort   (float v);
    static float  msToBeaufort_F (float v);
    static float  BeaufortToMs_F (float bf);	

    static double distancePointSegment (double a,double b,     // point
								  double x0, double y0,  // segment start
								  double x1, double y1   // segment end
								);
	static double roundMultiple (double x, double mul=1.0);
	static void dumpchars (unsigned char *buf, size_t offset, size_t len);

	//---------------------------------------------------------------
	static time_t timestampSetHour (time_t t, int hour, int min, int sec);

	//----------------------------------------------------------------
	bool veryClose (double x, double y, double eps=1.e-8)  {return fabs(x-y) < eps;}

    //-------------------------------------------------
    template <typename T>
        static void orderMinMax(T &a, T &b)
                    { if (a>b) {T c; c=a; a=b; b=c;} }
    
    //-------------------------------------------------
    template <typename T>
        static void orderMaxMin(T &a, T &b)
                    { if (a<b) {T c; c=a; a=b; b=c;} }
    
    //-------------------------------------------------
    template <typename T>
        static bool isInRange(T v, T min, T max)
                    { return (v>=min && v<=max); }
                    
        static bool isInRange(double v, double min, double max)
                    { return (v>=min && v<=max); }
    
    //-------------------------------------------------
    template <typename T>
        static T inRange(T v, T min, T max)
                    {
                        if (v<=min) return min;
                        else if (v>=max) return max;
                        else return v;
                    }
    
    //--------------------------------------------------------
    template <typename T>
        static void cleanListPointers (std::list <T*> & ls)
        {
            typename std::list <T*>::iterator it;
            for (it=ls.begin(); it!=ls.end(); it++) {
				if (*it != NULL) {
					delete *it;
					*it = NULL;
				}
            }
            ls.clear();
        }
    //--------------------------------------------------------
    template <typename K, typename T>
        static void cleanMapPointers (std::map <K, T*> & ls)
        {
            typename std::map <K, T*>::iterator it;
            for (it=ls.begin(); it!=ls.end(); it++) {
				if (it->second != NULL) {
					delete it->second;
					it->second = NULL;
				}
            }
            ls.clear();
        }
    //--------------------------------------------------------
	template <typename T>
		static void cleanVectorPointers (std::vector<T *> & ls) {
			typename std::vector<T *>::iterator it;
			for (it=ls.begin(); it!=ls.end(); it++) {
				if (*it != NULL) {
					delete *it;
					*it = NULL;
				}
			}
			ls.clear();
		}

	private :
		static void validArea (QNetworkRequest &request,double x0,double y0,double x1,double y1);
};
class QStrinq {
	public :
		QStringList split(QString sep) {return QString(data).split(sep);}
		void clear() {data.clear();}
		QByteArray data;
		void append(QByteArray in);
};
//======================================================================
inline int Util::kmhToBeaufort (float v) {
    return (int)(kmhToBeaufort_F(v)+0.5);
}
//-----------------------------------------------------------------------------
inline float Util::kmhToBeaufort_F (float v) {
    float bf = pow (v*v/9.0 , 0.33333);
    if (bf > 12.0)
        bf = 12.0;
    return bf;
}
inline float Util::BeaufortToKmh_F (float bf) {
    return sqrt (bf*bf*bf*9.0);
}
//-----------------------------------------------------------------------------
inline int Util::msToBeaufort (float v) {
    return (int)(msToBeaufort_F(v)+0.5);
}
inline float Util::msToBeaufort_F (float v) {
    float bf = pow (v*v*1.44 , 0.33333);
    if (bf > 12.0)
        bf = 12.0;
    return bf;
}
inline float Util::BeaufortToMs_F (float bf) {
    return sqrt (bf*bf*bf/1.44);
}
//-----------------------------------------------------------------------------


#endif
