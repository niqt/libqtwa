#include "mediadownload.h"

#include "util/utilities.h"
#include "globalconstants.h"

MediaDownload::MediaDownload(FMessage message, QString &useragent, bool downloadToGallery, QObject *parent) :
    HttpRequestv2(useragent, parent)
{
    this->message = message;

    fileName = Utilities::getSaveNameFor(message.media_name, message.media_wa_type, downloadToGallery);
    file.setFileName(fileName);
}

void MediaDownload::backgroundTransfer()
{
    connect(this,SIGNAL(finished()),
            this,SLOT(onResponse()));

    qDebug() << "Download media:" << message.media_url << "to" << fileName;
    get(message.media_url);

    emit progress(message, 0.01);
}

void MediaDownload::onSocketError(const QString &errorString)
{
    qDebug() << "Media download socket error:" << errorString;
    Q_EMIT httpError(this, message, errorString);
}

void MediaDownload::onResponse()
{
    qDebug() << "HTTP Response. Bytes available:" << QString::number(socket->bytesAvailable());

    if (errorCode != 200)
    {
        emit httpError(this, message, errorCode);
        return;
    }

    totalLength = getHeader("Content-Length").toLongLong();
    bytesWritten = 0;

    if (!file.open(QIODevice::WriteOnly))
    {
        // An error has occurred
        qDebug() << "MediaDownload: Error while trying to opening file:" << fileName;
        socket->close();
    }

    if (socket->bytesAvailable())
        writeToFile();

    connect(socket, SIGNAL(readyRead()), this, SLOT(writeToFile()));
}

void MediaDownload::writeToFile()
{
    qint64 bytesToRead = socket->bytesAvailable();

    QByteArray buffer;
    buffer.resize(bytesToRead);

    if (bytesToRead + bytesWritten > totalLength)
        bytesToRead = totalLength - bytesWritten;

    if (file.write(socket->read(bytesToRead)) != bytesToRead)
    {
        // An error has occurred
        qDebug() << "MediaDownload: Error while trying to opening file";
        file.close();
        socket->close();
    }

    bytesWritten += bytesToRead;

    float p = ((float)((bytesWritten) * 100.0)) / ((float)totalLength);

    emit progress(message,p);

    if (bytesWritten == totalLength)
    {
        file.close();
        socket->close();

        qDebug() << "MediaDownload: Downloading finished.";

        message.local_file_uri = fileName;

        emit downloadFinished(this, message);
    }
}
