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

#ifndef MEDIAUPLOAD_H
#define MEDIAUPLOAD_H

#include <QHash>

#include "fmessage.h"
#include "multipartuploader.h"

#include "libqtwa.h"

class LIBQTWA MediaUpload : public QObject
{
    Q_OBJECT

    struct MediaDescriptor
    {
        QString extension;
        QByteArray data;
        QString localFileUri;
        QString fileName;
        QString contentType;
        FMessage::MediaWAType waType;
        QString url;
        bool upload;
        qint64 duration;
        bool live;
    };


public:
    friend class WhatsappTransfer;
    explicit MediaUpload(const FMessage &message, QObject *parent = 0);

    void sendPicture(QStringList jids, MediaDescriptor descriptor);
    void sendVideo(QStringList jids, MediaDescriptor descriptor);
    void sendMedia(QStringList jids, FMessage message);
    void sendMedia(QStringList jids, MediaDescriptor descriptor);
    void uploadMedia();

signals:
    void sendMessage(MediaUpload *obj, FMessage msg);
    void readyToSendMessage(MediaUpload *obj, FMessage msg);
    void sslError(MediaUpload *obj, FMessage msg);
    void progress(FMessage msg, float p);
    void httpError(MediaUpload *obj, FMessage msg);
    void requestSent(qint64 bytes);
    void headersReceived(qint64 bytes);


private slots:
    void finished(MultiPartUploader *uploader, QVariantMap dictionary);
    void errorHandler(QAbstractSocket::SocketError error);
    void updateProgress(float p);
    void requestSentHandler(qint64 bytes);
    void headersReceivedHandler(qint64 bytes);
    void upload();

private:
    FMessage msg;
    FMessage _pendingMsg;

    QString generateMediaFilename(QString extension);
    QImage generateThumbnail(const QString &fileName, const QSize &requestedSize, bool crop);

};

#endif // MEDIAUPLOAD_H
