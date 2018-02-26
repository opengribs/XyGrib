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

/*************************************
Dessin des données GRIB (avec QT)
*************************************/

#ifndef ISOLINE_H
#define ISOLINE_H

#include <iostream>
#include <cmath>
#include <vector>
#include <set>

#include <QApplication>
#include <QPainter>

#include "GriddedRecord.h"
#include "Projection.h"
#include "Util.h"

// TODO: join segments and draw a spline

//===============================================================
// Elément d'isobare qui passe dans un carré (ab-cd)de la grille.
// a  b
// c  d
// Rejoint l'arête (i,j)-(k,l) à l'arête (m,n)-(o,p) (indices ds la grille GRIB)
class Segment
{
    public:
        Segment ( int I, int J,
                  char c1, char c2, char c3, char c4,
                  GriddedRecord *rec, double value,
				  DataCode dtc,
				  int deltaI, int deltaJ
				);

        int   i,j,  k,l;   // arête 1
        double px1,  py1;   // Coordonées de l'intersection (i,j)-(k,l)
        int m,n, o,p;      // arête 2
        double px2,  py2;   // Coordonées de l'intersection (m,n)-(o,p)

    private:
		int deltaI, deltaJ;
		
        void traduitCode(int I, int J, char c1, int &i, int &j);

        void intersectionAreteGrille ( 
					int i,int j, int k,int l,
					double *x, double *y,
					GriddedRecord *rec, double value,
					DataCode dtc );
};

//===============================================================
class IsoLine
{
    public:
        IsoLine ( DataCode dtc,
				  double val, 
				  GriddedRecord *rec,
				  int deltaI, int deltaJ);  
        ~IsoLine();


        void drawIsoLine (QPainter &pnt, const Projection *proj);

        void drawIsoLineLabels (QPainter &pnt, QColor &couleur, const Projection *proj,
                                int density, int first, double coef, double offset);

        int getNbSegments()     {return trace.size();}

    private:
        double value;
        int    W, H;     // taille de la grille
        const  GriddedRecord *rec;
		DataCode dtc;

        QColor isoLineColor;
        std::vector <Segment *> trace;

        void intersectionAreteGrille (
						int i,int j, int k,int l, double *x, double *y,
                        GriddedRecord *rec);

        //-----------------------------------------------------------------------
        // Génère la liste des segments.
        // Les coordonnées sont les indices dans la grille du GribRecord
        //---------------------------------------------------------
        void extractIsoLine (GriddedRecord *rec, int deltaI, int deltaJ);
};




#endif
