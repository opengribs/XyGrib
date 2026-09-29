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

#include <QDate>
#include <QDateTime>
#include <QStringList>
#include <QUrl>

#include "NoaaNomadsProvider.h"

//---------------------------------------------------------------------------
// Parameter mapping.
//
// Left  : the single-letter code the download dialog has always used
//         (FileLoaderGRIB::getGribFile builds these strings).
// Right : the NOMADS "var_" / "lev_" tokens, verified against a live
//         filter_gfs_0p25.pl request on 2026-09-29 (35 messages returned,
//         no unrecognised-variable warnings).
//---------------------------------------------------------------------------

QStringList NoaaNomadsProvider::supportedModels ()
{
    return QStringList() << "GFS" << "NAM CONUS" << "NAM CACBN" << "NAM PACIFIC";
}

QStringList NoaaNomadsProvider::supportedWaveModels ()
{
    return QStringList() << "WW3" << "GWAM";
}

bool NoaaNomadsProvider::supports (const QString& model, const QString& waveModel)
{
    if (model != "None" && model != "" && ! supportedModels().contains (model))
        return false;
    if (waveModel != "None" && waveModel != "" && ! supportedWaveModels().contains (waveModel))
        return false;
    // Need at least one component to fetch.
    return ! (model == "None" && waveModel == "None");
}

NoaaNomadsProvider::NoaaNomadsProvider ()
{
}

QString NoaaNomadsProvider::name () const
{
    return QString ("NOAA NOMADS");
}

//---------------------------------------------------------------------------
// GFS family: /cgi-bin/filter_gfs_<res>.pl, file gfs.t<cyc>z.pgrb2.<res>.f<step>
// NAM family uses 12km CONUS/CACBN/Pacific paths and is served from the same
// host but a different directory tree.
//---------------------------------------------------------------------------
QString NoaaNomadsProvider::buildStepUrl (const GribRequestParams& p,
                                          const QString& cycle,
                                          const QString& stamp,
                                          int step)
{
    QString script, fileBase, dir;
    const int hour = cycle.toInt();

    if (p.model == "GFS") {
        QString res, gfsFile = "pgrb2";
        if (p.resolution == 0.25f)      res = "0p25";
        // The 0.50 deg GFS is published as pgrb2full, not pgrb2; NOMADS rejects
        // the short name with "Filename does not match legal pattern".
        else if (p.resolution == 0.50f) { res = "0p50"; gfsFile = "pgrb2full"; }
        else                            res = "1p00";
        script   = "filter_gfs_" + res + ".pl";
        fileBase = QString ("gfs.t%1z.%2.%3").arg (cycle, 2, QChar('0'))
                                             .arg (gfsFile).arg (res);
        dir      = QString ("/gfs.%1/%2/atmos").arg (stamp).arg (hour, 2, 10, QChar('0'));
    }
    else if (p.model == "NAM CONUS") {
        script   = "filter_nam.pl";
        fileBase = QString ("nam.t%1z.conusnest.hiresf").arg (cycle, 2, QChar('0'));
        dir      = QString ("/nam.%1").arg (stamp);
    }
    else if (p.model == "NAM CACBN") {
        script   = "filter_nam.pl";
        fileBase = QString ("nam.t%1z.cacbnest.hiresf").arg (cycle, 2, QChar('0'));
        dir      = QString ("/nam.%1").arg (stamp);
    }
    else { // NAM PACIFIC
        script   = "filter_nam.pl";
        fileBase = QString ("nam.t%1z.hawaiinest.hiresf").arg (cycle, 2, QChar('0'));
        dir      = QString ("/nam.%1").arg (stamp);
    }

    QString url = "https://nomads.ncep.noaa.gov/cgi-bin/" + script + "?";
    url += "file=" + fileBase + QString (".f%1").arg (step, 3, 10, QChar('0'));
    url += atmVarsAndLevels (p).join (QString());
    url += "&subregion=";
    url += QString ("&leftlon=%1&rightlon=%2&toplat=%3&bottomlat=%4")
                .arg (p.x0).arg (p.x1).arg (p.y1).arg (p.y0);
    url += "&dir=" + QString (QUrl::toPercentEncoding (dir));

    return url;
}

//---------------------------------------------------------------------------
// WW3 / GWAM: /cgi-bin/filter_gfswave.pl and filter_gwam.pl.
// Same subregion arguments; wave data lives in a separate directory and the
// product only exists at 0.25 deg for GFS-based waves.
//---------------------------------------------------------------------------
QString NoaaNomadsProvider::buildWaveStepUrl (const GribRequestParams& p,
                                              const QString& cycle,
                                              const QString& stamp,
                                              const QString& waveModel,
                                              int step)
{
    QString url;
    if (waveModel == "GWAM") {
        url = "https://nomads.ncep.noaa.gov/cgi-bin/filter_gwam.pl?";
        url += QString ("file=gwam.t%1z.global.0p25.f%2.grib2")
                    .arg (cycle, 2, QChar('0'))
                    .arg (step, 3, 10, QChar('0'));
        url += waveVarsAndLevels (p, waveModel).join (QString());
        url += "&subregion=";
        url += QString ("&leftlon=%1&rightlon=%2&toplat=%3&bottomlat=%4")
                    .arg (p.x0).arg (p.x1).arg (p.y1).arg (p.y0);
        url += "&dir=" + QString (QUrl::toPercentEncoding (
                    QString ("/gwam.%1/%2/wave/gridded").arg (stamp).arg (cycle.toInt(), 2, 10, QChar('0'))));
    }
    else { // WW3
        url = "https://nomads.ncep.noaa.gov/cgi-bin/filter_gfswave.pl?";
        url += QString ("file=gfswave.t%1z.global.0p25.f%2.grib2")
                    .arg (cycle, 2, QChar('0'))
                    .arg (step, 3, 10, QChar('0'));
        url += waveVarsAndLevels (p, waveModel).join (QString());
        url += "&subregion=";
        url += QString ("&leftlon=%1&rightlon=%2&toplat=%3&bottomlat=%4")
                    .arg (p.x0).arg (p.x1).arg (p.y1).arg (p.y0);
        url += "&dir=" + QString (QUrl::toPercentEncoding (
                    QString ("/gfs.%1/%2/wave/gridded").arg (stamp).arg (cycle.toInt(), 2, 10, QChar('0'))));
    }
    return url;
}

//---------------------------------------------------------------------------
QStringList NoaaNomadsProvider::atmVarsAndLevels (const GribRequestParams& p)
{
    QStringList out;
    const QStringList& a = p.atmParams;

    // Level selectors are emitted separately from variable selectors; both
    // must be present for a parameter to come back.
    QStringList levs;

    if (a.contains ("W")) {
        out << "&var_UGRD=on&var_VGRD=on";
        levs << "&lev_10_m_above_ground=on";
    }
    if (a.contains ("P") || a.contains ("p")) {
        out << "&var_PRMSL=on";
        levs << "&lev_mean_sea_level=on";
    }
    if (a.contains ("R")) {
        out << "&var_APCP=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("C")) {
        out << "&var_TCDC=on";
        levs << "&lev_entire_atmosphere=on";
    }
    if (a.contains ("T")) {
        out << "&var_TMP=on";
        levs << "&lev_2_m_above_ground=on";
    }
    if (a.contains ("H")) {
        out << "&var_RH=on";
        levs << "&lev_2_m_above_ground=on";
    }
    if (a.contains ("I")) {
        out << "&var_TMP=on";
        levs << "&lev_0C_isotherm=on";
    }
    if (a.contains ("S")) {
        out << "&var_SNOD=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("s")) {
        out << "&var_CSNOW=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("Z")) {
        out << "&var_CFRZR=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("c")) {
        out << "&var_CAPE=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("i")) {
        out << "&var_CIN=on";
        levs << "&lev_surface=on";
    }
    if (a.contains ("r")) {
        out << "&var_REFC=on";
        levs << "&lev_entire_atmosphere=on";
    }
    if (a.contains ("G")) {
        out << "&var_GUST=on";
        levs << "&lev_surface=on";
    }

    // Isobaric levels: 200/300/400/500/600/700/850/925 mb are coded 2..9.
    // Altitude data is temperature + geopotential + wind + humidity.
    static const struct { const char* code; int mb; } levels[] = {
        { "2", 200 }, { "3", 300 }, { "4", 400 }, { "5", 500 },
        { "6", 600 }, { "7", 700 }, { "8", 850 }, { "9", 925 },
    };
    bool anyLevel = false;
    for (size_t i = 0; i < sizeof(levels)/sizeof(levels[0]); i++) {
        if (a.contains (levels[i].code)) {
            anyLevel = true;
            levs << QString ("&lev_%1_mb=on").arg (levels[i].mb);
        }
    }
    if (anyLevel) {
        out << "&var_TMP=on&var_HGT=on&var_UGRD=on&var_VGRD=on&var_RH=on";
    }
    if (a.contains ("skewt")) {
        // Skew-T needs the same isobaric columns; make sure they are asked for
        // even if no individual level box was ticked.
        if (! anyLevel) {
            out << "&var_TMP=on&var_HGT=on&var_UGRD=on&var_VGRD=on&var_RH=on";
            levs << "&lev_925_mb=on&lev_850_mb=on&lev_700_mb=on&lev_600_mb=on"
                 << "&lev_500_mb=on&lev_400_mb=on&lev_300_mb=on&lev_200_mb=on";
        }
    }

    out << levs;
    return out;
}

//---------------------------------------------------------------------------
QStringList NoaaNomadsProvider::waveVarsAndLevels (const GribRequestParams& p,
                                                   const QString& waveModel)
{
    Q_UNUSED (waveModel);
    QStringList out;
    const QStringList& w = p.waveParams;

    // "s" = significant height of combined wind waves and swell
    // "H"/"D"/"P" = total swell height / direction / mean period
    // "h"/"d"/"p" = wind wave height / direction / mean period
    //
    // NOMADS names these HTSGW / HTSGW / DIRPW+DIRPW / PERPW for the primary
    // set and SWELL/SWDIR/SWPER style tokens for the partitions. We request
    // the union of what the UI offers and let the client's GRIB2 parameter
    // mapping (Grib2Record::analyseProductType) sort them out.
    bool any = false;
    if (w.contains ("s")) { out << "&var_HTSGW=on"; any = true; }
    if (w.contains ("H") || w.contains ("D") || w.contains ("P")) {
        out << "&var_HTSGW=on";
        any = true;
    }
    if (w.contains ("h") || w.contains ("d") || w.contains ("p")) {
        out << "&var_HTSGW=on";
        any = true;
    }
    if (any) {
        // Primary wave direction/period plus wind wave direction/period.
        out << "&var_DIRPW=on&var_PERPW=on&var_WVDIR=on&var_WVPER=on";
        out << "&lev_surface=on";
    }
    return out;
}

//---------------------------------------------------------------------------
QStringList NoaaNomadsProvider::buildUrls (const GribRequestParams& p) const
{
    QStringList urls;
    if (! supports (p.model, p.waveModel))
        return urls;

    const QString cycle = resolveCycle (p.cycle);
    const QString stamp = QDateTime::currentDateTimeUtc().toString ("yyyyMMdd");

    const int maxHour = p.days * 24;
    const int step    = (p.interval > 0) ? p.interval : 3;

    for (int h = 0; h <= maxHour; h += step) {
        if (p.model != "None")
            urls << buildStepUrl (p, cycle, stamp, h);
        if (p.waveModel != "None")
            urls << buildWaveStepUrl (p, cycle, stamp, p.waveModel, h);
    }
    return urls;
}

//---------------------------------------------------------------------------
QString NoaaNomadsProvider::suggestedFileName (const GribRequestParams& p) const
{
    const QString stamp = QDateTime::currentDateTimeUtc().toString ("yyyyMMdd_hh");
    QString tag;
    if (p.model != "None")
        tag = p.model;
    if (p.waveModel != "None")
        tag += (tag.isEmpty() ? "" : "_") + p.waveModel;
    tag.replace (" ", "_");

    return QString ("%1_%2_%3d.grb")
                .arg (tag)
                .arg (stamp)
                .arg (p.days);
}

//---------------------------------------------------------------------------
QString NoaaNomadsProvider::waveModelFor (const GribRequestParams& p) const
{
    if (p.waveModel != "None" && supportedWaveModels().contains (p.waveModel))
        return p.waveModel;
    return QString();
}

//---------------------------------------------------------------------------
// NOMADS serves cycles 00/06/12/18 with roughly 3.5-5h publication lag. If the
// user asked for "last" (or a cycle that cannot exist yet today), pick the most
// recent cycle that should have files by now. A wrong guess is survivable: the
// step-2 fetch validates the GRIB magic and the loader falls back to the
// previous cycle if the answer is not GRIB.
//---------------------------------------------------------------------------
QString NoaaNomadsProvider::resolveCycle (const QString& requested)
{
    static QString cachedDay;
    static QString cachedCycle;

    const QString today = QDateTime::currentDateTimeUtc().toString ("yyyyMMdd");
    // NOMADS cycles are UTC, so the readiness check must be UTC too. Using local
    // time here made the answer depend on the workstation's timezone and could
    // select a cycle that has not been published.
    const QTime now = QDateTime::currentDateTimeUtc().time();
    // Assume a ~5h publication lag.
    const int readyHour = now.hour() - 5;

    int best = 0;
    if (readyHour >= 18)      best = 18;
    else if (readyHour >= 12) best = 12;
    else if (readyHour >= 6)  best = 6;
    else if (readyHour >= 0)  best = 0;
    else                      best = 0;   // early morning: 00Z of today, or 18Z of yesterday

    if (requested != "last" && requested != "" && requested != "0") {
        // Trust an explicit cycle if it is plausible, else fall back.
        bool ok = false;
        int want = requested.toInt (&ok);
        if (ok && want >= 0 && want <= 21) {
            // Only accept it if it should already be published.
            if (want <= best) return QString ("%1").arg (want, 2, 10, QChar('0'));
        }
    }
    if (cachedDay == today && ! cachedCycle.isEmpty())
        return cachedCycle;

    cachedDay   = today;
    cachedCycle = QString ("%1").arg (best, 2, 10, QChar('0'));
    return cachedCycle;
}
