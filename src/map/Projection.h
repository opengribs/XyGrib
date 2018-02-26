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

#ifndef PROJECTION_H
#define PROJECTION_H
#include <QObject>
#include <cstdio>

#include "proj_api.h"

class Projection : public QObject
{
Q_OBJECT
    public:
        Projection (int w, int h, double cx, double cy, double scale);
        Projection (const Projection &);
        virtual ~Projection () {}

        virtual Projection *clone () = 0;

        virtual void screen2map (int i, int j, double *x, double *y) const = 0;
        virtual void map2screen (double x, double y, int *i, int *j) const = 0;
        bool map2screen_glob (double x, double y, int *i, int *j) const;
		
        virtual void setScale (double sc)  = 0;
        virtual void setScreenSize (int w, int h);
        virtual void setVisibleArea (double x0, double y0, double x1, double y1) = 0;
		virtual void setVisibleAreaEarth () 	{setVisibleArea(-180,-85, 180,85);}
        
        virtual int   getW ()   const   {return W;};    // taille de l'écran
        virtual int   getH ()   const   {return H;};
        virtual double getCX () const   {return CX;};   // centre
        virtual double getCY () const   {return CY;};
        virtual double getScale ()     const  {return scale;};
        virtual double getCoefremp ()  const  {return coefremp;};
		virtual bool  isCylindrical () const	 {return cylindrical;}
        
        // zone visible (longitude/latitude)
        virtual double getXmin () const   {return xmin;};
        virtual double getXmax () const   {return xmax;};
        virtual double getYmin () const   {return ymin;};
        virtual double getYmax () const   {return ymax;};
        virtual void   getVisibleArea (double *x0, double *y0, double *x1, double *y1) const
										 {*x0=xmin; *y0=ymin; *x1=xmax; *y1=ymax;}
        
        virtual bool intersect (double w,double e,double s,double n)  const;
        virtual bool isPointVisible (double x,double y) const;
        
        virtual void setCentralPixel (int i, int j);

		virtual void setMapPointInScreen (double x,double y,  int pi,int pj);
        
        virtual void zoom (double k);
        virtual void move (double dx, double dy);

        enum ProjectionType {
				 PROJ_ZYGRIB,
				 PROJ_MERCATOR,
				 PROJ_UTM,
				 PROJ_CENTRAL_CYL,
				 PROJ_EQU_CYL,		// = Plate Carrée
				 PROJ_MILLER,
				 PROJ_LAMBERT_CONF_CON
		};
		
	signals:
		void projectionUpdated();

    protected:
        int    W, H;     // taille de la fenêtre (pixels)
        double CX, CY;                  // centre de la vue (longitude/latitude)
        
        double xmin,xmax, ymax,ymin;  // fenêtre visible (repère longitude/latitude)
        double scale;       	// échelle courante
        double scalemax;    	// échelle maxi
		double coefremp;		// Coefficient de remplissage (surface_visible/pixels)
        bool   cylindrical;		// projection cylindrique (axes orthonormaux) ?
        
        virtual void updateBoundaries();
	
	private :
		void init(int w, int h, double cx, double cy, double scale);
};

//=========================================================
class Projection_ZYGRIB : public Projection
{
	public :
        Projection_ZYGRIB(int w, int h, double cx, double cy, double scale);
        Projection_ZYGRIB(const Projection_ZYGRIB &);
        
        Projection_ZYGRIB *clone()
        		{ return new Projection_ZYGRIB(*this); }

        virtual void screen2map(int i, int j, double *x, double *y) const;
        virtual void map2screen(double x, double y, int *i, int *j) const;
        
        virtual void setVisibleArea(double x0, double y0, double x1, double y1);
        virtual void setScale(double sc);
	
	private :
        double dscale;	   // rapport scaley/scalex
};

//=========================================================
class Projection_libproj : public Projection
{
	public :
        Projection_libproj(int codeProj, int w, int h, double cx, double cy, double scale);
        Projection_libproj(const Projection_libproj &);
        ~Projection_libproj();

        Projection_libproj *clone()
        		{ return new Projection_libproj(*this); }
	
        virtual void screen2map(int i, int j, double *x, double *y) const;
        virtual void map2screen(double x, double y, int *i, int *j) const;
		
        virtual void setVisibleArea(double x0, double y0, double x1, double y1);
        virtual void setScale(double sc);
		
		void  setProjection(int codeProj);
		int   getProjection()   {return currentProj;}

	private :
		projPJ libProj;
		int  currentProj;
};

//=========================================================
class Projection_EQU_CYL : public Projection_libproj
{
	public :
        Projection_EQU_CYL(int w, int h, double cx, double cy, double scale)
				: Projection_libproj(Projection::PROJ_EQU_CYL, w,h, cx,cy, scale)
						{}
        Projection_EQU_CYL *clone()
        		{ return new Projection_EQU_CYL(*this); }
};
//----------------------------------------------------------
class Projection_CENTRAL_CYL : public Projection_libproj
{
	public :
        Projection_CENTRAL_CYL(int w, int h, double cx, double cy, double scale)
				: Projection_libproj(Projection::PROJ_CENTRAL_CYL, w,h, cx,cy, scale)
						{}
        Projection_CENTRAL_CYL *clone()
        		{ return new Projection_CENTRAL_CYL(*this); }

		void setVisibleAreaEarth() 	{setVisibleArea(-180,-72, 180,72);}
};
//----------------------------------------------------------
class Projection_MERCATOR : public Projection_libproj
{
	public :
        Projection_MERCATOR(int w, int h, double cx, double cy, double scale)
				: Projection_libproj(Projection::PROJ_MERCATOR, w,h, cx,cy, scale)
						{}
        Projection_MERCATOR *clone()
        		{ return new Projection_MERCATOR(*this); }
};
//----------------------------------------------------------
class Projection_MILLER : public Projection_libproj
{
	public :
        Projection_MILLER(int w, int h, double cx, double cy, double scale)
				: Projection_libproj(Projection::PROJ_MILLER, w,h, cx,cy, scale)
						{}
        Projection_MILLER *clone()
        		{ return new Projection_MILLER(*this); }
};


#endif




