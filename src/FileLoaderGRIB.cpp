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

#include <QApplication>
#include <QTextStream>
#include <QDomDocument>
#include <QNetworkProxy>
#include <QMessageBox>

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "FileLoaderGRIB.h"
#include "NoaaNomadsProvider.h"
#include "Util.h"
#include "Version.h"

//========================================================================
// Direct-download mode.
//
// Reading the environment variable at call time (rather than caching at
// startup) means a test harness or a wrapper script can flip it per run, and
// nothing needs to be written into the user's settings file. An explicit
// setting is also honoured, so users who prefer the INI can set
// downloadLocal=true instead.
//========================================================================
bool FileLoaderGRIB::useLocalDownload ()
{
    const QByteArray env = qgetenv ("XYGRIB_LOCAL_DOWNLOAD");
    if (! env.isEmpty()) {
        const QByteArray v = env.trimmed().toLower();
        if (v == "0" || v == "false" || v == "no" || v == "off")
            return false;
        return true;
    }
    return Util::getSetting ("downloadLocal", false).toBool();
}

//========================================================================
FileLoaderGRIB::FileLoaderGRIB (QNetworkAccessManager *manager, QWidget *parent)
			: FileLoader (manager)
{
	this->parent = parent;
    step = 0;
	downloadError = false;
    reply_step1 = nullptr;
    reply_step2 = nullptr;
    scriptpath = "/";
    localMode = false;
    localIndex = 0;
    localTotal = 0;
    localDone = 0;
}
//-------------------------------------------------------------------------------
FileLoaderGRIB::~FileLoaderGRIB () 
{
	if (reply_step1) {
		reply_step1->deleteLater ();
        reply_step1 = nullptr;
	}
	if (reply_step2) {
		reply_step2->deleteLater ();
        reply_step2 = nullptr;
	}
    for (int i = 0; i < localReplies.size(); i++) {
        localReplies[i]->deleteLater ();
    }
    localReplies.clear ();
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::stop () 
{
	if (reply_step1) {
		reply_step1->close ();
		downloadError = true;
	}
	if (reply_step2) {
		reply_step2->close ();
		downloadError = true;
	}
    for (int i = 0; i < localReplies.size(); i++) {
        localReplies[i]->close ();
    }
    if (! localReplies.isEmpty())
        downloadError = true;
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::abort ()
{
	if (reply_step1) {
		reply_step1->abort ();
		downloadError = true;
	}
	if (reply_step2) {
		reply_step2->abort ();
		downloadError = true;
	}
    for (int i = 0; i < localReplies.size(); i++) {
        localReplies[i]->abort ();
    }
    if (! localReplies.isEmpty())
        downloadError = true;
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::getGribFile(
        const QString& atmModel,
        float x0, float x1, float y0, float y1,
        float resolution, int interval, int days,
        const QString& cycle,
        bool wind, bool pressure, bool rain,
        bool cloud, bool temp, bool humid, bool isotherm0,
        bool snowDepth,
		bool snowCateg, bool frzRainCateg,
        bool CAPEsfc, bool CINsfc, bool reflectivity,
		bool altitudeData200,
		bool altitudeData300,
		bool altitudeData400,
		bool altitudeData500,
		bool altitudeData600,
		bool altitudeData700,
		bool altitudeData850,
		bool altitudeData925,
		bool skewTData,
		bool GUSTsfc,
        const QString& wvModel,
        bool sgwh,
        bool swell,
        bool wwav
	)
{
    QString page;
	step = 0;
    //----------------------------------------------------------------
    // Step 1: Request the creation of the Grib file (status, name, size & checksum in return)
    //----------------------------------------------------------------
    // build the parameter string
    QString parameters = "";
    if (wind) {
        parameters += "W;";
    }
    if (pressure && (atmModel != "ECMWF" && atmModel != "Arpege-EU" && atmModel != "Arome 0.025°")) {
        parameters += "P;";
    }
    if (pressure && (atmModel == "ECMWF" || atmModel == "Arpege-EU" || atmModel == "Arome 0.025°")){
        parameters += "p;";
    }
    if (rain) {
        parameters += "R;";
    }
    if (cloud) {
        parameters += "C;";
    }
    if (temp) {
        parameters += "T;";
    }
    if (humid) {
        parameters += "H;";
    }
	if (isotherm0) {
		parameters += "I;";
    }    
//    if (tempMin) {
//        parameters += "m;";
//    }
//    if (tempMax) {
//        parameters += "M;";
//    }
    if (snowDepth) {
        parameters += "S;";
    }
    if (snowCateg) {
        parameters += "s;";
    }
    if (frzRainCateg) {
        parameters += "Z;";
    }
    if (CAPEsfc) {
        parameters += "c;";
    }
    if (CINsfc) {
        parameters += "i;";
    }
    if (reflectivity) {
        parameters += "r;";
    }
    if (GUSTsfc) {
        parameters += "G;";
    }
//    if (SUNSDsfc) { // not currently used
//        parameters += "D;";
//    }
	
    if (altitudeData200) parameters += "2;";
    if (altitudeData300) parameters += "3;";
    if (altitudeData400) parameters += "4;";
    if (altitudeData500) parameters += "5;";
    if (altitudeData600) parameters += "6;";
    if (altitudeData700) parameters += "7;";
    if (altitudeData850) parameters += "8;";
    if (altitudeData925) parameters += "9;";
//    if (cloudLayers) parameters += "L;";
	if (skewTData)  parameters += "skewt;";
	

    // wave parameters have their own string
    QString waveParams = "";
    if (sgwh) {
        waveParams += "s;";
    }
    if (swell) {
        waveParams += "H;D;P;";
    }
    if (wwav) {
        waveParams += "h;d;p;";
    }

    // model identifier (combines resolution)
    QString amod = "";
    if (atmModel == "GFS" && resolution == 0.25 ){
        amod = "gfs_p25_";
    } else if (atmModel == "GFS" && resolution == 0.50 ){
        amod = "gfs_p50_";
    } else if (atmModel == "GFS" && resolution == 1.0 ){
        amod = "gfs_1p0_";
    } else if (atmModel == "ICON"){
        amod = "icon_p25_";
    } else if (atmModel == "Arpege"){
        amod = "arpege_p50_";
    } else if (atmModel == "ECMWF"){
        amod = "ecmwf_p50_";
    } else if (atmModel == "NAM CONUS"){
        amod = "nam_conus_12km_";
    } else if (atmModel == "NAM CACBN"){
        amod = "nam_cacbn_12km_";
    } else if (atmModel == "NAM PACIFIC"){
        amod = "nam_pacific_12km_";
    } else if (atmModel == "ICON-EU"){
        amod = "icon_eu_p06_";
    } else if (atmModel == "Arpege-EU"){
        amod = "arpege_eu_p10_";
    } else if (atmModel == "Arome 0.025°"){
        amod = "arome_p025_";
    } else if (atmModel == "None"){
        amod = "none";
    }

    // wave model identifier
    QString wmod = "";
    if (wvModel == "WW3"){
        wmod = "ww3_p50_";
    } else if (wvModel == "GWAM" ){
        wmod = "gwam_p25_";
    } else if (wvModel == "EWAM"){
        wmod = "ewam_p05_";
    } else if (wvModel == "None"){
        wmod = "none";
    }

    //----------------------------------------------------------------
    // Direct download: no broker. The provider does the cropping and we
    // concatenate one response per forecast step. The parameter strings
    // built above are the dialog's own alphabet and are re-mapped by the
    // provider, so nothing above this point is broker-specific.
    //----------------------------------------------------------------
    if (useLocalDownload ())
    {
        if (parameters == "" && waveParams == "")
        {
            emit signalGribLoadError (tr("No atmospheric or wave parameters were selected"));
            downloadError = true;
            return;
        }
        if (! NoaaNomadsProvider::supports (atmModel, wvModel))
        {
            emit signalGribLoadError (
                    tr("Local download does not support this model yet: %1 %2. "
                       "Turn off XYGRIB_LOCAL_DOWNLOAD to use the server.")
                        .arg (atmModel).arg (wvModel));
            downloadError = true;
            return;
        }
        GribRequestParams p;
        buildLocalParams (atmModel, x0, y0, x1, y1, resolution, interval, days,
                          cycle,
                          wind, pressure, rain, cloud, temp, humid, isotherm0,
                          snowDepth, snowCateg, frzRainCateg, CAPEsfc, CINsfc,
                          reflectivity,
                          altitudeData200, altitudeData300, altitudeData400,
                          altitudeData500, altitudeData600, altitudeData700,
                          altitudeData850, altitudeData925,
                          skewTData, GUSTsfc,
                          wvModel, sgwh, swell, wwav);
        startLocalDownload (p);
        return;
    }

//    QString runCycle = Util::getSetting("downloadRunCycle", "last").toString().toLower();
    //strbuf.clear();
    if (!(parameters == "" && waveParams == ""))
    {
        step = 1;
        emit signalGribSendMessage(
                tr("Preparing file on server... Please wait..."));
        emit signalGribReadProgress(step, 0, 0);


        QString phpfilename = scriptpath+ "getmygribs2.php?";
        QString appVer = Version::getVersion();
        QString ptype = "Unknown";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        ptype = QSysInfo::productType();
#endif
        QTextStream(&page) << phpfilename
                           << "osys=" << ptype
                           << "&ver=" << appVer
                           << "&model=" << amod
                           << "&la1=" << y0
                           << "&la2=" << y1
                           << "&lo1=" << x0
                           << "&lo2=" << x1
                           << "&intv=" << interval
                           << "&days=" << days
                           << "&cyc=" << cycle
                           << "&par=" << parameters
                           << "&wmdl=" << wmod
                           << "&wpar=" << waveParams
                           ;
//                            << "&client=" << Version::getCompleteName()

        QNetworkRequest request = Util::makeNetworkRequest ("http://"+Util::getServerName()+page);
		reply_step1 = networkManager->get (request);
		connect (reply_step1, SIGNAL(downloadProgress (qint64,qint64)), 
				 this, SLOT(downloadProgress (qint64,qint64)));
		connect (reply_step1, SIGNAL(error(QNetworkReply::NetworkError)),
				 this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
		connect (reply_step1, SIGNAL(finished()),
				 this, SLOT(slotFinished_step1 ()));
    }
    else
    {
        QMessageBox::warning(parent, tr("Bad Request"), tr("No atmospheric or wave parameters were selected"));
    }
}
//-------------------------------------------------------------------------------
void FileLoaderGRIB::slotNetworkError (QNetworkReply::NetworkError /*err*/)
{
	if (! downloadError) {
		if (sender() == reply_step1) {
			emit signalGribLoadError (reply_step1->errorString());
			downloadError = true;
		}
		else if (sender() == reply_step2) {
			emit signalGribLoadError (reply_step2->errorString());
			downloadError = true;
		}
	}
}
//-------------------------------------------------------------------------------
void FileLoaderGRIB::downloadProgress (qint64 done, qint64 total)
{
	if (downloadError) {
		step = 1000;
		return;
	}
	if (sender() == reply_step2) {
		emit signalGribReadProgress (step, done, total);
	}
}
//-------------------------------------------------------------------------------
void FileLoaderGRIB::slotFinished_step1 ()
{
DBG("slotFinished_step1");
	if (!downloadError) {
		//-------------------------------------------
        // Back from step 1: preparing the file
		//-------------------------------------------
        //xserv=reply_step1->rawHeader("XServer");
        //strbuf.append(xserv);
		QByteArray data = reply_step1->readAll ();
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        QJsonObject jsondata = jsondoc.object();

        bool status = jsondata["status"].toBool();
        // if the result is valid the status is true and the message contains object
        // with name, size and checksum
        if (status) {
            QJsonObject msg = jsondata["message"].toObject();
            fileName = msg["url"].toString();
            fileSize = msg["size"].toInt();
            checkSumSHA1 = msg["sha1"].toString();

        }else{ // message contains only the error message
            QString m = jsondata["message"].toString();
//            QMessageBox::warning(parent, tr("Information"), m);
            emit signalGribLoadError(m);
            downloadError = true;
            reply_step1->close();
        }

		//-------------------------------------------------------------
        // Start Step 2: Request the contents of the Grib file
		//-------------------------------------------------------------
        if (status) {
            step = 2;
            QString s;
            s = tr("Total size : ") + QString("%1 kb").arg(fileSize/1024, 0);
            emit signalGribSendMessage(s);
            emit signalGribStartLoadData();            
			QNetworkRequest request;
            request.setUrl (QUrl(fileName));
            reply_step2 = networkManager->get (request);
			connect (reply_step2, SIGNAL(downloadProgress (qint64,qint64)), 
					 this, SLOT(downloadProgress (qint64,qint64)));
			connect (reply_step2, SIGNAL(error(QNetworkReply::NetworkError)),
					 this, SLOT(slotNetworkError (QNetworkReply::NetworkError)));
			connect (reply_step2, SIGNAL(finished()),
					this, SLOT(slotFinished_step2 ()));
		}
		else {
			downloadError = true;
		}
	}
}
//-------------------------------------------------------------------------------
void FileLoaderGRIB::slotFinished_step2 ()
{
// DBG("slotFinished_step2");		
	if (!downloadError) {
		step = 1000;
		arrayContent = reply_step2->readAll ();
		if (arrayContent.size() < 80) {
			emit signalGribLoadError (tr("Empty file."));
			return;
		}
		emit signalGribSendMessage(tr("CheckSum control"));
		if (Util::sha1 (arrayContent) == checkSumSHA1)
		{
			emit signalGribSendMessage(tr("Finish")
                            + QString(" %1 kb").arg(arrayContent.size()/1024.0,0,'f',1));
            // TODO not sure about the .grb thing here
			emit signalGribDataReceived(&arrayContent, fileName.replace("%20",".grb"));
		}
		else {
			emit signalGribLoadError (tr("Bad checksum."));
		}
	}
}

//===============================================================================
// Direct-provider download
//===============================================================================

//-------------------------------------------------------------------------------
// Translate the dialog's widget flags into a plain request object. Kept
// separate from getGribFile so the mapping is in one place and the URL builder
// does not need to know about booleans.
//-------------------------------------------------------------------------------
void FileLoaderGRIB::buildLocalParams (const QString& atmModel,
                                       float x0, float y0, float x1, float y1,
                                       float resolution, int interval, int days,
                                       const QString& cycle,
                                       bool wind, bool pressure, bool rain,
                                       bool cloud, bool temp, bool humid,
                                       bool isotherm0, bool snowDepth,
                                       bool snowCateg, bool frzRainCateg,
                                       bool CAPEsfc, bool CINsfc,
                                       bool reflectivity,
                                       bool altitudeData200, bool altitudeData300,
                                       bool altitudeData400, bool altitudeData500,
                                       bool altitudeData600, bool altitudeData700,
                                       bool altitudeData850, bool altitudeData925,
                                       bool skewTData, bool GUSTsfc,
                                       const QString& wvModel,
                                       bool sgwh, bool swell, bool wwav)
{
    localParams = GribRequestParams ();
    localParams.model      = atmModel;
    localParams.waveModel  = wvModel;
    localParams.cycle      = cycle;
    localParams.x0 = x0; localParams.y0 = y0;
    localParams.x1 = x1; localParams.y1 = y1;
    localParams.resolution = resolution;
    localParams.interval   = interval;
    localParams.days       = days;

    QStringList& a = localParams.atmParams;
    if (wind)        a << "W";
    if (pressure)    a << "P";
    if (rain)        a << "R";
    if (cloud)       a << "C";
    if (temp)        a << "T";
    if (humid)       a << "H";
    if (isotherm0)   a << "I";
    if (snowDepth)   a << "S";
    if (snowCateg)   a << "s";
    if (frzRainCateg) a << "Z";
    if (CAPEsfc)     a << "c";
    if (CINsfc)      a << "i";
    if (reflectivity) a << "r";
    if (GUSTsfc)     a << "G";
    if (altitudeData200) a << "2";
    if (altitudeData300) a << "3";
    if (altitudeData400) a << "4";
    if (altitudeData500) a << "5";
    if (altitudeData600) a << "6";
    if (altitudeData700) a << "7";
    if (altitudeData850) a << "8";
    if (altitudeData925) a << "9";
    if (skewTData)   a << "skewt";

    QStringList& w = localParams.waveParams;
    if (sgwh)  w << "s";
    if (swell) w << "H" << "D" << "P";
    if (wwav)  w << "h" << "d" << "p";
}

//-------------------------------------------------------------------------------
// A GRIB message always starts with the 4-byte magic "GRIB". Upstream returns
// an HTML error page (with a non-200 status) for a cycle that is not published
// yet, so checking the magic is how we tell a real file from an error body
// without trusting the status code alone.
//-------------------------------------------------------------------------------
bool FileLoaderGRIB::looksLikeGrib (const QByteArray& data)
{
    return data.size() >= 4 && data.startsWith ("GRIB");
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::startLocalDownload (const GribRequestParams& p)
{
    localMode    = true;
    arrayContent.clear ();
    localReplies.clear ();
    localDone    = 0;
    localIndex   = 0;
    downloadError = false;

    NoaaNomadsProvider provider;
    localUrls      = provider.buildUrls (p);
    localTotal     = localUrls.size ();
    localSuggested = provider.suggestedFileName (p);

    if (localUrls.isEmpty()) {
        emit signalGribLoadError (tr("Nothing to download for this selection."));
        downloadError = true;
        return;
    }

    step = 1;
    emit signalGribSendMessage (
            tr("Downloading %1 forecast steps directly from NOAA...").arg (localTotal));
    emit signalGribReadProgress (1, 0, 0);
    emit signalGribStartLoadData ();

    fetchNextLocalUrl ();
}

//-------------------------------------------------------------------------------
// Fetches one URL at a time. Sequential rather than parallel so a long horizon
// does not open dozens of sockets against NOMADS at once and so partial
// failures leave a coherent prefix of the timeline.
//-------------------------------------------------------------------------------
void FileLoaderGRIB::fetchNextLocalUrl ()
{
    if (downloadError)
        return;

    if (localIndex >= localUrls.size ()) {
        // All steps landed. A GRIB2 file is a concatenation of self-contained
        // messages, so the accumulated buffer is already a valid file.
        step = 1000;
        if (! looksLikeGrib (arrayContent)) {
            emit signalGribLoadError (
                    tr("Upstream did not return GRIB data. The forecast cycle may "
                       "not be published yet, or the area is not covered by this model."));
            downloadError = true;
            return;
        }
        if (arrayContent.size () < 80) {
            emit signalGribLoadError (tr("Empty file."));
            downloadError = true;
            return;
        }
        emit signalGribSendMessage (tr("Finish")
                + QString (" %1 kb").arg (arrayContent.size () / 1024.0, 0, 'f', 1));
        emit signalGribDataReceived (&arrayContent, localSuggested);
        localMode = false;
        return;
    }

    const QString url = localUrls.at (localIndex);
    emit signalGribSendMessage (
            tr("Step %1 of %2...").arg (localIndex + 1).arg (localTotal));

    QNetworkRequest request;
    request.setUrl (QUrl (url));
    // NOMADS currently returns raw GRIB rather than a content-encoded body,
    // but asking for identity keeps it that way and guarantees Content-Length
    // stays meaningful for the progress display.
    request.setRawHeader ("Accept-Encoding", "identity");
    QNetworkReply *reply = networkManager->get (request);
    localReplies.append (reply);

    connect (reply, SIGNAL (downloadProgress (qint64,qint64)),
             this, SLOT (slotLocalStepProgress (qint64,qint64)));
    connect (reply, SIGNAL (error(QNetworkReply::NetworkError)),
             this, SLOT (slotNetworkError (QNetworkReply::NetworkError)));
    connect (reply, SIGNAL (finished()),
             this, SLOT (slotLocalStepFinished ()));
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::slotLocalStepProgress (qint64 /*done*/, qint64 /*total*/)
{
    // Per-step bytes are not additive across replies the way the legacy
    // single-file progress was, so progress is reported by step count and the
    // accumulated size so far.
    if (downloadError)
        return;
    emit signalGribReadProgress (localDone, localDone, qMax (1, localTotal));
}

//-------------------------------------------------------------------------------
void FileLoaderGRIB::slotLocalStepFinished ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender ());
    if (! reply)
        return;

    if (downloadError) {
        reply->deleteLater ();
        return;
    }

    const QByteArray body = reply->readAll ();
    const int httpStatus =
            reply->attribute (QNetworkRequest::HttpStatusCodeAttribute).toInt ();

    reply->deleteLater ();
    localReplies.removeAll (reply);

    if (! looksLikeGrib (body)) {
        // Not a GRIB message: an error page, a redirect body, or an empty
        // response. Report it rather than silently writing a corrupt file.
        emit signalGribLoadError (
                tr("Upstream returned no GRIB data for step %1 (HTTP %2). "
                   "The forecast cycle may not be published yet.")
                    .arg (localIndex + 1).arg (httpStatus));
        downloadError = true;
        return;
    }

    arrayContent.append (body);
    localDone += body.size ();
    localIndex++;

    emit signalGribReadProgress (localIndex, localDone, qMax (1, localTotal));
    fetchNextLocalUrl ();
}

