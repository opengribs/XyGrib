/**********************************************************************
XyGrib: meteorological GRIB file viewer
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

#ifndef NOAA_NOMADS_PROVIDER_H
#define NOAA_NOMADS_PROVIDER_H

#include "GribProvider.h"

//---------------------------------------------------------------------------
// Fetches GFS/NAM and WW3/GWAM directly from the NOAA NOMADS GRIB filter,
// which crops by bounding box, parameter and level on the server side. This
// is the upstream service that makes the old OpenGribs broker unnecessary:
// a small spatial window costs a few tens of kilobytes instead of the
// hundreds of megabytes an unfiltered file would.
//
// URLs look like:
//   https://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p25.pl
//       ?file=gfs.t00z.pgrb2.0p25.f003
//       &var_UGRD=on&var_VGRD=on&lev_10_m_above_ground=on
//       &subregion=&leftlon=..&rightlon=..&toplat=..&bottomlat=..
//       &dir=%2Fgfs.YYYYMMDD%2F00%2Fatmos
//---------------------------------------------------------------------------
class NoaaNomadsProvider : public GribProvider
{
    public:
        NoaaNomadsProvider ();

        QString name () const override;
        QStringList buildUrls (const GribRequestParams& p) const override;
        QString suggestedFileName (const GribRequestParams& p) const override;
        QString waveModelFor (const GribRequestParams& p) const override;

        // Models this provider can serve.
        static QStringList supportedModels ();
        static QStringList supportedWaveModels ();
        static bool supports (const QString& model, const QString& waveModel);

        // Cycle currently advertised as available. Cached per day so a long
        // horizon does not re-probe for every step.
        static QString resolveCycle (const QString& requested);

    private:
        static QString buildStepUrl (const GribRequestParams& p,
                                     const QString& cycle,
                                     const QString& stamp,   // YYYYMMDD
                                     int step);
        static QString buildWaveStepUrl (const GribRequestParams& p,
                                         const QString& cycle,
                                         const QString& stamp,
                                         const QString& waveModel,
                                         int step);
        static QStringList atmVarsAndLevels (const GribRequestParams& p);
        static QStringList waveVarsAndLevels (const GribRequestParams& p,
                                              const QString& waveModel);
};

#endif
