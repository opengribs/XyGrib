#include "DialogLoadGribMeteoFrance.h"
#include "GUI/ui_LoadGribMeteoFranceDialog.h"


//-------------------------------------------------------------------------------
QString DialogLoadGRIBMeteoFrance::getFile(QNetworkAccessManager *networkManager,
                                           QWidget *parent, double x0, double y0,
                                           double x1, double y1)
{
    /* Static method that loads the Dialog Window
     * with downloading options for the Meteo France GRIB,
     * and return the filename of the GRIB file.
     */
    DialogLoadGRIBMeteoFrance *globalDiag = new DialogLoadGRIBMeteoFrance(networkManager, parent);
    globalDiag->setZone(x0, y0, x1, y1);
    globalDiag->exec();
    return globalDiag->m_fullPathFileName;
}


DialogLoadGRIBMeteoFrance::DialogLoadGRIBMeteoFrance(QNetworkAccessManager *networkManager,
                                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoadGRIBMeteoFrance)
{
    ui->setupUi(this);
    m_networkManager = networkManager;

    // Init progress bar
    ui->progressBar->setValue(0);

    // Save old cursor before we change it
    m_oldCursor = cursor();
    m_waitCursor = Qt::WaitCursor;
}

DialogLoadGRIBMeteoFrance::~DialogLoadGRIBMeteoFrance()
{
    delete ui;
    delete m_networkManager;
}

void DialogLoadGRIBMeteoFrance::setZone(double lon_min, double lat_min,
                                        double lon_max, double lat_max)
{
    double tmp;
    if (lon_min > lon_max)  { tmp=lon_min; lon_min=lon_max; lon_max=tmp; }
    if (lat_max > lat_min)  { tmp=lat_min; lat_min=lat_max; lat_max=tmp; }

    m_lat_min = (int)ceil(lat_min);
    m_lon_min = (int)floor(lon_min);
    m_lat_max = (int)floor(lat_max);
    m_lon_max = (int)ceil(lon_max);
}

// SLOTS

void DialogLoadGRIBMeteoFrance::slotBtOK()
{
    // Slot to be run when button 'OK'
    // is clicked by the user.

    // Start the loading bar
    // When setMinimum and setMaximum are set to 0 both, then QProgressBar only
    // displays a busy indicator as MeteoFrance GRIB server does not send the
    // content-length header...
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);

    // Display downloading status
    ui->downloadingInfo->setText("Meteo France's servers are very slow. Please be patient while downloading...");

    // Change cursor to waiting cursor
    setCursor(m_waitCursor);

    // Disable OK button while downloading
    ui->buttonBoxOK->setEnabled(false);

    // Start downloading
    // By default it is the arpege model already selected
    MeteoFranceModel *model = NULL;

    if (ui->radioArome->isChecked())
    {
        Arome *modelArome = new Arome(m_networkManager,
                                      m_lat_min, m_lon_min, m_lat_max, m_lon_max);
        model = modelArome;
    }
    else
    {
        Arpege *modelArpege = new Arpege(m_networkManager,
                                         m_lat_min, m_lon_min, m_lat_max, m_lon_max);
        model = modelArpege;
    }

    model->download();

    // Connections
    connect(this, SIGNAL(rejected()), model, SLOT(slotAbortDownload()));
    connect(model, SIGNAL(signalFullPathAbort()), this, SLOT(slotFullPathAbort()));
    connect(model, SIGNAL(signalErrorSaveToDisk(QString)), this, SLOT(slotErrorSaveToDisk(QString)));
    connect(model, SIGNAL(signalErrorNetwork(QString)), this, SLOT(slotNetworkError(QString)));
    connect(model, SIGNAL(signalGribSaved(QString)), this, SLOT(slotGribSaved(QString)));
}

void DialogLoadGRIBMeteoFrance::slotGribSaved(QString fullPathFileName)
{
    m_fullPathFileName = fullPathFileName;
    setCursor(m_oldCursor);
    accept();
}

void DialogLoadGRIBMeteoFrance::slotFullPathAbort()
{
    setCursor(m_oldCursor);
    ui->buttonBoxOK->setEnabled(true);
}

void DialogLoadGRIBMeteoFrance::slotErrorSaveToDisk(QString msg)
{
    QMessageBox::critical(this, "GRIB error", msg);
    // Error dialog is displayed also when cancel button is clicked
}

void DialogLoadGRIBMeteoFrance::slotNetworkError(QString msg)
{
    QMessageBox::critical(this, "Network Error", msg);
    // Error dialog is displayed also when cancel button is clicked
}

