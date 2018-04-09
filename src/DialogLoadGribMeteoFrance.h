#ifndef DIALOGLOADGRIBMETEOFRANCE_H
#define DIALOGLOADGRIBMETEOFRANCE_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QMessageBox>

#include "MeteoFranceModel.h"


namespace Ui {
class DialogLoadGRIBMeteoFrance;
}

class DialogLoadGRIBMeteoFrance : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLoadGRIBMeteoFrance(QNetworkAccessManager *manager, QWidget *parent = 0);
    ~DialogLoadGRIBMeteoFrance();
    static QString getFile(QNetworkAccessManager *netManager, QWidget *parent,
                           double x0, double y0, double x1, double y1);
    void setZone(double lat_min, double lon_min, double lat_max, double lon_max);

public slots:
    void slotBtOK();
    void slotGribSaved(QString);
    void slotFullPathAbort();
    void slotErrorSaveToDisk(QString);
    void slotNetworkError(QString);

private:
    Ui::DialogLoadGRIBMeteoFrance *ui;
    QNetworkAccessManager *m_networkManager;
    int m_lat_min;
    int m_lat_max;
    int m_lon_min;
    int m_lon_max;
    QString m_fullPathFileName;
    QCursor m_waitCursor;
    QCursor m_oldCursor;
};

#endif // DIALOGLOADGRIBMETEOFRANCE_H
