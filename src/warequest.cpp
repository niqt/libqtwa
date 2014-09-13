/* Copyright 2013 Naikel Aparicio. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL EELI REILIN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the author and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 */

#include "warequest.h"

WARequest::WARequest(const QString &useragent, const QString &method, QObject *parent):
    QObject(parent),
    _method(method),
    _userAgent(useragent)
{
    urlQuery.clear();
    reply = 0;
    nam = new QNetworkAccessManager(this);
}

void WARequest::getRequest()
{
    if (nam) {
        QUrl reqUrl(QUrl(URL_REGISTRATION_V2 + _method));
        reqUrl.setQuery(urlQuery);
        QNetworkRequest request(reqUrl);
        request.setRawHeader("User-Agent", _userAgent);
        request.setRawHeader("Connection", "closed");
        qDebug() << "User-Agent" << _userAgent;
        qDebug() << "WARequest" << _method << "request:" << request.url().toString();

        reply = nam->get(request);
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
        QObject::connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(sslErrors(const QList<QSslError>)));
    }
}

void WARequest::onResponse()
{
    if (reply->error() == QNetworkReply::NoError) {
        if (reply->bytesAvailable())
            readResult();
        else {
            if (reply) {
                QObject::connect(reply,SIGNAL(readyRead()),this,SLOT(readResult()));
            }
            else {
                //this->deleteLater();
                qDebug() << "broken reply";
            }
        }
    }
    else {
        emit httpError(reply->errorString());
    }
}

void WARequest::readResult()
{
    QByteArray json = reply->readAll();

    // Debugging info
    qDebug() << "Reply:" << json;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error == QJsonParseError::NoError) {
        QVariantMap mapResult = doc.toVariant().toMap();
        emit finished(mapResult);
    }
    else
        emit httpError("Cannot parse json reply");
}

void WARequest::addParam(QString name, QString value)
{
    urlQuery.addQueryItem(name, QUrl::toPercentEncoding(value));
}

void WARequest::error(QNetworkReply::NetworkError code)
{
    qDebug() << "WARequest network error:" << reply->errorString() << QString::number(code);
    emit httpError(reply->errorString());
}

void WARequest::sslErrors(const QList<QSslError> &errors)
{
    qDebug() << "WARequest ssl error";
    Q_UNUSED(errors);
    emit sslError();
}
