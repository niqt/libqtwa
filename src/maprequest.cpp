#include "maprequest.h"

MapRequest::MapRequest(const QString &source, const QString &latitude, const QString &longitude,
                       int zoom, int width, int height, const QStringList &jids, QObject *parent) :
    QObject(parent),
    _jids(jids),
    _latitude(latitude),
    _longitude(longitude)
{
    _nam = new QNetworkAccessManager(this);

    if (source == "google") {
        QUrlQuery path;
        path.addQueryItem("maptype", "roadmap");
        path.addQueryItem("sensor", "false");
        path.addQueryItem("zoom", QString::number(zoom));
        path.addQueryItem("size", QString("%1x%2").arg(width).arg(height));
        path.addQueryItem("center", QString("%1,%2").arg(latitude).arg(longitude));
        QUrl url("http://maps.googleapis.com/maps/api/staticmap");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "here") {
        QUrlQuery path;
        path.addQueryItem("app_id", "ZXpeEEIbbZQHDlyl5vEn");
        path.addQueryItem("app_code", "GQvKkpzHomJpzKu-hGxFSQ");
        path.addQueryItem("nord", "");
        path.addQueryItem("f", "0");
        path.addQueryItem("z", QString::number(zoom));
        path.addQueryItem("w", QString::number(width));
        path.addQueryItem("h", QString::number(height));
        path.addQueryItem("ctr", QString("%1,%2").arg(latitude).arg(longitude));
        QUrl url("https://maps.nlp.nokia.com/mia/1.6/mapview");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "nokia") {
        QUrlQuery path;
        path.addQueryItem("nord", "");
        path.addQueryItem("f", "0");
        path.addQueryItem("z", QString::number(zoom));
        path.addQueryItem("w", QString::number(width));
        path.addQueryItem("h", QString::number(height));
        path.addQueryItem("ctr", QString("%1,%2").arg(latitude).arg(longitude));
        QUrl url("http://m.nok.it");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "osm") {
        QUrlQuery path;
        path.addQueryItem("maptype", "mapnik");
        path.addQueryItem("zoom", QString::number(zoom));
        path.addQueryItem("size", QString("%1x%2").arg(width).arg(height));
        path.addQueryItem("center", QString("%1,%2").arg(latitude).arg(longitude));
        QUrl url("https://coderus.openrepos.net/staticmaplite/staticmap.php");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "bing") {
        QUrlQuery path;
        path.addQueryItem("key", "AvkH1TAJ9k4dkzOELMutZbk_t3L4ImPPW5LXDvw16XNRd5U36a018XJo2Z1jsPbW");
        path.addQueryItem("mapSize", QString("%1,%2").arg(width).arg(height));
        QUrl url(QString("http://dev.virtualearth.net/REST/v1/Imagery/Map/Road/%1,%2/%3").arg(latitude).arg(longitude).arg(zoom));
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "mapquest") {
        QUrlQuery path;
        path.addQueryItem("key", "Fmjtd%7Cluur2q0y2q%2Cbw%3Do5-9abn5f");
        path.addQueryItem("type", "map");
        path.addQueryItem("imagetype", "png");
        path.addQueryItem("zoom", QString::number(zoom));
        path.addQueryItem("size", QString("%1,%2").arg(width).arg(height));
        path.addQueryItem("center", QString("%1,%2").arg(latitude).arg(longitude));
        QUrl url("http://www.mapquestapi.com/staticmap/v4/getmap");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "yandexuser") {
        QUrlQuery path;
        path.addQueryItem("l", "pmap");
        path.addQueryItem("z", QString::number(zoom));
        path.addQueryItem("size", QString("%1,%2").arg(width).arg(height));
        path.addQueryItem("ll", QString("%1,%2").arg(longitude).arg(latitude));
        QUrl url("http://static-maps.yandex.ru/1.x");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "yandex") {
        QUrlQuery path;
        path.addQueryItem("l", "map");
        path.addQueryItem("z", QString::number(zoom));
        path.addQueryItem("size", QString("%1,%2").arg(width).arg(height));
        path.addQueryItem("ll", QString("%1,%2").arg(longitude).arg(latitude));
        QUrl url("http://static-maps.yandex.ru/1.x");
        url.setQuery(path);
        _query.setUrl(url);
    }
    else if (source == "2gis") {
        QUrlQuery path;
        path.addQueryItem("zoom", QString::number(zoom));
        path.addQueryItem("size", QString("%1,%2").arg(width).arg(height));
        path.addQueryItem("center", QString("%1,%2").arg(longitude).arg(latitude));
        QUrl url("http://static.maps.api.2gis.ru/1.0");
        url.setQuery(path);
        _query.setUrl(url);
    }
}

void MapRequest::doRequest()
{
    connect(_nam->get(_query), SIGNAL(finished()), this, SLOT(requestComplete()));
}

void MapRequest::requestComplete()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply && reply->error() == QNetworkReply::NoError) {
        Q_EMIT mapAvailable(reply->readAll(), _latitude, _longitude, _jids, this);
    }
    else {
        Q_EMIT requestError(this);
    }
}
