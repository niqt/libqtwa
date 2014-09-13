#ifndef MEDIADOWNLOAD_H
#define MEDIADOWNLOAD_H

#include <QFile>

#include "fmessage.h"
#include "httprequestv2.h"

#include "libqtwa.h"

class LIBQTWA MediaDownload : public HttpRequestv2
{
    Q_OBJECT
public:
    explicit MediaDownload(FMessage message, const QString &useragent, bool downloadToGallery = true, QObject *parent = 0);

private slots:
    void onSocketError(const QString &errorString);
    void onResponse();
    void writeToFile();

signals:
    void progress(FMessage msg, float p);
    void downloadFinished(MediaDownload *,FMessage msg);
    void httpError(MediaDownload *, FMessage msg, const QString &errorString);

public slots:
    void backgroundTransfer();

private:
    FMessage message;
    QString fileName;
    QFile file;
    qint64 totalLength;
    qint64 bytesWritten;
};

#endif // MEDIADOWNLOAD_H
