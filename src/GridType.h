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

    ~PlateCarree() = default;

    int getNx() const override { return Nx;}
    int getNy() const override { return Ny;}

    void lonLat2XY(double lon, double lat, double &x, double &y) const override {
        y = (lat - lat_ll_deg)/delta_lat_deg;
        x = (lon - lon_ll_deg)/delta_lon_deg;
    }

    void XY2LonLat(double x, double y, double &lon, double &lat) const override {

        lat = lat_ll_deg + delta_lat_deg*y;
        lon = lon_ll_deg + delta_lon_deg*x;
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
class Mercator : GridType {
public:
    Mercator() = default;
    ~Mercator() = default;
};

class Lambert : GridType {
public:
    Lambert() = default;
    ~Lambert() = default;
};

#endif
