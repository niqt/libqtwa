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

#include <QFile>

#include "src/client.h"

#include "utilities.h"
#include "qtmd5digest.h"

#include "src/globalconstants.h"
#include "src/Whatsapp/fmessage.h"

#include <QRegExp>
#include <QStringList>
#include <QDir>
#include <QStandardPaths>

#include <QCryptographicHash>
#include <QMimeDatabase>

//#define MIMETYPES_FILE  "/usr/share/harbour-mitakuuluu2/data/mime-types.tab"

Utilities::Utilities()
{
}

QString Utilities::getTokenAndroid(const QString &phoneNumber)
{
    QByteArray ipad = QByteArray::fromBase64(QByteArray(ANDROID_S1));
    QByteArray opad = QByteArray::fromBase64(QByteArray(ANDROID_S2));
    QByteArray data = QByteArray::fromBase64(QByteArray(ANDROID_S3));
    data.append(QByteArray::fromBase64(Client::wandroidscratch1.toLatin1()));
    data.append(phoneNumber.toLatin1());
    ipad.append(data);

    QCryptographicHash *sha1 = new QCryptographicHash(QCryptographicHash::Sha1);
    sha1->reset();
    sha1->addData(ipad);
    opad.append(sha1->result());
    sha1->reset();
    sha1->addData(opad);

    QString token = QString::fromLatin1(sha1->result().toBase64());
    delete sha1;

    return token;
}

QString Utilities::getTokenNokia(const QString &phoneNumber)
{
    QString token = Client::wanokiascratch1 + Client::wanokiascratch2 + phoneNumber;

    QtMD5Digest digest;
    digest.reset();

    digest.update(token.toUtf8());

    QByteArray bytes = digest.digest();

    return QString::fromLatin1(bytes.toHex().constData());
}

QString Utilities::guessMimeType(const QString &fileName)
{
    /*
    QString lower = getExtension(fileName);

    QFile file(MIMETYPES_FILE);
    if (file.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(QChar(0x09));
            if (list[0] == lower && list.length() > 1)
            {
                file.close();
                return list[1];
            }
        }

        file.close();
    }

    // if this happens then Yappari was hacked
    return "application/unknown";
    */

    QMimeDatabase md;
    return md.mimeTypeForFile(fileName);
}

QString Utilities::getExtension(const QString &filename)
{
    QFileInfo fi(filename);
    return fi.suffix();
}

QString Utilities::getPathFor(int media_wa_type, bool gallery)
{
    QString folder;

    if (gallery) {
        switch (media_wa_type)
        {
            case FMessage::Audio:
                folder = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
                break;
            case FMessage::Image:
                folder = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
                break;
            case FMessage::Video:
                folder = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
                break;
        }

        folder = folder + DATA_DIR;
    }
    else {
        switch (media_wa_type)
        {
            case FMessage::Audio:
                folder = AUDIO_DIR;
                break;
            case FMessage::Image:
                folder = IMAGES_DIR;
                break;
            case FMessage::Video:
                folder = VIDEOS_DIR;
                break;
        }
        folder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + folder;
    }

    if (!QDir::home().exists(folder))
        QDir::home().mkpath(folder);

    return folder;
}

QString Utilities::getSaveNameFor(const QString &media, int media_wa_type, bool gallery)
{
    QString path = (media_wa_type, gallery);

    QDir home = QDir::home();
    if (!home.exists(path))
        home.mkpath(path);

    // Let's try to be organized here with the downloads
    /*if (Client::importMediaToGallery)
    {
        path.append(WHATSAPP_DIR);
        QDir home = QDir::home();
        if (!home.exists(path))
            home.mkpath(path);
    }*/

    QString fileName = path + "/" + media;
    int pos = fileName.lastIndexOf('.');
    if (pos < 0)
        pos = fileName.length();

    QString extension = fileName.mid(pos);

    // Loop until a fileName is not currently being used
    QFile file(fileName);
    for (int count = 1; file.exists(); count++)
        file.setFileName(fileName.left(pos) + "_" + QString::number(count) + extension);

    return file.fileName();
}
