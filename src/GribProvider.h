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

#ifndef GRIBPROVIDER_H
#define GRIBPROVIDER_H

#include <QString>
#include <QStringList>

//---------------------------------------------------------------------------
// What the user asked the download dialog for, translated out of the widget
// layer into plain values. One of these is built per download request and
// handed to a provider, which turns it into upstream URLs.
//---------------------------------------------------------------------------
struct GribRequestParams
{
    QString model;          // "GFS", "NAM CONUS", "None", ...
    QString waveModel;      // "WW3", "GWAM", "EWAM", "None"
    QString cycle;          // "last", or "00"/"06"/"12"/"18"

    float   x0 = -180.0f;   // west
    float   y0 = -90.0f;    // south
    float   x1 = 180.0f;    // east
    float   y1 = 90.0f;     // north
    float   resolution = 0.25f;

    int     interval = 3;   // hours between forecast steps
    int     days = 1;       // forecast horizon

    // Atmospheric parameters, in the legacy single-letter alphabet used by
    // the download dialog (see FileLoaderGRIB::getGribFile).
    QStringList atmParams;
    // Wave parameters: "s" (significant height), "H"/"D"/"P" (swell
    // height/direction/period), "h"/"d"/"p" (wind wave height/direction/period).
    QStringList waveParams;
};

//---------------------------------------------------------------------------
// A provider fetches weather data straight from an upstream source that does
// the spatial/parameter cropping server-side, so the client does not need a
// broker. Providers return the list of URLs to fetch; FileLoaderGRIB fetches
// them and concatenates the results.
//
// Concatenation is valid because a GRIB2 file is just a sequence of
// self-contained messages, and the reader already handles multi-message files.
//---------------------------------------------------------------------------
class GribProvider
{
    public:
        virtual ~GribProvider() {}

        // Human-readable name, for messages and diagnostics.
        virtual QString name () const = 0;

        // Build the list of URLs for this request, one per forecast step.
        // Empty list means "this request cannot be served".
        virtual QStringList buildUrls (const GribRequestParams& p) const = 0;

        // Suggested file name for the saved GRIB.
        virtual QString suggestedFileName (const GribRequestParams& p) const = 0;

        // Which wave model, if any, this provider can supply for p.model.
        // Empty means the request has no wave component.
        virtual QString waveModelFor (const GribRequestParams& p) const = 0;

    protected:
        // Shared helper: is this provider able to serve (model, waveModel)?
        static bool isKnownModel (const QString& model, const QStringList& known)
        {
            return known.contains (model);
        }
};

#endif
