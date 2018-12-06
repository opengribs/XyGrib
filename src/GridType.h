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
};

/*
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

/*
 */
class Mercator : public GridType {
public:
    Mercator() = default;
    Mercator(int nx, int ny, double lo, double la, double lo2, double la2, double dlon, double dlat)
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

class Lambert : public GridType {
public:
    Lambert() = default;
    virtual ~Lambert() = default;
};

#endif
