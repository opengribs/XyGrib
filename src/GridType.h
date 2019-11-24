#ifndef GRIDTYPE_H
#define GRIDTYPE_H

#include <cmath>

class GridType
{
public:

    virtual void lonLat2XY(double lon, double lat, double &x, double &y) const = 0;

    virtual void XY2LonLat(double x, double y, double &lon, double &lat) const = 0;

    virtual int getNx() const = 0;
    virtual int getNy() const = 0;

    virtual double rotGrid2Earth(int x, int y) const = 0;

protected:
    double rescale_lon(double lon) const {
        double new_lon = lon;

        if ( new_lon < -180.0 )  {
            while ( new_lon < -180.0 )  new_lon += 360.0;
        }
        if ( new_lon > 180.0 )  {
            while ( new_lon >  180.0 )  new_lon -= 360.0;
        }
        return new_lon;
    }
};

/* --------------------------------------------------
 */
class PlateCarree : public GridType {
public:
    PlateCarree() = default;
    PlateCarree(int nx, int ny, double lon, double lat, double dlon, double dlat)
        : Nx(nx), Ny(ny), lon_ll_deg(lon), lat_ll_deg(lat),
          delta_lon_deg(dlon), delta_lat_deg(dlat)
        {}

    virtual ~PlateCarree()  = default;

    int getNx() const override { return Nx;}
    int getNy() const override { return Ny;}

    void lonLat2XY(double lon, double lat, double &x, double &y) const override {
        x = (lon - lon_ll_deg)/delta_lon_deg;
        y = (lat - lat_ll_deg)/delta_lat_deg;
    }

    void XY2LonLat(double x, double y, double &lon, double &lat) const override {
        lon = lon_ll_deg + delta_lon_deg*x;
        lat = lat_ll_deg + delta_lat_deg*y;
    }

    double rotGrid2Earth(int x, int y) const override { return 0.;}

private:
    int Nx{0};
    int Ny{0};
    double lon_ll_deg{0.0};
    double lat_ll_deg{0.0};
    double delta_lon_deg{0.0};
    double delta_lat_deg{0.0};
};

/* --------------------------------------------------
 */
class Mercator : public GridType {
public:
    Mercator() = default;
    Mercator(int nx, int ny, double lo, double la, double la2, double dlon, double dlat)
        : Nx(nx), Ny(ny), lon_ll_deg(lo), lat_ll_deg(la),
          delta_lon_deg(dlon), delta_lat_deg(dlat)
        {
           s = log(tan((45 +lat_ll_deg/2)*M_PI/180));
           double n = log(tan((45 +la2/2)*M_PI/180));
           delta_lat_deg = (n - s) / (ny - 1);
        }

    virtual ~Mercator() = default;

    int getNx() const override { return Nx;}
    int getNy() const override { return Ny;}

    void lonLat2XY(double lon, double lat, double &x, double &y) const override {
        x = (lon - lon_ll_deg)/delta_lon_deg;
        double Y = log( tan(M_PI/4. +lat*M_PI/180/2.) );
        y =  (Y -s)/delta_lat_deg;
    }

    void XY2LonLat(double x, double y, double &lon, double &lat) const override {
        lon = lon_ll_deg + delta_lon_deg*x;
        double Y = s+y*delta_lat_deg;
        lat = 2*atan(exp(Y))*180/M_PI -90.;
    }

    double rotGrid2Earth(int x, int y) const override { return 0.;}

private:
    int Nx{0};
    int Ny{0};
    double lon_ll_deg{0.0};
    double lat_ll_deg{0.0};
    double delta_lon_deg{0.0};
    double delta_lat_deg{0.0};
    double s;
};

/* --------------------------------------------------
*/
class Lambert : public GridType {
public:
    Lambert() = default;

    Lambert(int nx, int ny, double lo, double la, double dlat, 
                double latin1, double latin2, double lov)
      : Nx(nx), Ny(ny), lon_ll_deg(lo), lat_ll_deg(la), Delta_km(dlat)
    {
        Lon0_radians = -rescale_lon(lon_ll_deg)*M_PI/180;
        reduce_rad(Lon0_radians);

        Phi0_radians = (lat_ll_deg)*M_PI/180;

        Phi1_radians = latin1*M_PI/180;
        Phi2_radians = latin2*M_PI/180;

        Lon_cen_radians = -rescale_lon(lov)*M_PI/180;
        reduce_rad(Lon_cen_radians);
        //Delta_km = data.d_km;
        //Radius_km = data.r_km;

        if ( fabs(Phi1_radians - Phi2_radians) < 1.0e-5 ) {
            cone = sin(Phi1_radians);
        }
        else {
            double t, b;

            t = cos(Phi1_radians)/cos(Phi2_radians);
            b = tan(M_PI/4 - 0.5*Phi1_radians)/tan(M_PI/4 - 0.5*Phi2_radians);
            cone = log(t)/log(b);
        }

        alpha = (-1.0/der_func(Phi1_radians))*(Radius_km/Delta_km);

        double r0, theta0;
        r0 = func(Phi0_radians);
        theta0 = cone*(Lon_cen_radians - Lon0_radians);

        Bx = -alpha*r0*sin(theta0);
        By =  alpha*r0*cos(theta0);
    }

    virtual ~Lambert() = default;

    int getNx() const override { return Nx;}
    int getNy() const override { return Ny;}

    void lonLat2XY(double lon, double lat, double &x, double &y) const override {
        double lat_rad, lon_rad;
        double r, theta;

        lat_rad = lat*M_PI/180;
        lon_rad = -rescale_lon(lon)*M_PI/180;
        reduce_rad(lon_rad);
        r = func(lat_rad);
        theta = cone*(Lon_cen_radians - lon_rad);
        x = Bx + alpha*r*sin(theta);
        y = By - alpha*r*cos(theta);
    }

    void XY2LonLat(double x, double y, double &lon, double &lat) const override {
        double lat_rad, lon_rad;
        double r, theta;

        x = (x - Bx)/alpha;
        y = (y - By)/alpha;

        r = sqrt( x*x + y*y );
        lat_rad = inv_func(r);

        lat = lat_rad * 180/M_PI;
        if ( fabs(r) < 1.0e-5 )  
            theta = 0.0;
        else
            theta = atan2(x, -y);

        lon_rad = Lon_cen_radians - theta/cone;
        reduce_rad(lon_rad);
        lon = 360. -lon_rad * 180/M_PI;
    }

    double rotGrid2Earth(int x, int y) const override { 
        double lat_deg, lon_deg, angle;
        double diff, hemi;

        XY2LonLat((double) x, (double) y, lon_deg, lat_deg);
        diff = Lon_cen_radians*M_PI/180 - lon_deg;

        if (Phi1_radians < 0.0) 
            hemi = -1.0;
        else
            hemi = 1.0;

        angle = diff*cone*hemi;
        return angle;
    }

    void uv_to_xy(double u, double v, double &x, double &y) const {
        x = alpha*u + Bx;
        y = -alpha*v + By;
    }

    void xy_to_uv(double x, double y, double &u, double &v) const {
        u = (x - Bx)/alpha;
        v = (y - By)/(-alpha);
    }

private:
     void reduce_rad(double &angle_rad) const {
         angle_rad = angle_rad - 2*M_PI*floor( (angle_rad/(2*M_PI)) + 0.5 );
    }

    double func(double lat_rad) const {
        double r;
        r = tan(M_PI/4 -0.5*lat_rad);
        r = pow(r, cone);
        return r;
    }

    double inv_func(double r) const {
        return M_PI/2 - 2.0*atan(pow(r, 1.0/cone));
    }

    double der_func(double lat_rad) const {
        return -(cone/cos(lat_rad))*func(lat_rad);
    }

    int Nx{0};
    int Ny{0};
    double lon_ll_deg{0.0};
    double lat_ll_deg{0.0};
    double Phi1_radians{0.0};
    double Phi2_radians{0.0};
    double Phi0_radians{0.0};
    double Lon0_radians{0.0};
    double Lon_cen_radians{0.0};
    double Delta_km{0.0};
    double Radius_km{6371.200};
    double Bx{0.0};
    double By{0.0};
    double alpha{0.0};
    double cone{0.0};
};

/* --------------------------------------------------
*/
class Stereographic: public GridType {
public:
    Stereographic() = default;

    Stereographic(int nx, int ny, double lo, double la, double dlat, 
                double latD, double lov)
      : Nx(nx), Ny(ny), lon_ll_deg(lo), lat_ll_deg(la), Delta_km(dlat)
    {
        Phi0_radians = (lat_ll_deg)*M_PI/180;
        Lon0_radians = -rescale_lon(lon_ll_deg)*M_PI/180;
        reduce_rad(Lon0_radians);

        Phi1_radians = latD*M_PI/180;

        Lon_cen_radians = -rescale_lon(lov)*M_PI/180;
        reduce_rad(Lon_cen_radians);
        //Delta_km = data.d_km;
        //Radius_km = data.r_km;

        alpha = (-1.0/der_func(Phi1_radians))*(Radius_km/Delta_km);

        double r0, theta0;
        r0 = func(Phi0_radians);
        theta0 = Lon_cen_radians - Lon0_radians;

        Bx = -alpha*r0*sin(theta0);
        By =  alpha*r0*cos(theta0);
    }

    virtual ~Stereographic() = default;

    int getNx() const override { return Nx;}
    int getNy() const override { return Ny;}

    void lonLat2XY(double lon, double lat, double &x, double &y) const override {
        double lat_rad, lon_rad;
        double r, theta;

        lat_rad = lat*M_PI/180;
        lon_rad = -rescale_lon(lon)*M_PI/180;
        reduce_rad(lon_rad);
        r = func(lat_rad);
        theta = Lon_cen_radians - lon_rad;
        x = Bx + alpha*r*sin(theta);
        y = By - alpha*r*cos(theta);
    }

    void XY2LonLat(double x, double y, double &lon, double &lat) const override {
        double lat_rad, lon_rad;
        double r, theta;

        x = (x - Bx)/alpha;
        y = (y - By)/alpha;

        r = sqrt( x*x + y*y );
        lat_rad = inv_func(r);

        lat = lat_rad * 180/M_PI;
        if ( fabs(r) < 1.0e-5 )  
            theta = 0.0;
        else
            theta = atan2(x, -y);

        lon_rad = Lon_cen_radians - theta;
        reduce_rad(lon_rad);
        lon = 360. -lon_rad * 180/M_PI;
    }

    double rotGrid2Earth(int x, int y) const override { 
        double lat_deg, lon_deg, angle;
        double diff, hemi;

        XY2LonLat((double) x, (double) y, lon_deg, lat_deg);
        diff = Lon_cen_radians*M_PI/180 - lon_deg;

        if (Phi1_radians < 0.0) 
            hemi = -1.0;
        else
            hemi = 1.0;

        angle = diff*hemi;
        return angle;
    }

    void uv_to_xy(double u, double v, double &x, double &y) const {
        x = alpha*u + Bx;
        y = -alpha*v + By;
    }

    void xy_to_uv(double x, double y, double &u, double &v) const {
        u = (x - Bx)/alpha;
        v = (y - By)/(-alpha);
    }

private:
     void reduce_rad(double &angle_rad) const {
         angle_rad = angle_rad - 2*M_PI*floor( (angle_rad/(2*M_PI)) + 0.5 );
    }

    double func(double lat_rad) const {
        double r;
        r = tan(M_PI/4 -0.5*lat_rad);
        return r;
    }

    double inv_func(double r) const {
        return M_PI/2 - 2.0*atan(r);
    }

    double der_func(double lat_rad) const {
        return -1.0/(1.0 + sin(lat_rad));
    }

    int Nx{0};
    int Ny{0};
    double lon_ll_deg{0.0};
    double lat_ll_deg{0.0};
    double Phi1_radians{0.0};
    double Phi0_radians{0.0};
    double Lon0_radians{0.0};
    double Lon_cen_radians{0.0};
    double Delta_km{0.0};
    double Radius_km{6371.200};
    double Bx{0.0};
    double By{0.0};
    double alpha{0.0};
};

#endif
