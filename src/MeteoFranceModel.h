#ifndef METEOFRANCEMODEL_H
#define METEOFRANCEMODEL_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>


class MeteoFranceModel: public QObject
{
    Q_OBJECT

public:
    MeteoFranceModel(QNetworkAccessManager *networkManager,
                     int lat_min, int lon_min, int lat_max, int lon_max);
    ~MeteoFranceModel() { }
    virtual void getEndpoint() = 0;     // Abstract method
    void download();
    QString getPartialFileName();
    virtual QString getFileName() = 0; // Abstract method
    QString getFullPathFileName(const QString &filename);
    bool saveToDisk(const QString &fullPath, QIODevice *data);

public slots:
    void slotFinished();
    void slotAbortDownload();
    void slotNetworkError(QNetworkReply::NetworkError);

signals:
    void signalGribSaved(QString);
    void signalErrorSaveToDisk(QString);
    void signalErrorNetwork(QString);
    void signalFullPathAbort();

protected:
    QNetworkAccessManager *m_networkManager;
    int m_lat_min;
    int m_lat_max;
    int m_lon_min;
    int m_lon_max;
    QString m_args;
    QUrl m_api;
    QNetworkReply *m_reply;
    bool m_error;
    bool m_abort;
};


class Arpege: public MeteoFranceModel
{
public:
    Arpege(QNetworkAccessManager *networkManager,
           int lat_min, int lon_min, int lat_max, int lon_max);
    ~Arpege() { }
    void getEndpoint();
    QString getFileName();
};


class Arome: public MeteoFranceModel
{
public:
    Arome(QNetworkAccessManager *networkManager,
           int lat_min, int lon_min, int lat_max, int lon_max);
    ~Arome() { }
    void getEndpoint();
    QString getFileName();
};

#endif // METEOFRANCEMODEL_H
