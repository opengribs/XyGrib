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

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollBar>

#include <cmath>
#include <cassert>

#include "MeteoTable.h"
#include "GribPlot.h"
#include "Util.h"
#include "Settings.h"
#include "DataQString.h"

//------------------------------------------------------------------------
QWidget *MeteoTableDialog::createDataTable ()
{
	if (dataTable) {
		delete dataTable;
	}
	
	meteoTableWidget = new MeteoTableWidget (plotter, lon,lat, locationName, this);
	assert (meteoTableWidget);

	//-------------------------------------
	// scrollarea containing data area
	scrollArea = new QScrollArea();
	assert (scrollArea);
	scrollArea->setWidget (meteoTableWidget);
	scrollArea->setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
	
	//-------------------------------------
	// Headers area
	dataTable = new QWidget (this);
	assert (dataTable);

	// hdr column
	QWidget * hdrTable;
	hdrTable = new QWidget (dataTable);
	
	QVBoxLayout *layhdr = new QVBoxLayout (hdrTable);
	layhdr->setContentsMargins (0,0,0,0);
	layhdr->setSpacing (0);
		layhdr->addWidget (meteoTableWidget->getDataHeaders());
		layhdr->addStretch ();
	
	hdrscroll = new QScrollArea ();
	assert (hdrscroll);
	hdrscroll->setWidgetResizable (true);
	
	hdrscroll->setWidget (hdrTable);
	hdrscroll->setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOff);
	hdrscroll->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

	// synchronise les scrollbars
	QScrollBar * dataVscrollbar = scrollArea->verticalScrollBar ();
	QScrollBar * hdrVscrollbar = hdrscroll->verticalScrollBar ();
	
	connect (dataVscrollbar, SIGNAL(valueChanged(int)),
			 hdrVscrollbar,  SLOT(setValue(int)));

	//-------------------------------------
	// hdr + data
	QGridLayout *laytable = new QGridLayout (dataTable);
	laytable->setContentsMargins (0,0,0,0);
	laytable->setSpacing (0);
		laytable->addWidget (hdrscroll, 0,0);
		laytable->addWidget (scrollArea, 0,1);
		
	laytable->setColumnStretch (0, 0);
	laytable->setColumnStretch (1, 1);

	return dataTable;
}
//===================================================================
// MeteoTable : dialog + MeteoTableWidget
//===================================================================
MeteoTableDialog::MeteoTableDialog (
					GriddedPlotter *plotter, 
					double lon, double lat, QString locationName)
	: QWidget (NULL, Qt::Window)
{
	setProperty ("objectType", "MeteoTableDialog");
	setFocusPolicy(Qt::StrongFocus);
	setWindowIcon (QIcon (Util::pathImg("zyGrib_32.xpm")));
	this->plotter = plotter;
	this->lon = lon;
	this->lat = lat;
	this->locationName = locationName;
	optionsDialog = NULL;
	dataTable = NULL;
    
    if (!plotter || !plotter->isReaderOk()) {
        QMessageBox::critical (this,
			tr("Error"),tr("Can't create Meteotable:\n\nNo GRIB file opened."));
		return;
    }
    GriddedReader *reader = plotter->getReader();
    GriddedRecord *record;
	if ((record=reader->getFirstRecord()) == NULL) {
        QMessageBox::critical (this,
            	tr("Error"),tr("Can't create Meteotable\n\nGRIB area undefined."));
		return;
    }
	if (!reader->isPointInMap (this->lon, this->lat)) {
		this->lon += 360;
		if (!reader->isPointInMap (this->lon, this->lat)) {
			QMessageBox::critical (this,
					tr("Error"),tr("Can't create Meteotable:\n\nPoint outside GRIB area."));
			return;
		}
	}
	//======================================================================
	// Data table
	//======================================================================
	dataTable = createDataTable ();
	
	//======================================================================
	
	mainLayout = new QVBoxLayout (this);

	QString position = Util::formatPosition(lon, lat);
	QLabel *lbpos;
	if (locationName == "") {
		setWindowTitle(position);
		lbpos = new QLabel(tr("Location: ") + position);
	}
	else {
		setWindowTitle(locationName);
		lbpos = new QLabel(tr("Location: <b>") +locationName + "</b> : " + position);
	}
	
	//------------------------------------------------------------
	// Dates of the forecast (meteo center dependent)
	//------------------------------------------------------------
	std::set<DataCenterModel>  allDcm = reader->getAllDataCenterModel ();
	std::set<DataCenterModel>::iterator it;
	QString srefdates;
	for (it=allDcm.begin(); it!=allDcm.end(); it++) {
		DataCenterModel dcm = *it;
		if (srefdates != "")
			srefdates += "\n";
		srefdates += tr("Reference date: ")
					+ DataCodeStr::toString (dcm)
					+": " 
					+ Util::formatDateTimeLong(reader->getRefDateForDataCenter(dcm));
	}
	QLabel *lbdate = new QLabel(srefdates);
	
	//-------------------------------
	btClose   = new QPushButton(tr("Close"));
	btOptions = new QPushButton(tr("Options"));
	btExport  = new QPushButton(tr("Save"));
    assert(btClose);
    assert(btOptions);
    assert(btExport);
    QFrame *framebts = new QFrame(this);
    assert(framebts);
    QHBoxLayout *hlay = new QHBoxLayout();
    assert(hlay);
        hlay->addWidget(btClose);
        hlay->addWidget(btOptions);
        hlay->addWidget(btExport);
        framebts->setLayout(hlay);
        
	connect(btClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(btOptions, SIGNAL(clicked()), this, SLOT(slotBtOptions()));
	connect(btExport, SIGNAL(clicked()), this, SLOT(slotBtExport()));
	
	//-------------------------------
	mainLayout->addWidget (lbpos);
	mainLayout->addWidget (lbdate);
	mainLayout->addWidget (dataTable);	
	mainLayout->addWidget (framebts, 0, Qt::AlignHCenter);
	
	// taille par défaut pour la 1ère ouverture
	adjustSize();
	int w = 800;
	int h = this->height()+80;
    resize( Util::getSetting("meteoTableDialogSize", QSize(w,h)).toSize() );
	show();	
}
//-----------------------------------------
MeteoTableDialog::~MeteoTableDialog()
{
	Util::setSetting("meteoTableDialogSize", size());
	if (optionsDialog)
		delete optionsDialog;
}

//-----------------------------------------
void MeteoTableDialog::slotBtOptions()
{
	if (! optionsDialog) {
		optionsDialog = new DialogMeteotableOptions();
		assert(optionsDialog);
		connect(optionsDialog, SIGNAL(accepted()), this, SLOT(slotOptionsChanged()));
	}
	optionsDialog->show();
}

//-----------------------------------------
void MeteoTableDialog::slotOptionsChanged()
{
	int index = mainLayout->indexOf (dataTable);
	mainLayout->removeWidget (dataTable);
	dataTable = createDataTable ();
	mainLayout->insertWidget (index, dataTable);
}

//-----------------------------------------
void MeteoTableDialog::slotBtExport ()
{
	QString path = Util::getSetting("slkFilePath", "").toString();
	if (path == "")
		path = "./";
	else
		path += "/";
	QString fileName;
	fileName = Util::getSaveFileName (this,
					tr("Save SYLK file"), path, "*.slk");
    if (fileName != "")
    {
		if (! fileName.endsWith(".slk", Qt::CaseInsensitive))
			fileName += ".slk";
		SylkFile slk (fileName, "zyGrib");
		if (slk.isOk()) {
			Util::setSetting("slkFilePath", slk.getFileInfo().absolutePath() );
			saveFileSYLK (slk);
			slk.close ();
		}
		else {
            QMessageBox::critical (this,
                    tr("Error"),  tr("Can't write file."));
		}
	}
}

//===============================================================
void MeteoTableDialog::saveFileSYLK (SylkFile &slk)
{
	int dl, dc, col;
	
	QList <MTGribData *> listData = meteoTableWidget->getListVisibleData();
	std::vector <DataPointInfo *> lspinfos = meteoTableWidget->getListPinfos(); 
	std::vector <DataPointInfo *>::iterator itp;
	DataPointInfo *pinfo;
	
	if (lspinfos.size() == 0) {
		return;
	}
	//----------------------------------------
	// Position
	pinfo = *(lspinfos.begin());
	slk.addCell (2, 1, Util::formatPosition (pinfo->x,pinfo->y));
	slk.addCell (2, 2, locationName);
	slk.addCell (1, 1, "zyGrib");
	//-----------------------------------------------------------------------
	// Dates
	dl = 2;
	dc = 4;
	for (col=0,itp=lspinfos.begin(); itp!=lspinfos.end(); col++,itp++) {
		DataPointInfo *pinfo = *itp;
		slk.addCell (dl, col+dc, Util::formatDateShort(pinfo->date));
		slk.addCell (dl+1, col+dc, Util::formatTime(pinfo->date));
	}
	//-----------------------------------------------------------------------
	// All Data
	dl = 4;
	dc = 1;
	for (int j=0; j<listData.size(); j++) {
		MTGribData *data = listData.at(j);
		//DBG ("%d %s",data->dtc.dataType,qPrintable(DataCodeStr::toString(data->dtc.dataType)));
		switch (data->dtc.dataType) {
			case GRB_PRV_WIND_XY2D    : 
				dl += SYLK_addData_wind (slk, dl, dc, data->dtc);
				break;
			case GRB_PRV_CUR_XY2D    : 
				dl += SYLK_addData_current (slk, dl, dc, data->dtc);
				break;
			case GRB_CUR_VX      : 
			case GRB_CUR_VY      : 
			case GRB_PRV_CUR_DIR      : 
			case GRB_WIND_VX      : 
			case GRB_WIND_VY      : 
			case GRB_PRV_WIND_DIR     : 
				break;
			case GRB_PRV_WAV_MAX : 
			case GRB_PRV_WAV_SWL : 
			case GRB_PRV_WAV_WND : 
			case GRB_PRV_WAV_PRIM : 
			case GRB_PRV_WAV_SCDY : 
				dl += SYLK_addData_waves (slk, dl, dc, data->dtc);
				break;
			default : 
				dl += SYLK_addData_gen (slk, dl, dc, data->dtc);
		}
	}
}
//-----------------------------------------------------------------------------
int MeteoTableDialog::SYLK_addData_waves (SylkFile &slk, int lig,int col, DataCode dtc)
{
// 	DBG ("%d %s",dtc.dataType,qPrintable(DataCodeStr::toString(dtc.dataType)));
	std::vector <DataPointInfo *> lspinfos = meteoTableWidget->getListPinfos(); 
	std::vector <DataPointInfo *>::iterator itp;
	int curlig;
	QString tht, tdir, tper;
	QString uht, udir, uper;
	uht  = Util::getDataUnit (GRB_WAV_MAX_HT);
	udir = Util::getDataUnit (GRB_WAV_MAX_DIR);
	uper = Util::getDataUnit (GRB_WAV_MAX_PER);
	switch (dtc.dataType) {
		case GRB_PRV_WAV_MAX:
			tht  = DataCodeStr::toString_name (GRB_WAV_MAX_HT);
			tdir = DataCodeStr::toString_name (GRB_WAV_MAX_DIR);
			tper = DataCodeStr::toString_name (GRB_WAV_MAX_PER);
			break;
		case GRB_PRV_WAV_WND:
			tht  = DataCodeStr::toString_name (GRB_WAV_WND_HT);
			tdir = DataCodeStr::toString_name (GRB_WAV_WND_DIR);
			tper = DataCodeStr::toString_name (GRB_WAV_WND_PER);
			break;
		case GRB_PRV_WAV_SWL:
			tht  = DataCodeStr::toString_name (GRB_WAV_SWL_HT);
			tdir = DataCodeStr::toString_name (GRB_WAV_SWL_DIR);
			tper = DataCodeStr::toString_name (GRB_WAV_SWL_PER);
			break;
		case GRB_PRV_WAV_PRIM:
			tht  = "";
			tdir = DataCodeStr::toString_name (GRB_WAV_PRIM_DIR);
			tper = DataCodeStr::toString_name (GRB_WAV_PRIM_PER);
			break;
		case GRB_PRV_WAV_SCDY:
			tht  = "";
			tdir = DataCodeStr::toString_name (GRB_WAV_SCDY_DIR);
			tper = DataCodeStr::toString_name (GRB_WAV_SCDY_PER);
			break;
		default:
			return 0;
	}
	//-----------------------------------------------------------------------
	// Line 1 header: Wave height
	curlig = lig;
	if (tht != "") {
		slk.addCell (curlig, col, tht);
		slk.addCell (curlig, col+1, AltitudeStr::toString(dtc.getAltitude()));
		slk.addCell (curlig, col+2, uht);
		curlig ++;
	}
	//-----------------------------------------------------------------------
	// Line 2 header: Wave direction
	slk.addCell (curlig, col, tdir);
	slk.addCell (curlig, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (curlig, col+2, udir );
	curlig ++;
	//-----------------------------------------------------------------------
	// Line 3 header: Wave period
	slk.addCell (curlig, col, tper);
	slk.addCell (curlig, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (curlig, col+2, uper);
	curlig ++;
	//-----------------------------------------------------------------------
	// Data
	int row = col+3;
	for (itp=lspinfos.begin(); itp!=lspinfos.end(); row++,itp++) 
	{
		DataPointInfo *pf = *itp;
		float ht, per, dir;
		QString txt;
		curlig = lig;
		if (pf->getWaveValues (dtc.dataType, &ht, &per, &dir)) {
			if (tht != "") {
				if (ht != GRIB_NOTDEF) {
					txt = Util::formatWaveHeight (ht, false);
					slk.addCell (curlig, row, txt);
				}
				curlig ++;
			}
			if (dir != GRIB_NOTDEF) {
				txt = Util::formatWaveDirection (dir, false);
				slk.addCell (curlig, row, txt);
			}
			curlig ++;
			if (per != GRIB_NOTDEF) {
				txt = Util::formatWavePeriod (per, false);
				slk.addCell (curlig, row, txt);
			}
			curlig ++;
		}
	}
	return tht == "" ? 2 : 3;
}
//-----------------------------------------------------------------------------
int MeteoTableDialog::SYLK_addData_wind (SylkFile &slk, int lig,int col, DataCode dtc)
{
	std::vector <DataPointInfo *> lspinfos = meteoTableWidget->getListPinfos(); 
	std::vector <DataPointInfo *>::iterator itp;
	int nblig = 2;
	//-----------------------------------------------------------------------
	// Line 1 header: Wind speed
	slk.addCell (lig, col, tr("Wind")+" ("+ tr("speed") +")");
	slk.addCell (lig, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (lig, col+2, Util::getDataUnit(dtc));
	//-----------------------------------------------------------------------
	// Line 2 header: Wind direction
	slk.addCell (lig+1, col, tr("Wind")+" ("+ tr("direction") +")");
	slk.addCell (lig+1, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (lig+1, col+2, tr("°") );
	//-----------------------------------------------------------------------
	// Data
	int row = col+3;
	for (itp=lspinfos.begin(); itp!=lspinfos.end(); row++,itp++) 
	{
		DataPointInfo *pf = *itp;
		float v, dir;
		QString txt;
		if (pf->getWindValues (dtc.getAltitude(), &v, &dir)) {
			txt = Util::formatSpeed_Wind (v, false);
			slk.addCell (lig, row, txt);
			txt = Util::formatDirection (dir, false);
			slk.addCell (lig+1, row, txt);
		}
	}
	return nblig;
}
//-----------------------------------------------------------------------------
int MeteoTableDialog::SYLK_addData_current (SylkFile &slk, int lig,int col, DataCode dtc)
{
	std::vector <DataPointInfo *> lspinfos = meteoTableWidget->getListPinfos(); 
	std::vector <DataPointInfo *>::iterator itp;
	int nblig = 2;
	//-----------------------------------------------------------------------
	// Line 1 header: Current speed
	slk.addCell (lig, col, tr("Current")+" ("+ tr("speed") +")");
	slk.addCell (lig, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (lig, col+2, Util::getDataUnit(dtc));
	//-----------------------------------------------------------------------
	// Line 2 header: Current direction
	slk.addCell (lig+1, col, tr("Current")+" ("+ tr("direction") +")");
	slk.addCell (lig+1, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (lig+1, col+2, tr("°") );
	//-----------------------------------------------------------------------
	// Data
	int row = col+3;
	for (itp=lspinfos.begin(); itp!=lspinfos.end(); row++,itp++) 
	{
		DataPointInfo *pf = *itp;
		float v, dir;
		QString txt;
		if (pf->getCurrentValues (&v, &dir)) {
			txt = Util::formatSpeed_Current (v, false);
			slk.addCell (lig, row, txt);
			txt = Util::formatDirection (dir, false);
			slk.addCell (lig+1, row, txt);
		}
	}
	return nblig;
}
//-----------------------------------------------------------------------------
int MeteoTableDialog::SYLK_addData_gen (SylkFile &slk, int lig,int col, DataCode dtc)
{
	std::vector <DataPointInfo *> lspinfos = meteoTableWidget->getListPinfos(); 
	std::vector <DataPointInfo *>::iterator itp;
	int nblig;
	//-----------------------------------------------------------------------
	// Data Type ; Altitude ; Unit
	//-----------------------------------------------------------------------
	slk.addCell (lig, col, DataCodeStr::toString_name(dtc));
	slk.addCell (lig, col+1, AltitudeStr::toString(dtc.getAltitude()));
	slk.addCell (lig, col+2, Util::getDataUnit(dtc));
		
// 	DBG ("%d %s",dtc.dataType,qPrintable(DataCodeStr::toString(dtc.dataType)));
	int row = col+3;
	for (itp=lspinfos.begin(); itp!=lspinfos.end(); row++,itp++) 
	{
		DataPointInfo *pinfo = *itp;
		float val = pinfo->getDataValue (dtc);
		QString txt = "";
		if (val != GRIB_NOTDEF) {
			switch (dtc.dataType) {
				case GRB_SNOW_CATEG   : 
				case GRB_FRZRAIN_CATEG: 
					txt = Util::formatCategoricalData (val); 
					break;
				case GRB_TEMP_POT     : 
				case GRB_TMAX         : 
				case GRB_TMIN         : 
				case GRB_DEWPOINT     : 
				case GRB_TEMP         : 
				case GRB_PRV_THETA_E      : 
					txt = Util::formatTemperature (val, false); 
					break;
				case GRB_PRV_DIFF_TEMPDEW : 
					txt = Util::formatTemperature (val+273.15, false); 
					break;
				case GRB_SNOW_DEPTH   : 
					txt = Util::formatSnowDepth (val, false); 
					break;
				case GRB_CLOUD_TOT    : 
				case GRB_HUMID_SPEC   : 
				case GRB_HUMID_REL    : 
					txt = Util::formatPercentValue (val, false); 
					break;
				case GRB_GEOPOT_HGT   :
					txt = Util::formatGeopotAltitude (val, false); 
					break;
				case GRB_CAPE 		  : 
					txt = Util::formatCAPEsfc (val, false); 
					break;
				case GRB_CIN 		  : 
					txt = Util::formatCAPEsfc (val, false); 
					break;
				case GRB_WIND_GUST	  : 
					txt = Util::formatSpeed_Wind (val, false); 
					break;
				case GRB_PRESSURE_MSL : 
					txt = Util::formatPressure (val, false); 
					break;
				case GRB_PRECIP_TOT   : 
				case GRB_PRECIP_RATE  : 
					txt = Util::formatRain (val, false); 
					break;
				case GRB_WAV_WHITCAP_PROB    : 
					txt = Util::formatPercentValue (val, false); 
					break;
				case GRB_WAV_SIG_HT : 
					txt = Util::formatWaveHeight (val, false); 
					break;
			}
			slk.addCell (lig, row, txt);
		}
	}
	nblig = 1;
	return nblig;
}

    




