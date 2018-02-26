/**********************************************************************
zUGrib: meteorologic GRIB file data viewer
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

#ifndef GshhsREADER_H
#define GshhsREADER_H

#include <cassert>
#include <iostream>
#include <vector>

#include <QImage>
#include <QPainter>


#include "zuFile.h"
#include "Projection.h"
#include "GshhsRangsReader.h"

// GSHHS file format:
//
// int id;			 /* Unique polygon id number, starting at 0 */
// int n;			 /* Number of points in this polygon */
// int flag;			 /* level + version << 8 + greenwich << 16 + source << 24 */
// int west, east, south, north; /* min/max extent in micro-degrees */
// int area;			 /* Area of polygon in 1/10 km^2 */
// 
// Here, level, version, greenwhich, and source are
// level:	1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake
// version:	Set to 4 for GSHHS version 1.4
// greenwich:	1 if Greenwich is crossed
// source:	0 = CIA WDBII, 1 = WVS

//==========================================================
class GshhsPoint {
    public:
        double lon, lat;    // longitude, latitude
        GshhsPoint(double lo, double la) {
            lon = lo;
            lat = la;
        }
};

//==========================================================
// GshhsPolygon  (compatible avec le format .rim de RANGS)
//==========================================================
class GshhsPolygon
{
    public:
        GshhsPolygon() {};
        GshhsPolygon(ZUFILE *file);
        virtual ~GshhsPolygon();
        
        int  getLevel()     {return flag&255;};
        int  isGreenwich()  {return greenwich;};
        int  isAntarctic()  {return antarctic;};
        bool isOk()         {return ok;};
        //----------------------
        int id;				/* Unique polygon id number, starting at 0 */
        int n;				/* Number of points in this polygon */
        int flag;			/* level + version << 8 + greenwich << 16 + source << 24 */
        double west, east, south, north;	/* min/max extent in DEGREES */
        int area;			/* Area of polygon in 1/10 km^2 */
        //----------------------
        std::vector <GshhsPoint *> lsPoints;

    protected:
        ZUFILE *file;
        bool ok;
        bool greenwich, antarctic;
        inline virtual int readInt4();
        inline virtual int readInt2();
};

//==========================================================
// GshhsPolygon_WDB     (entete de type GSHHS récent)
//==========================================================
class GshhsPolygon_WDB : public GshhsPolygon
{
    public:
        GshhsPolygon_WDB(ZUFILE *file);
        virtual ~GshhsPolygon_WDB() {}
    protected:
        inline virtual int readInt4();
        inline virtual int readInt2();
};

//==========================================================
class GshhsReader
{
    public:
        GshhsReader (std::string fpath, int quality);
        GshhsReader (const GshhsReader &model);
        ~GshhsReader();
        
        void setUserPreferredQuality (int quality); // 5 levels: 0=low ... 4=full
        
        void drawBackground (QPainter &pnt, Projection *proj,
                QColor seaColor, QColor backgroundColor);
        void drawContinents (QPainter &pnt, Projection *proj,
                QColor seaColor, QColor landColor);
                
        void drawSeaBorders (QPainter &pnt, Projection *proj);
        void drawBoundaries (QPainter &pnt, Projection *proj);
        void drawRivers (QPainter &pnt, Projection *proj);
        
        bool gshhsFilesExists(int quality);
        
    private:
        int quality, userPreferredQuality;  // 5 levels: 0=low ... 4=full
        int  getQuality()   {return quality;}
        void setQuality (int quality);
		void selectBestQuality (Projection *proj);

        std::string fpath;     // directory containing gshhs files
        
        GshhsRangsReader * gshhsRangsReader;
        bool               isUsingRangsReader;
        
        std::string getNameExtension (int quality);
        std::string getFileName_gshhs (int quality);
        std::string getFileName_boundaries (int quality);
        std::string getFileName_rivers (int quality);
        void		readGshhsFiles ();
        
        //-----------------------------------------------------
        // Listes de polygones
        // Pour chaque type, une liste par niveau de qualité,
        // pour éviter les relectures de fichier (pb mémoire ?)
		bool  isListCreator;
        std::vector <GshhsPolygon*> * lsPoly_level1  [5];
        std::vector <GshhsPolygon*> * lsPoly_level2  [5];
        std::vector <GshhsPolygon*> * lsPoly_level3  [5];
        std::vector <GshhsPolygon*> * lsPoly_level4  [5];
        std::vector <GshhsPolygon*> * lsPoly_boundaries [5];
        std::vector <GshhsPolygon*> * lsPoly_rivers  [5];

        std::vector <GshhsPolygon*> & getList_level(int level);
        std::vector <GshhsPolygon*> & getList_boundaries();
        std::vector <GshhsPolygon*> & getList_rivers();
        //-----------------------------------------------------
                
        int GSHHS_scaledPoints(GshhsPolygon *pol, QPoint *pts, double decx,
                                Projection *proj
        );
        void GsshDrawPolygons(QPainter &pnt, std::vector <GshhsPolygon*> &lst,
                                Projection *proj
        );
        void GsshDrawLines(QPainter &pnt, std::vector <GshhsPolygon*> &lst,
                                Projection *proj, bool isClosed
        );
        void clearLists();
    

};

//-------------------------------------------------
inline int GshhsPolygon::readInt4() {
    unsigned char tab[4];
    if (zu_read(file, tab, 4) != 4) {
        ok = false;
    }
    return ((int)tab[3]<<24)+((int)tab[2]<<16)+((int)tab[1]<<8)+((int)tab[0]);
}

//-------------------------------------------------
inline int GshhsPolygon_WDB::readInt4() {    // pas le même indien
    unsigned char tab[4];
    if (zu_read(file, tab, 4) != 4) {
        ok = false;
    }
    return ((int)tab[0]<<24)+((int)tab[1]<<16)+((int)tab[2]<<8)+((int)tab[3]);
}

//-------------------------------------------------
inline int GshhsPolygon::readInt2() {
    unsigned char tab[2];
    if (zu_read(file, tab, 2) != 2) {
        ok = false;
    }
    return ((int)tab[1]<<8)+((int)tab[0]);
}
//-------------------------------------------------
inline int GshhsPolygon_WDB::readInt2() {
    unsigned char tab[2];
    if (zu_read(file, tab, 2) != 2) {
        ok = false;
    }
    return ((int)tab[0]<<8)+((int)tab[1]);
}




#endif
