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

#include <QCryptographicHash>
#include <QImageReader>
#include <QImage>
#include <QBuffer>

#include "mediaupload.h"
#include "formdata.h"
#include "src/client.h"

#include "util/utilities.h"

#include <QLibrary>

typedef QImage (*CreateThumbnailFunc)(const QString &fileName, const QSize &requestedSize, bool crop);

MediaUpload::MediaUpload(const FMessage &message, QObject *parent) :
    QObject(parent)
{
    _pendingMsg = message;
}

QString MediaUpload::generateMediaFilename(QString extension)
{
    QByteArray bytes;

    bytes.append(Client::phoneNumber.toUtf8());
    bytes.append(QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8());

    QByteArray hashed = QCryptographicHash::hash(bytes, QCryptographicHash::Sha1).toHex();
    hashed.append(QString("." + extension).toUtf8());

    return QString::fromUtf8(hashed);
}

QImage MediaUpload::generateThumbnail(const QString &fileName, const QSize &requestedSize, bool crop)
{
    QImage image;

    static CreateThumbnailFunc createThumbnail = (CreateThumbnailFunc)QLibrary::resolve(
                QLatin1String("/usr/lib/qt5/qml/org/nemomobile/thumbnailer/thumbnailers/libvideothumbnailer.so"), "createThumbnail");

    if (createThumbnail) {
        image = createThumbnail(fileName, requestedSize, crop);
    } else {
        qWarning("Cannot generate video thumbnail, thumbnailer function not available.");
    }

    return image;
}


void MediaUpload::sendPicture(QStringList jids, MediaDescriptor descriptor)
{
    qDebug() << "loading image:" << descriptor.localFileUri;
    QImageReader reader(descriptor.localFileUri);
    QRect clip;
    if (reader.size().width() > reader.size().height()) {
        clip.setTop(0);
        clip.setLeft((reader.size().width() - reader.size().height()) / 2);
        clip.setWidth(reader.size().height());
        clip.setHeight(reader.size().height());
    }
    else {
        clip.setLeft(0);
        clip.setTop((reader.size().height() - reader.size().width()) / 2);
        clip.setWidth(reader.size().width());
        clip.setHeight(reader.size().width());
    }
    reader.setClipRect(clip);
    reader.setScaledSize(QSize(100, 100));
    qDebug() << "creating preview";

    QImage picture = reader.read();

    descriptor.data.clear();
    QBuffer out(&descriptor.data);
    out.open(QIODevice::WriteOnly);
    qDebug() << "saving preview";
    picture.save(&out, "JPG");

    sendMedia(jids, descriptor);
}


void MediaUpload::sendVideo(QStringList jids, MediaDescriptor descriptor)
{
    qDebug() << "loading video:" << descriptor.localFileUri;
    QImage picture = generateThumbnail(descriptor.localFileUri, QSize(100, 100), true);
    if (picture.isNull()) {
        QString thumbnail = "/usr/share/harbour-mitakuuluu2/images/thumbnail-video.jpg";
        picture.load(thumbnail);
        picture = picture.scaled(64, 64, Qt::KeepAspectRatio);
    }
    descriptor.data.clear();
    QBuffer out(&descriptor.data);
    out.open(QIODevice::WriteOnly);
    picture.save(&out, "JPG");

    sendMedia(jids, descriptor);
}

void MediaUpload::sendMedia(QStringList jids, FMessage message)
{
    MediaDescriptor descriptor;

    QStringList vjids = jids;
    if (jids.isEmpty())
        vjids << message.remote_resource;

    descriptor.waType = (FMessage::MediaWAType) message.media_wa_type;
    descriptor.extension = Utilities::getExtension(message.media_name);
    descriptor.duration = message.media_duration_seconds;
    descriptor.contentType =
            message.media_mime_type.isEmpty() ?
                Utilities::guessMimeType(message.media_name) :
                message.media_mime_type;
    descriptor.localFileUri = message.media_name;
    descriptor.fileName = message.local_file_uri;
    descriptor.url = message.media_url;
    descriptor.upload = (message.status == FMessage::Uploading);
    descriptor.live = message.live;

    switch (descriptor.waType)
    {
        case FMessage::Image:
            sendPicture(vjids, descriptor);
            break;

        case FMessage::Video:
            sendVideo(vjids, descriptor);
            break;

        case FMessage::Audio:
            sendMedia(vjids, descriptor);

        default:
            break;
    }
}

void MediaUpload::sendMedia(QStringList jids, MediaDescriptor descriptor)
{
    QString jid = jids.size() > 1 ? "broadcast" : jids.first();

    msg = FMessage(jid, true);

    QFile file(descriptor.localFileUri);

    msg.type = FMessage::MediaMessage;
    msg.data = descriptor.data;
    msg.thumb_image = QString::fromLatin1(msg.data.toBase64());
    msg.media_size =  file.size();
    msg.media_wa_type = descriptor.waType;
    msg.media_mime_type = descriptor.contentType;
    msg.remote_resource = jid;
    msg.key.remote_jid = jid;
    msg.local_file_uri = descriptor.fileName;
    msg.media_name = generateMediaFilename(descriptor.extension);
    msg.media_duration_seconds = descriptor.duration;
    msg.live = descriptor.live;
    if (jids.size() > 1) {
        msg.broadcast = true;
        msg.broadcastJids = jids;
    }

    // This will be overwritten if a media upload is required with
    // the final URL
    msg.media_url = descriptor.url;

    // ToDo: Save tmp file for persistence sending.

    msg.status = FMessage::Uploading;
    msg.data = msg.data.toBase64();
    emit readyToSendMessage(this,msg);
    msg.data = QByteArray::fromBase64(msg.data);

    if (descriptor.upload)
    {
        uploadMedia();
    }
    else
    {
        msg.status = FMessage::Uploaded;
        emit sendMessage(this,msg);
    }
}

void MediaUpload::uploadMedia()
{
    QList<FormData*> formData;

    FormDataString *str = new FormDataString();
    str->name = "from";
    str->content = Client::myJid;

    formData.append(str);

    FormDataString *str2 = new FormDataString();
    str2->name = "to";
    str2->content = msg.remote_resource;

    formData.append(str2);

    FormDataFile *file = new FormDataFile();
    file->name = "file";
    file->fileName = msg.media_name;
    QString scaled = QString("/var/tmp/%1").arg(msg.local_file_uri.split("/").last());
    if (QFile(scaled).exists())
        file->uri = scaled;
    else
        file->uri = msg.local_file_uri;
    file->contentType = msg.media_mime_type;

    qDebug() << "Uploading media:" << file->fileName << file->uri;

    formData.append(file);

    MultiPartUploader *uploader = new MultiPartUploader(this);

    connect(uploader,SIGNAL(finished(MultiPartUploader*,QVariantMap)),
            this,SLOT(finished(MultiPartUploader*,QVariantMap)));
    connect(uploader,SIGNAL(socketError(QAbstractSocket::SocketError)),
            this,SLOT(errorHandler(QAbstractSocket::SocketError)));
    connect(uploader,SIGNAL(progress(float)),
            this,SLOT(updateProgress(float)));
    connect(uploader,SIGNAL(headersReceived(qint64)),
            this,SLOT(headersReceivedHandler(qint64)));
    connect(uploader,SIGNAL(requestSent(qint64)),
            this,SLOT(requestSentHandler(qint64)));


    // uploader->open("https://mms.whatsapp.net/client/iphone/upload.php", formData);
    uploader->open(msg.media_url, formData);
}

void MediaUpload::finished(MultiPartUploader *uploader, QVariantMap dictionary)
{
    disconnect(uploader, 0, 0, 0);
    uploader->deleteLater();

    if (dictionary.contains("url"))
    {
        msg.media_mime_type = dictionary.value("mimetype").toString();
        msg.media_name = dictionary.value("name").toString();
        msg.media_size = dictionary.value("size").toLongLong();
        msg.media_url = dictionary.value("url").toString();
        msg.media_duration_seconds = dictionary.value("duration").toInt();
        msg.status = FMessage::Uploaded;

        qDebug() << "Upload finished:" << msg.media_name << "size:" << QString::number(msg.media_size);
        qDebug() << "Url:" << msg.media_url;

        emit sendMessage(this, msg);
    }
    else if (dictionary.contains("error"))
    {
        qDebug() << "Upload failed:" << dictionary.value("error").toString();
        emit httpError(this, msg);
    }
}


void MediaUpload::errorHandler(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::SslHandshakeFailedError)
    {
        // SSL error is a fatal error
        emit sslError(this, msg);
    }
    else
    {
        // ToDo: Retry here
        qDebug() << "Upload failed: Socket error" << QString::number(error);
        emit httpError(this, msg);
    }
}

void MediaUpload::updateProgress(float p)
{
    emit progress(msg,p);
}

void MediaUpload::requestSentHandler(qint64 bytes)
{
    emit requestSent(bytes);
}

void MediaUpload::headersReceivedHandler(qint64 bytes)
{
    emit headersReceived(bytes);
}

void MediaUpload::upload()
{
    sendMedia(_pendingMsg.broadcastJids, _pendingMsg);
}

