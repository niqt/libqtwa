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

#ifndef PHONEREG_H
#define PHONEREG_H

#include <QObject>

#include "warequest.h"

#include "libqtwa.h"

class LIBQTWA PhoneReg : public QObject
{
    Q_OBJECT
public:
    explicit PhoneReg(const QString& userAgent,
                      const QString& cc, const QString& number, const QString& method = "sms",
                      const QString& smscode = "", const QString& password = "",
                      const QString& mcc = "", const QString& mnc = "", QObject *parent = 0);

signals:
    void finished(const QVariantMap& reply);
    void expired(const QVariantMap& result);
    void smsTimeout(int timeout);
    void existsFailed(const QVariantMap& reply);
    void codeRequested(const QVariantMap& reply);
    void codeFailed(const QVariantMap& reply);

public slots:
    void requestConfirmation();
    void requestRegistration();
    void codeReceived(const QString &code);

private slots:
    void onCodeRequestDone(const QVariantMap &result);
    void onRegRequestDone(const QVariantMap &result);
    void errorHandler(const QString &errorString);
    void sslError();

private:
    QString method;
    QString number;
    QString cc;
    QString id;
    QString smscode;
    QString mcc;
    QString mnc;
    QString userAgent;

    WARequest *request;

    void startRegRequest();
    void startCodeRequest();
};

#endif // PHONEREG_H
