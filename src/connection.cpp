/**
 * Copyright (C) 2013 Naikel Aparicio. All rights reserved.
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

#include <QRegExp>
#include <QUuid>
#include <QTime>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "util/qtmd5digest.h"
#include "util/utilities.h"
#include "util/datetimeutilities.h"
#include "protocoltreenodelistiterator.h"

#include "globalconstants.h"

#include "connection.h"

FunStore Connection::store;

/**
    Constructs a Connection object.

    @param socket       A QTcpSocket with a established TCP connection.
    @param domain       Usually "s.whatsapp.net"
    @param resource     String that describes the client used.
    @param user         Phone number.
    @param push_name    User name or alias.
    @param password     Password.
    @param counters     Pointer to a DataCounters class whre counters are kept.
    @param parent       QObject parent.
*/
Connection::Connection(const QString &server, int port, const QString &domain, const QString &resource,
                       const QString &user, const QString &push_name, const QByteArray &password,
                       const QByteArray &challenge, const QString &language, const QString &country,
                       const QString &mcc, const QString &mnc, const QString version,
                       DataCounters *counters, QObject *parent)
    : QObject(parent)
{

    /*
     * This is the dictionary Whatsapp uses to compress its data
     * so the packets are really tiny
     */

    dictionary
        << NULL << NULL << NULL
        << "account" << "ack" << "action" << "active" << "add" << "after" << "all" << "allow" << "apple" << "auth" << "author" << "available" << "bad-protocol" << "bad-request" << "before" << "body" << "broadcast" << "cancel" << "category" << "challenge" << "chat" << "clean" << "code" << "composing" << "config" << "contacts" << "count" << "create" << "creation" << "debug" << "default" << "delete" << "delivery" << "delta" << "deny" << "digest" << "dirty" << "duplicate" << "elapsed" << "enable" << "encoding" << "error" << "event" << "expiration" << "expired" << "fail" << "failure" << "false" << "favorites" << "feature" << "features" << "feature-not-implemented" << "field" << "first" << "free" << "from" << "g.us" << "get" << "google" << "group" << "groups" << "http://etherx.jabber.org/streams" << "http://jabber.org/protocol/chatstates" << "ib" << "id" << "image" << "img" << "index" << "internal-server-error" << "ip" << "iq" << "item-not-found" << "item" << "jabber:iq:last" << "jabber:iq:privacy" << "jabber:x:event" << "jid" << "kind" << "last" << "leave" << "list" << "max" << "mechanism" << "media" << "message_acks" << "message" << "method" << "microsoft" << "missing" << "modify" << "mute" << "name" << "nokia" << "none" << "not-acceptable" << "not-allowed" << "not-authorized" << "notification" << "notify" << "off" << "offline" << "order" << "owner" << "owning" << "p_o" << "p_t" << "paid" << "participant" << "participants" << "participating" << "paused" << "picture" << "pin" << "ping" << "platform" << "port" << "presence" << "preview" << "probe" << "prop" << "props" << "query" << "raw" << "read" << "reason" << "receipt" << "received" << "relay" << "remote-server-timeout" << "remove" << "request" << "required" << "resource-constraint" << "resource" << "response" << "result" << "retry" << "rim" << "s_o" << "s_t" << "s.us" << "s.whatsapp.net" << "seconds" << "server-error" << "server" << "service-unavailable" << "set" << "show" << "silent" << "stat" << "status" << "stream:error" << "stream:features" << "subject" << "subscribe" << "success" << "sync" << "t" << "text" << "timeout" << "timestamp" << "to" << "true" << "type" << "unavailable" << "unsubscribe" << "uri" << "url" << "urn:ietf:params:xml:ns:xmpp-sasl" << "urn:ietf:params:xml:ns:xmpp-stanzas" << "urn:ietf:params:xml:ns:xmpp-streams" << "urn:xmpp:ping" << "urn:xmpp:receipts" << "urn:xmpp:whatsapp:account" << "urn:xmpp:whatsapp:dirty" << "urn:xmpp:whatsapp:mms" << "urn:xmpp:whatsapp:push" << "urn:xmpp:whatsapp" << "user" << "user-not-found" << "value" << "version" << "w:g" << "w:p:r" << "w:p" << "w:profile:picture" << "w" << "wait" << "WAUTH-2" << "x" << "xmlns:stream" << "xmlns" << "1" << "chatstate" << "crypto" << "enc" << "class" << "off_cnt" << "w:g2" << "promote" << "demote" << "creator"
        << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL << NULL
        << "Bell.caf" << "Boing.caf" << "Glass.caf" << "Harp.caf" << "TimePassing.caf" << "Tri-tone.caf" << "Xylophone.caf" << "background" << "backoff" << "chunked" << "context" << "full" << "in" << "interactive" << "out" << "registration" <<  "sid" << "urn:xmpp:whatsapp:sync" << "flt" << "s16" << "u8" << "adpcm" << "amrnb" << "amrwb" << "mp3" << "pcm" << "qcelp" << "wma" << "h263" << "h264" << "jpeg" << "mpeg4" <<  "wmv" << "audio/3gpp" << "audio/aac" << "audio/amr" << "audio/mp4" << "audio/mpeg" << "audio/ogg" << "audio/qcelp" << "audio/wav" << "audio/webm" << "audio/x-caf" << "audio/x-ms-wma" << "image/gif" << "image/jpeg" << "image/png" << "video/3gpp" <<  "video/avi" << "video/mp4" << "video/mpeg" << "video/quicktime" << "video/x-flv" << "video/x-ms-asf" << "302" << "400" << "401" << "402" << "403" << "404" << "405" << "406" << "407" << "409" <<  "500" << "501" << "503" << "504" << "abitrate" << "acodec" << "app_uptime" << "asampfmt" << "asampfreq" << "audio" << "bb_db" << "clear" << "conflict" << "conn_no_nna" << "cost" << "currency" <<  "duration" << "extend" << "file" << "fps" << "g_notify" << "g_sound" << "gcm" << "google_play" << "hash" << "height" << "invalid" << "jid-malformed" << "latitude" << "lc" << "lg" << "live" <<  "location" << "log" << "longitude" << "max_groups" << "max_participants" << "max_subject" << "mimetype" << "mode" << "napi_version" << "normalize" << "orighash" << "origin" << "passive" << "password" << "played" << "policy-violation" <<  "pop_mean_time" << "pop_plus_minus" << "price" << "pricing" << "redeem" << "Replaced by new connection" << "resume" << "signature" << "size" << "sound" << "source" << "system-shutdown" << "username" << "vbitrate" << "vcard" << "vcodec" <<  "video" << "width" << "xml-not-well-formed" << "checkmarks" << "image_max_edge" << "image_max_kbytes" << "image_quality" << "ka" << "ka_grow" << "ka_shrink" << "newmedia" << "library" << "caption" << "forward" << "c0" << "c1" <<  "c2" << "c3" << "clock_skew" << "cts" << "k0" << "k1" << "login_rtt" << "m_id" << "nna_msg_rtt" << "nna_no_off_count" << "nna_offline_ratio" << "nna_push_rtt" << "no_nna_con_count" << "off_msg_rtt" << "on_msg_rtt" << "stat_name" <<  "sts" << "suspect_conn" << "lists" << "self" << "qr" << "web" << "w:b" << "recipient" << "w:stats" << "forbidden" << "aurora.m4r" << "bamboo.m4r" << "chord.m4r" << "circles.m4r" << "complete.m4r" << "hello.m4r" <<  "input.m4r" << "keys.m4r" << "note.m4r" << "popcorn.m4r" << "pulse.m4r" << "synth.m4r" << "filehash";

    this->user = user;
    this->domain = domain;
    this->server = server;
    this->port = port;
    this->resource = resource;
    this->push_name = push_name;
    this->password = password;
    this->challenge = challenge;
    this->language = language;
    this->country = country;
    this->version = version;
    this->mcc = mcc;
    while (this->mcc.length() < 3)
        this->mcc.prepend("0");
    this->mnc = mnc;
    while (this->mnc.length() < 3)
        this->mnc.prepend("0");
    this->iqid = 0;
    this->counters = counters;
    this->myJid = user + "@" + JID_DOMAIN;
}

void Connection::init()
{
    this->socket = new QTcpSocket(this);
    this->out = new BinTreeNodeWriter(socket, dictionary, this);
    this->in = new BinTreeNodeReader(socket, dictionary, this);

    QObject::connect(this->out, SIGNAL(socketBroken()), this, SLOT(finalCleanup()));
    QObject::connect(this->in, SIGNAL(socketBroken()), this, SLOT(finalCleanup()));

    qDebug() << "Connecting to" << server;

    connect(socket,SIGNAL(connected()),this,SLOT(connectedToServer()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(disconnected()),this,SLOT(finalCleanup()));

    socket->connectToHost(server, port);
}

void Connection::disconnectAndDelete()
{
    disconnect(socket,0,0,0);
    socket->disconnectFromHost();
    finalCleanup();
}

void Connection::finalCleanup()
{
    Q_EMIT disconnected();
    disconnect(this,0,0,0);
    this->deleteLater();
}

/**
    Destroys a Connection object
*/
Connection::~Connection()
{
    /**
        @todo Clean disconnect to the WhatsApp servers
     */
    qDebug() << "Connection destructor";
}

/**
    Login to the WhatsApp service.

    @param nextChallenge    Next authentication challente to use.
*/
void Connection::login(const QByteArray &nextChallenge)
{
    // Update the challenge
    this->nextChallenge = nextChallenge;

    int outBytes, inBytes;
    outBytes = inBytes = 0;
    outBytes = out->streamStart(domain,resource);
    outBytes += sendFeatures();
    outBytes += sendAuth();
    inBytes += in->readStreamStart();
    QByteArray challengeData = readFeaturesUntilChallengeOrSuccess(&inBytes);
    if (challengeData.size() > 0)
    {
        outBytes += sendResponse(challengeData);
        inBytes += readSuccess();
    }

    counters->increaseCounter(DataCounters::ProtocolBytes, inBytes, outBytes);
}

/**
    Reads the next node.

    @return     true if a new node was read successfully.
*/
bool Connection::read()
{
    lastActivity = QDateTime::currentDateTime().toTime_t();

    ProtocolTreeNode node;
    bool pictureReceived = false;

    bool haveTree = false;

    try {
        haveTree = in->nextTree(node);
    }
    catch (IOException &e)
    {
        qDebug() << "read(): There was an IO Exception: " << e.toString();
        connectionClosed();
    }
    catch (ProtocolException &e)
    {
        qDebug() << "read(): There was a Protocol Exception: " << e.toString();
        connectionClosed();
    }

    if (haveTree)
    {
        lastTreeRead = QDateTime::currentMSecsSinceEpoch();
        QString tag = node.getTag();

        if (tag == "stream:error") {
            ProtocolTreeNodeListIterator i(node.getChildren());
            while (i.hasNext())
            {
                ProtocolTreeNode child = i.next().value();
                qDebug() << child.getTag();
                if (child.getTag() == "text") {
                    qDebug() << child.getDataString();
                }
            }
            Q_EMIT streamError();
        }
        if (tag == "iq")
        {
            QString type = node.getAttributeValue("type");
            QString id = node.getAttributeValue("id");
            QString from = node.getAttributeValue("from");
            QString xmlns = node.getAttributeValue("xmlns");

            if (xmlns == "urn:xmpp:ping")
            {
                sendPong(id);
            }
            else if (type == "result")
            {
                QStringList groupParticipants;

                ProtocolTreeNodeListIterator i(node.getChildren());
                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();
                    if (child.getTag() == "group")
                    {
                        QString childId = child.getAttributeValue("id");
                        if (id.startsWith("create_group_")) {
                            QString jid = childId + "@g.us";
                            Q_EMIT groupCreated(jid);
                            sendGetGroupInfo(jid);
                        }
                        else /*if (id.startsWith("get_groups_"))*/ {
                            QString subject = child.getAttributeValue("subject");
                            QString author = child.getAttributeValue("owner");
                            QString creation = child.getAttributeValue("creation");
                            QString subject_o = child.getAttributeValue("s_o");
                            QString subject_t = child.getAttributeValue("s_t");
                            emit groupInfoFromList(id, childId + "@g.us", author,
                                                   subject, creation,
                                                   subject_o, subject_t);
                        }
                    }

                    else if (child.getTag() == "leave")
                    {
                        ProtocolTreeNodeListIterator j(child.getChildren());
                        while (j.hasNext())
                        {
                            ProtocolTreeNode group = j.next().value();
                            if (group.getTag() == "group")
                            {
                                QString groupId = group.getAttributeValue("id");
                                emit groupLeft(groupId);
                                qDebug() << "Leaving group:" << groupId;
                            }
                        }
                    }

                    else if (child.getTag() == "query")
                    {
                        if (id.startsWith("last_"))
                        {
                            qint64 timestamp = QDateTime::currentDateTime().toTime_t() -
                                    child.getAttributeValue("seconds").toLongLong();

                            emit lastOnline(from, timestamp);
                        }
                        else if (id.startsWith("privacylist_")) {
                            QStringList privacyList;
                            ProtocolTreeNodeListIterator j(child.getChildren());
                            while (j.hasNext())
                            {
                                ProtocolTreeNode group = j.next().value();
                                if (group.getTag() == "list") {
                                    ProtocolTreeNodeListIterator k(group.getChildren());
                                    while (k.hasNext())
                                    {
                                        ProtocolTreeNode list = k.next().value();
                                        if (list.getTag() == "item")
                                        {
                                            QString jid = list.getAttributeValue("value");
                                            if (!jid.isEmpty()) {
                                                privacyList.append(jid);
                                            }
                                        }
                                    }
                                }
                            }
                            if (!privacyList.isEmpty()) {
                                emit privacyListReceived(privacyList);
                            }
                        }
                        else {
                            qDebug() << "xmlns type:" << xmlns;
                        }
                    }

                    else if (child.getTag() == "privacy" && id.startsWith("privacysettings_")) {
                        QVariantMap values;
                        ProtocolTreeNodeListIterator j(child.getChildren());
                        while (j.hasNext())
                        {
                            ProtocolTreeNode group = j.next().value();
                            if (group.getTag() == "category") {
                                values[group.getAttributeValue("name")] = group.getAttributeValue("value");
                            }
                        }
                        Q_EMIT privacySettingsReceived(values);
                    }

                    else if (child.getTag() == "media" || child.getTag() == "duplicate")
                    {
                        Key k(JID_DOMAIN,true,id);
                        FMessage message = store.value(k);

                        if (message.key.id == id)
                        {
                            message.status = (child.getTag() == "media")
                                        ? FMessage::Uploading
                                        : FMessage::Uploaded;
                            message.media_url = child.getAttributeValue("url");
                            if (child.getTag() == "duplicate") {
                                message.media_mime_type = child.getAttributeValue("mimetype");
                                if (message.media_wa_type == FMessage::Video ||
                                    message.media_wa_type == FMessage::Audio)
                                {
                                    QString duration = child.getAttributeValue("duration");
                                    message.media_duration_seconds =
                                            (duration.isEmpty()) ? 0 : duration.toInt();
                                }
                                if (message.media_wa_type == FMessage::Image ||
                                    message.media_wa_type == FMessage::Audio)
                                {
                                    QString width = child.getAttributeValue("width");
                                    QString height = child.getAttributeValue("height");
                                    if (!width.isEmpty() && !height.isEmpty()) {
                                        message.media_width = width.toInt();
                                        message.media_height = height.toInt();
                                    }
                                }
                            }

                            store.remove(k);

                            emit mediaUploadAccepted(message);

                        }
                    }

                    // This is the result of the sendGetPhotoIds()
                    // That method is not used anymore

                    else if (child.getTag() == "picture")
                    {
                        QString imageType = child.getAttributeValue("type");
                        QString photoId = child.getAttributeValue("id");
                        QByteArray bytes = child.getData();

                        if (bytes.size() > 0)
                            emit photoReceived(from, bytes, photoId, (imageType == "image"));
                        else
                            sendGetPhoto(from, QString(), true);


                        pictureReceived = true;
                        counters->increaseCounter(DataCounters::ProfileBytes, node.getSize(), 0);
                    }

                    else if (child.getTag() == "sync")
                    {
                        qDebug() << "sync response";
                        ProtocolTreeNodeListIterator j(child.getChildren());
                        while (j.hasNext())
                        {
                            ProtocolTreeNode group = j.next().value();
                            if (group.getTag() == "full" || group.getTag() == "in") {
                                QStringList jids;
                                QVariantList contacts;
                                ProtocolTreeNodeListIterator k(group.getChildren());
                                while (k.hasNext())
                                {
                                    ProtocolTreeNode list = k.next().value();
                                    if (list.getTag() == "user")
                                    {
                                        QString jid = list.getAttributeValue("jid");
                                        jids.append(jid);
                                        QVariantMap contact;
                                        contact["jid"] = jid;
                                        contact["phone"] = list.getDataString();
                                        contacts.append(contact);
                                    }
                                }
                                sendGetStatus(jids);
                                Q_EMIT contactsSynced(contacts);
                            }
                        }
                        Q_EMIT syncFinished();
                    }

                    else if (child.getTag() == "status")
                    {
                        qDebug() << "status response";
                        QVariantList contacts;
                        ProtocolTreeNodeListIterator j(child.getChildren());
                        while (j.hasNext())
                        {
                            ProtocolTreeNode list = j.next().value();
                            if (list.getTag() == "user")
                            {
                                QString jid = list.getAttributeValue("jid");
                                QString t = list.getAttributeValue("t");
                                QVariantMap contact;
                                contact["jid"] = jid;
                                contact["timestamp"] = t;
                                QString message = list.getDataString();
                                if (message.isEmpty()) {
                                    QString code = list.getAttributeValue("code");
                                    if (code == "401") {
                                        contact["hidden"] = true;
                                    }
                                }
                                contact["message"] = message;
                                contacts.append(contact);
                            }
                        }
                        Q_EMIT contactsStatus(contacts);
                    }

                    else if (child.getTag() == "participant" && id.startsWith("get_participants_")) {
                        QString jid = child.getAttributeValue("jid");
                        groupParticipants.append(jid);
                        //Q_EMIT groupUser(from, jid);
                    }
                }

                if (!groupParticipants.isEmpty()) {
                    Q_EMIT groupUsers(from, groupParticipants);
                }

                if (id.startsWith("privacy_")) {
                    sendGetPrivacyList();
                }
            }
            else if (type == "error")
            {
                QString id = node.getAttributeValue("id");
                if (id.startsWith("privacylist"))
                   emit privacyListReceived(QStringList());
                else if (id.startsWith("get_picture_")) {
                   ProtocolTreeNodeListIterator i(node.getChildren());
                   while (i.hasNext())
                   {
                       ProtocolTreeNode child = i.next().value();
                       if (child.getTag() == "error")
                       {
                           QString code = child.getAttributeValue("code");
                           if (code == "401") {
                               Q_EMIT photoReceived(from, QByteArray(), "hidden", true);
                           }
                           else if (code == "404") {
                               Q_EMIT photoReceived(from, QByteArray(), "empty", true);
                           }
                       }
                   }
                }
                else if (id.startsWith("last_")) {
                    ProtocolTreeNodeListIterator i(node.getChildren());
                    while (i.hasNext())
                    {
                        ProtocolTreeNode child = i.next().value();
                        if (child.getTag() == "error")
                        {
                            QString code = child.getAttributeValue("code");
                            if (code == "405") { //privacy
                                Q_EMIT lastOnline(from, -1);
                            }
                            if (code == "401") { //blocked
                                Q_EMIT lastOnline(from, -2);
                            }
                        }
                    }
                }
            }
        }

        else if (tag == "ib")
        {
            ProtocolTreeNodeListIterator i(node.getChildren());
            while (i.hasNext()) {
                ProtocolTreeNode child = i.next().value();
                if (child.getTag() == "dirty") {
                    sendCleanDirty(QStringList() << child.getAttributeValue("type"));
                }
                else if (child.getTag() == "offline") {
                    Q_EMIT notifyOfflineMessages(child.getAttributeValue("count").toInt());
                }
            }
        }

        else if (tag == "presence")
        {
            QString from = node.getAttributeValue("from");
            if (!from.isEmpty() && !from.contains("-"))
            {
                QString type = node.getAttributeValue("type");
                if (type.isEmpty() || type == "available")
                    emit available(from, true);
                else if (type == "unavailable")
                    emit available(from, false);
            }
        }

        else if (tag == "chatstate") {
            QString from = node.getAttributeValue("from");
            ProtocolTreeNodeListIterator i(node.getChildren());
            while (i.hasNext()) {
                ProtocolTreeNode child = i.next().value();
                if (child.getTag() == "composing") {
                    emit composing(from, "");
                }
                else if (child.getTag() == "paused") {
                    emit paused(from);
                }
            }
        }

        else if (tag == "ack") {
            QString aclass = node.getAttributeValue("class");
            if (aclass == "message") {
                QString from = node.getAttributeValue("from");
                QString id = node.getAttributeValue("id");
                emit messageStatusUpdate(from, id, FMessage::ReceivedByServer);
            }
            else if (aclass == "receipt") {
                qDebug() << "TODO:" << "ack message receipt class";
            }
        }

        else if (tag == "receipt") {
            QString from = node.getAttributeValue("from");
            QString id = node.getAttributeValue("id");
            QString type = node.getAttributeValue("type");
            QString participant = node.getAttributeValue("participant");
            if (from.contains("broadcast")) {
                emit messageStatusUpdate(participant, id, (type == "played")
                                                     ? FMessage::Played
                                                     : FMessage::ReceivedByTarget);
            }
            else if (!from.contains("s.us")) {
                emit messageStatusUpdate(from, id, (type == "played")
                                                     ? FMessage::Played
                                                     : FMessage::ReceivedByTarget);
            }
            if (type == "delivered" || type == "played" || type.isEmpty())
            {
                // Delivery Receipt received
                sendReceiptAck(id, type);
            }
        }
        else if (tag == "notification")
        {
            QString notificationType = node.getAttributeValue("type");
            QString from = node.getAttributeValue("from");
            QString to = node.getAttributeValue("to");
            QString participant = node.getAttributeValue("participant");
            QString id = node.getAttributeValue("id");
            QString notify = node.getAttributeValue("notify");
            bool offline = !node.getAttributeValue("offline").isEmpty();
            if (!notify.isEmpty()) {
                if (from.contains("-")) {
                    if (!participant.isEmpty())
                        Q_EMIT updatePushname(participant, notify);
                }
                else {
                    Q_EMIT updatePushname(from, notify);
                }
            }

            if (notificationType == "picture")
            {
                QString timestamp = node.getAttributeValue("t");

                ProtocolTreeNodeListIterator i(node.getChildren());

                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();

                    if (child.getTag() == "set")
                    {
                        QString photoId = child.getAttributeValue("id");
                        if (!photoId.isEmpty()) {
                            QString author = child.getAttributeValue("author");
                            emit photoIdReceived(from, notify, author, timestamp, photoId, id, offline);
                        }
                    }
                    else if (child.getTag() == "delete") {
                        QString author = child.getAttributeValue("author");
                        emit photoDeleted(from, notify, author, timestamp, id, offline);
                    }
                }

                sendNotificationReceived(from, id, to, participant, notificationType, ProtocolTreeNode());
            }

            else if (notificationType == "contacts") {
                ProtocolTreeNodeListIterator i(node.getChildren());
                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();

                    if (child.getTag() == "add")
                    {
                        QString jid = child.getAttributeValue("jid");
                        if (!jid.isEmpty())
                            Q_EMIT contactAdded(jid);
                    }
                }

                ProtocolTreeNode sync("sync");
                AttributeList syncattrs;
                syncattrs.insert("contacts", "out");
                sync.setAttributes(syncattrs);
                sendNotificationReceived(from, id, to, participant, notificationType, sync);
            }

            else if (notificationType == "subject") {
                sendNotificationReceived(from, id, to, participant, notificationType, ProtocolTreeNode());
                QString timestamp = node.getAttributeValue("t");
                ProtocolTreeNodeListIterator i(node.getChildren());
                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();
                    if (child.getTag() == "body")
                    {
                        //QString event = child.getAttributeValue("event");
                        //if (event == "add") {
                            QString subject = child.getDataString();
                            Q_EMIT groupNewSubject(from, participant, notify, subject, timestamp, id, offline);
                        //}
                    }
                }
            }

            else if (notificationType == "status") {
                sendNotificationReceived(from, id, to, participant, notificationType, ProtocolTreeNode());
                QString timestamp = node.getAttributeValue("t");
                ProtocolTreeNodeListIterator i(node.getChildren());
                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();
                    if (child.getTag() == "set")
                    {
                        QString message = child.getDataString();
                        Q_EMIT userStatusUpdated(from, message, timestamp.toInt());
                    }
                }
            }

            else if (notificationType == "web") {
                sendNotificationReceived(from, id, to, participant, notificationType, ProtocolTreeNode());
            }

            else if (notificationType == "participant") {
                sendNotificationReceived(from, id, to, participant, notificationType, ProtocolTreeNode());
                QString timestamp = node.getAttributeValue("t");
                ProtocolTreeNodeListIterator i(node.getChildren());
                while (i.hasNext())
                {
                    ProtocolTreeNode child = i.next().value();
                    if (child.getTag() == "add")
                    {
                        QString jid = child.getAttributeValue("jid");
                        if (jid == myJid) {
                            sendGetGroupInfo(from);
                        }
                        else if (!jid.isEmpty()) {
                            Q_EMIT groupAddUser(from, jid, timestamp, id, offline);
                        }
                    }
                    else if (child.getTag() == "remove")
                    {
                        QString jid = child.getAttributeValue("jid");
                        if (!jid.isEmpty())
                            Q_EMIT groupRemoveUser(from, jid, timestamp, id, offline);
                    }
                }
            }
        }

        else if (tag == "message")
            parseMessageInitialTagAlreadyChecked(node);

        // Update counters
        if (tag != "message" && !pictureReceived)
            counters->increaseCounter(DataCounters::ProtocolBytes, node.getSize(), 0);

        return true;
    }

    return false;
}

/**
    Parses a node where the initial tag was already checked to be <message>.

    @param messageNode      ProtocolTreeNode object where its main tag is <message>.
*/
void Connection::parseMessageInitialTagAlreadyChecked(ProtocolTreeNode &messageNode)
{
    ChatMessageType msgType = Unknown;

    QString id = messageNode.getAttributeValue("id");
    QString attribute_t = messageNode.getAttributeValue("t");
    QString from = messageNode.getAttributeValue("from");
    QString author = messageNode.getAttributeValue("participant");
    bool broadcast = false;
    if (from.contains("@broadcast")) {
        from = author;
        broadcast = true;
    }
    bool offline = !messageNode.getAttributeValue("offline").isEmpty();
    QString retry = messageNode.getAttributeValue("retry");
    QString typeAttribute = messageNode.getAttributeValue("type");

    if (typeAttribute == "text" || typeAttribute == "media")
    {
        ProtocolTreeNodeListIterator i(messageNode.getChildren());

        FMessage message;
        if (!offline)
            message.timestamp = QDateTime::currentDateTime().toTime_t();
        else
            message.timestamp = attribute_t.toLongLong();
        while (i.hasNext())
        {
            ProtocolTreeNode child = i.next().value();

            if (child.getTag() == "body")
            {
                // New message received

                Key k(from, false, id);
                message.setKey(k);
                message.setData(child.getData());
                message.remote_resource = author;
                message.setThumbImage("");
                message.type = FMessage::BodyMessage;
                message.notify_name = messageNode.getAttributeValue("notify");

                msgType = MessageReceived;
                sendMessageReceived(message);

            }
            else if (child.getTag() == "media")
            {
                // New mms received

                Key k(from, false, id);
                message.setKey(k);
                message.remote_resource = author;
                message.type = FMessage::MediaMessage;

                message.setMediaWAType(child.getAttributeValue("type"));

                if (message.media_wa_type == FMessage::Contact) {
                    ProtocolTreeNodeListIterator ci(child.getChildren());

                    while (ci.hasNext())
                    {
                        ProtocolTreeNode cc = ci.next().value();

                        if (cc.getTag() == "vcard")
                        {
                            message.media_name = cc.getAttributeValue("name");
                            message.setData(QString::fromUtf8(cc.getData().data()));
                        }
                    }
                }
                else {
                    message.media_url = child.getAttributeValue("url");

                    if (message.media_wa_type == FMessage::Location)
                    {
                        message.media_name = child.getAttributeValue("name");
                        message.latitude = child.getAttributeValue("latitude").toDouble();
                        message.longitude = child.getAttributeValue("longitude").toDouble();
                    }
                    else
                        message.media_name = child.getAttributeValue("file");

                    message.media_size = child.getAttributeValue("size").toLongLong();
                    message.media_mime_type = child.getAttributeValue("mimetype");

                    if (message.media_wa_type == FMessage::Video ||
                        message.media_wa_type == FMessage::Audio) {
                        message.media_duration_seconds = child.getAttributeValue("duration").toInt();
                    }
                    if (message.media_wa_type == FMessage::Image ||
                        message.media_wa_type == FMessage::Video) {
                        message.media_width = child.getAttributeValue("width").toInt();
                        message.media_height = child.getAttributeValue("height").toInt();
                    }

                    message.live = (child.getAttributeValue("origin") == "live");

                    QString encoding = child.getAttributeValue("encoding");
                    if (encoding == "raw") {
                        message.setData(QString::fromUtf8(child.getData().toBase64().constData()));
                    }
                    else {
                        message.setData(QString::fromUtf8(child.getData().data()));
                    }
                }

                msgType = MessageReceived;
                sendMessageReceived(message);
            }
            else if (child.getTag() == "received")
            {
                QString receipt_type = child.getAttributeValue("type");
                Key k(from,true,id);
                message = store.value(k);
                if (message.key.id == id)
                {
                    message.status = (receipt_type == "played")
                            ? FMessage::Played
                            : FMessage::ReceivedByTarget;
                    msgType = (from == "s.us") ? Unknown : MessageStatusUpdate;

                    // Remove it from the store if it's not a voice message
                    // Or if it's a voice message already played
                    if ((message.live && receipt_type == "played") || !message.live)
                        store.remove(k);
                }
                if (receipt_type == "delivered" || receipt_type == "played" ||
                    receipt_type.isEmpty())
                {
                    // Delivery Receipt received
                    sendDeliveredReceiptAck(from,id,
                                            (receipt_type.isEmpty()
                                             ? "delivered"
                                             : receipt_type));
                }
            }
        }
        message.broadcast = broadcast;
        message.offline = offline;

        switch (msgType)
        {
            case MessageReceived:
                emit messageReceived(message);
                break;

            case MessageStatusUpdate:
                emit messageStatusUpdate(message.key.remote_jid, message.key.id, message.status);
                break;

            default:
                break;
        }

        // Increase data counters
        if (msgType == MessageReceived)
        {
            counters->increaseCounter(DataCounters::Messages, 1, 0);
            counters->increaseCounter(DataCounters::MessageBytes, messageNode.getSize(), 0);
        }
    }
    else if (typeAttribute == "error")
    {
        if (from.right(5) == "@g.us")
            emit groupError(from);
    }

    counters->increaseCounter(DataCounters::ProtocolBytes, messageNode.getSize(), 0);

}

/**
    Get the unixtime of the last node successfully read.

    @return     unixtime of the last node successfully read.
*/
qint64 Connection::getLastTreeReadTimestamp()
{
    return lastTreeRead;
}

void Connection::connectedToServer()
{
    qDebug() << "Connected to" << server;

    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    //socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    /*int fd = socket->socketDescriptor();
    int optval = 30;
    socklen_t optlen = sizeof(optval);
    setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);*/

    Q_EMIT connected();

    login(challenge);
}

void Connection::connectionClosed()
{
    qDebug() << "Connection closed";
    disconnectAndDelete();
}

void Connection::checkActivity()
{
    uint now = QDateTime::currentDateTime().toTime_t();
    qDebug() << "wakeup check activity. now:" << now << "last:" << lastActivity;
    if ((now - lastActivity) > 905) {
        qDebug() << "should reconnect";
        disconnectAndDelete();
    }
}

void Connection::socketError(QAbstractSocket::SocketError error)
{
    IOException e(error);
    qDebug() << "There was an IO error:" << e.toString();
    finalCleanup();
}

void Connection::readNode()
{
    while (socket->bytesAvailable() > 0) {
        if (!read())
            qDebug() << "Error reading tree";
    }
}

/** ***********************************************************************
 ** Authentication methods
 **/

/**
    Sends the features supported by this client to the WhatsApp servers.

    So far this method sends the support of profile pictures.

    @return     number of bytes written to the socket.
*/
int Connection::sendFeatures()
{
    /*ProtocolTreeNode child("receipt_acks");

    AttributeList attrs;

    ProtocolTreeNode child2("w:profile:picture");
    attrs.insert("type","all");
    child2.setAttributes(attrs);

    attrs.clear();

    ProtocolTreeNode child3("status");*/

    ProtocolTreeNode node("stream:features");
    //node.addChild(child);
    //node.addChild(child2);
    //node.addChild(child3);

    int bytes = out->write(node,false);
    return bytes;
}

/**
    Sends the authentication request.

    This method implements the WAUTH-2 WhatsApp protocol of authentication.

    @return     number of bytes written to the socket.
*/
int Connection::sendAuth()
{
    QByteArray data;
    if (nextChallenge.size() > 0)
        data = getAuthBlob(nextChallenge);

    AttributeList attrs;

    //attrs.insert("xmlns", "urn:ietf:params:xml:ns:xmpp-sasl");
    attrs.insert("passive", "false");
    attrs.insert("mechanism", "WAUTH-2");
    attrs.insert("user", user);

    ProtocolTreeNode node("auth", data);
    node.setAttributes(attrs);
    int bytes = out->write(node, false);
    if (nextChallenge.size() > 0)
        out->setCrypto(true);

    return bytes;
}

/**
    Constructs the authentication data to be sent.

    The authentication data is crypted with RC4 using the user password and the
    challenge data as salt.

    @param nonce    Challenge data.
    @return         Authentication blob encrypted.
*/
QByteArray Connection::getAuthBlob(QByteArray &nonce)
{
    QList<QByteArray> keys = KeyStream::keyFromPasswordAndNonce(password,nonce);
    inputKey = new KeyStream(keys.at(2), keys.at(3), this);
    outputKey = new KeyStream(keys.at(0), keys.at(1), this);

    in->setInputKey(inputKey);
    out->setOutputKey(outputKey);

    QByteArray list(4, (char)0);

    list.append(user.toUtf8());
    list.append(nonce);

    qint64 totalSeconds = QDateTime::currentMSecsSinceEpoch() / 1000;
    list.append(QString::number(totalSeconds).toUtf8());
    list.append(QString("WhatsApp/%1 Android/4.2.1 Device/GalaxyS3").arg(version));
    if (!mcc.isEmpty() && !mnc.isEmpty()) {
        list.append(QString(" MccMnc/%2%3").arg(mcc).arg(mnc));
    }

    outputKey->encodeMessage(list, 0, 4, list.length()-4);

    return list;
}

/**
    Reads the features from the node and the challenge data.

    @param bytes    Pointer to the number of bytes read to be updated.
    @return         New challenge data.
*/
QByteArray Connection::readFeaturesUntilChallengeOrSuccess(int *bytes)
{
    ProtocolTreeNode node;

    QByteArray data;
    bool moreNodes;

    while ((moreNodes = in->nextTree(node)))
    {
        *bytes += node.getSize();

        if (node.getTag() == "stream:features")
        {
        }

        if (node.getTag() == "challenge")
        {
            data = node.getData();
            qDebug() << QString("Challenge: (%1) %2").arg(QString::number(data.length())).arg(QString::fromLatin1(data.toHex()));

            return data;
        }

        if (node.getTag() == "success")
        {
            parseSuccessNode(node);
            return data;
        }
    }

    return data;
}

/**
    Sends authentication response.

    @param challengeData    Authentication challenge data (nonce/salt).
    @return                 number of bytes written to the socket.
*/
int Connection::sendResponse(QByteArray &challengeData)
{
    QByteArray authBlob = getAuthBlob(challengeData);

    AttributeList attrs;

    attrs.insert("xmlns","urn:ietf:params:xml:ns:xmpp-sasl");
    ProtocolTreeNode node("response", authBlob);
    node.setAttributes(attrs);
    int bytes = out->write(node, false);
    out->setCrypto(true);

    return bytes;
}

/**
    Read the authentication success node and parse it.

    @return     number of bytes read from the socket (node size).
*/
int Connection::readSuccess()
{
    ProtocolTreeNode node;

    in->nextTree(node);
    parseSuccessNode(node);

    return node.getSize();
}

/**
    Parses the authentication success node.

    This method retrieves all the account information.  It also updates the
    next challenge data.

    @param node     ProtocolTreeNode object with the authentication success node.
*/
void Connection::parseSuccessNode(const ProtocolTreeNode &node)
{
    if (node.getTag() == "success") {
        // This has to be converted to a date object
        accountstatus = node.getAttributeValue("status");
        expiration = node.getAttributeValue("expiration");
        creation = node.getAttributeValue("creation");
        kind = node.getAttributeValue("kind");

        if (accountstatus == "expired") {
            QVariantMap reason;
            reason["reason"] = QString("Login");
            Q_EMIT accountExpired(reason);
            disconnectAndDelete();
            return;
        }

        nextChallenge = node.getData();

        connect(socket,SIGNAL(readyRead()),this,SLOT(readNode()));

        //sendClientConfig("android");
        sendClientConfig("none");

        Q_EMIT authSuccess(creation, expiration, kind, accountstatus, nextChallenge);

        lastActivity = QDateTime::currentDateTime().toTime_t();
    }
    else {
        Q_EMIT authFailed();

        connectionClosed();
    }
}

void Connection::sendCleanDirty(const QStringList &categories)
{
    QString id = makeId("clean_dirty_");
    ProtocolTreeNode iqNode("iq");
    AttributeList attrs;
    attrs.insert("id", id);
    attrs.insert("type", "set");
    attrs.insert("to", domain);
    attrs.insert("xmlns", "urn:xmpp:whatsapp:dirty");
    iqNode.setAttributes(attrs);
    foreach (const QString &category, categories) {
        ProtocolTreeNode catNode("clean");
        attrs.clear();
        attrs.insert("type", category);
        catNode.setAttributes(attrs);
        iqNode.addChild(catNode);
    }

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendGetDirty()
{
    QString id = makeId("get_dirty_");
    ProtocolTreeNode statusNode("status");
    AttributeList attrs;
    attrs.insert("xmlns", "urn:xmpp:whatsapp:dirty");
    statusNode.setAttributes(attrs);
    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "get");
    attrs.insert("to", domain);
    //attrs.insert("xmlns", "urn:xmpp:whatsapp:dirty");
    iqNode.setAttributes(attrs);
    iqNode.addChild(statusNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

QString Connection::getMediaWAType(int type)
{
    switch(type)
    {
        case FMessage::Image:
            return "image";

        case FMessage::System:
            return "system";

        case FMessage::Audio:
            return "audio";

        case FMessage::Video:
            return "video";

        case FMessage::Contact:
            return "vcard";

        case FMessage::Location:
            return "location";
    }

    return "undefined";
}

/** ***********************************************************************
 ** Message handling methods
 **/

/**
    Sends a message

    @param message      FMessage object containing the message.
*/
void Connection::sendMessage(const FMessage &message)
{
    switch (message.type)
    {
        case FMessage::BodyMessage:
            sendMessageWithBody(message);
            break;

        case FMessage::MediaMessage:
            sendMessageWithMedia(message);
            break;

        case FMessage::RequestMediaMessage:
            requestMessageWithMedia(message);

        default:
            break;
    }
}

void Connection::sendSyncContacts(const QStringList &numbers)
{
    qDebug() << "numbers:" << numbers;
    QString id = makeId("sync_");

    AttributeList attrs;

    ProtocolTreeNode syncNode("sync");
    attrs.clear();
    attrs.insert("context", "background");
    attrs.insert("index", "0");
    attrs.insert("mode", "delta");
    attrs.insert("last", "true");
    attrs.insert("sid", QString::number(QDateTime::currentDateTimeUtc().toTime_t()));
    syncNode.setAttributes(attrs);

    foreach (QString number, numbers) {
        if (number.contains("@")) {
            number = number.split("@").first();
            ProtocolTreeNode userNode("user");
            userNode.setData(QString("+%1").arg(number).toLatin1());
            syncNode.addChild(userNode);
        }
        ProtocolTreeNode userNode("user");
        userNode.setData(number.toLatin1());
        syncNode.addChild(userNode);
    }

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "get");
    attrs.insert("to", myJid);
    attrs.insert("xmlns", "urn:xmpp:whatsapp:sync");
    iqNode.setAttributes(attrs);
    iqNode.addChild(syncNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a text message.

    @param message      FMessage object containing the text message.
*/
void Connection::sendMessageWithBody(const FMessage &message)
{
    QString text = QString::fromUtf8(message.data.data());
    text = text.replace("<br />", "\n")
               .replace("&quot;","\"")
               .replace("&lt;", "<")
               .replace("&gt;", ">")
               .replace("&amp;", "&");

    // Add it to the store
    store.put(message);

    ProtocolTreeNode bodyNode("body", text.toUtf8());
    ProtocolTreeNode messageNode;

    messageNode = getMessageNode(message, bodyNode);

    int bytes = out->write(messageNode);
    counters->increaseCounter(DataCounters::Messages, 0, 1);
    counters->increaseCounter(DataCounters::MessageBytes, 0, bytes);
}

/**
    Sends a request to send a multimedia message.

    To send a multimedia message to another user an upload request has to be sent
    first.  This method sends such request.

    @param message      FMessage object containing the multimedia message.
*/
void Connection::requestMessageWithMedia(const FMessage &message)
{
    qDebug() << "key:" << message.key.remote_jid << ":" << message.key.id;

    // Add it to the store
    store.put(message);

    AttributeList attrs;

    ProtocolTreeNode mediaNode("media");
    attrs.insert("hash", message.data);
    attrs.insert("type", getMediaWAType(message.media_wa_type));
    attrs.insert("size", QString::number(message.media_size));
    if (message.live)
        attrs.insert("origin","live");

    mediaNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id", message.key.id);
    attrs.insert("type","set");
    attrs.insert("to",domain);
    attrs.insert("xmlns","w:m");
    iqNode.setAttributes(attrs);
    iqNode.addChild(mediaNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a multimedia message.

    @param message      FMessage object containing the multimedia message.
*/
void Connection::sendMessageWithMedia(const FMessage& message)
{
    qDebug() << "send media message";
    // Add it to the store
    store.put(message);

    // Global multimedia messages attributes
    AttributeList attrs;
    attrs.insert("type", getMediaWAType(message.media_wa_type));

    if (message.media_wa_type == FMessage::Contact && !message.media_name.isEmpty())
    {
        ProtocolTreeNode cardNode("vcard", message.data);
        AttributeList cardattrs;
        cardattrs.insert("name", message.media_name);
        cardNode.setAttributes(cardattrs);

        ProtocolTreeNode mediaNode("media");
        mediaNode.setAttributes(attrs);
        mediaNode.addChild(cardNode);

        ProtocolTreeNode messageNode = getMessageNode(message, mediaNode);

        int bytes = out->write(messageNode);
        counters->increaseCounter(DataCounters::Messages, 0, 1);
        counters->increaseCounter(DataCounters::MessageBytes, 0, bytes);
    }
    else
    if (message.media_wa_type == FMessage::Location && message.latitude != 0 && message.longitude != 0)
    {
        ProtocolTreeNode mediaNode("media", message.data);
        attrs.insert("latitude", QString::number(message.latitude));
        attrs.insert("longitude", QString::number(message.longitude));
        mediaNode.setAttributes(attrs);

        ProtocolTreeNode messageNode = getMessageNode(message, mediaNode);

        int bytes = out->write(messageNode);
        counters->increaseCounter(DataCounters::Messages, 0, 1);
        counters->increaseCounter(DataCounters::MessageBytes, 0, bytes);
    }
    else
    if (!message.media_name.isEmpty() && !message.media_url.isEmpty() &&
        message.media_size > 0)
    {
        attrs.insert("file", message.media_name);
        attrs.insert("size", QString::number(message.media_size));
        attrs.insert("url", message.media_url);
        if (message.live)
            attrs.insert("origin","live");

        if (message.media_wa_type == FMessage::Audio ||
            message.media_wa_type == FMessage::Video)
        {
            attrs.insert("duration", QString::number(message.media_duration_seconds));
            attrs.insert("seconds", QString::number(message.media_duration_seconds));
        }
        if (message.data.size() > 0)
            attrs.insert("encoding","raw");

        ProtocolTreeNode mediaNode("media", message.data);
        mediaNode.setAttributes(attrs);

        ProtocolTreeNode messageNode = getMessageNode(message, mediaNode);

        int bytes = out->write(messageNode);
        counters->increaseCounter(DataCounters::Messages, 0, 1);
        counters->increaseCounter(DataCounters::MessageBytes, 0, bytes);
    }
}

/**
    Constructs a message node.

    @param message      FMessage object containing the message.
    @param child        ProtocolTreeNode object with the message data
    @return             ProtocolTreeNode object containing the message node.
*/
ProtocolTreeNode Connection::getMessageNode(const FMessage &message, const ProtocolTreeNode &child)
{
    ProtocolTreeNode serverNode("server");

    ProtocolTreeNode xNode("x");
    AttributeList attrs;
    attrs.insert("xmlns","jabber:x:event");
    xNode.setAttributes(attrs);
    xNode.addChild(serverNode);

    attrs.clear();
    attrs.insert("id",message.key.id);
    attrs.insert("type",child.getTag() == "body" ? "text" : "media");
    attrs.insert("to",message.key.remote_jid);

    qDebug() << "Message ID" << message.key.id;

    ProtocolTreeNode messageNode("message");

    messageNode.setAttributes(attrs);
    if (message.key.remote_jid == "broadcast") {
        ProtocolTreeNode broadcast("broadcast");
        foreach (QString jid, message.broadcastJids) {
            ProtocolTreeNode broadcastChild("to");
            AttributeList to;
            to.insert("jid", jid);
            broadcastChild.setAttributes(to);
            broadcast.addChild(broadcastChild);
        }
        messageNode.addChild(broadcast);
    }
    messageNode.addChild(child);
    messageNode.addChild(xNode);

    return messageNode;
}

/**
    Send a message received acknowledgement.

    @param message      FMessage object containing the message to be acknowledged.
*/
void Connection::sendMessageReceived(const FMessage &message, const QString &type)
{
    AttributeList attrs;

    ProtocolTreeNode messageNode("receipt");
    attrs.clear();
    QString resource = message.broadcast ? message.remote_resource : message.key.remote_jid;
    attrs.insert("to",resource);
    attrs.insert("id",message.key.id);
    if (!type.isEmpty()) {
        attrs.insert("type", type);
    }

    messageNode.setAttributes(attrs);

    int bytes = out->write(messageNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Send a notification received acknowledgement.

    Notification nodes usually contain profile pictures change notifications.

    @param message      FMessage object containing the message to be acknowledged.
*/
void Connection::sendNotificationReceived(const QString &to, const QString &id, const QString &from, const QString &participant, const QString &type, const ProtocolTreeNode &childNode)
{
    AttributeList attrs;

    ProtocolTreeNode ackNode("ack");
    attrs.clear();
    attrs.insert("to",to);
    attrs.insert("class","notification");
    attrs.insert("id",id);
    attrs.insert("type",type);
    if (!participant.isEmpty()) {
        attrs.insert("participant", participant);
    }
    if (!from.isEmpty()) {
        attrs.insert("from", from);
    }
    ackNode.setAttributes(attrs);
    if (!childNode.getTag().isEmpty()) {
        ackNode.addChild(childNode);
    }

    int bytes = out->write(ackNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a receipt acknowledging a delivered message notification received.

    @param to           Destination jid.
    @param id           Id of the receipt received.
    @param type         Type of the ack (delivered, played)
*/
void Connection::sendDeliveredReceiptAck(const QString &to, const QString &id, const QString &type)
{
    ProtocolTreeNode ackNode("ack");
    AttributeList attrs;
    attrs.insert("xmlns","urn:xmpp:receipts");
    attrs.insert("type",type);
    ackNode.setAttributes(attrs);

    ProtocolTreeNode node("message");
    node.addChild(ackNode);
    attrs.clear();
    attrs.insert("to",to);
    attrs.insert("type","chat");
    attrs.insert("id",id);
    node.setAttributes(attrs);

    int bytes = out->write(node);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendReceiptAck(QString id, QString type)
{
    ProtocolTreeNode ackNode("ack");
    AttributeList attrs;
    attrs.insert("class","receipt");
    attrs.insert("type",type.isEmpty() ? "delivery" : type);
    attrs.insert("id",id);
    ackNode.setAttributes(attrs);

    int bytes = out->write(ackNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/** ***********************************************************************
 ** User handling methods
 **/

/**
    Sends a query to request the time when a user was last seen online.

    @param jid           Destination jid.
*/
void Connection::sendQueryLastOnline(const QString &jid)
{
    if (jid.contains("-"))
        return;

    QString id = makeId("last_");

    ProtocolTreeNode queryNode("query");

    ProtocolTreeNode iqNode("iq");

    AttributeList attrs;
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",jid);
    attrs.insert("xmlns","jabber:iq:last");
    iqNode.setAttributes(attrs);
    iqNode.addChild(queryNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a query to get the current status of a user.

    @param jid           Destination jid.
*/
void Connection::sendGetStatus(const QStringList &jids)
{
    QString id = makeId("syncgetstatus_");


    ProtocolTreeNode statusNode("status");

    AttributeList attrs;

    foreach (QString jid, jids) {
        if (jid.contains("@"))
        {
            ProtocolTreeNode userNode("user");
            attrs.clear();
            attrs.insert("jid", jid);
            userNode.setAttributes(attrs);
            statusNode.addChild(userNode);
        }
    }

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("to", "s.whatsapp.net");
    attrs.insert("type", "get");
    attrs.insert("xmlns", "status");
    iqNode.setAttributes(attrs);
    iqNode.addChild(statusNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendSetStatus(const QString &status)
{
    QString id = makeId("sendstatus_");

    ProtocolTreeNode iqNode("iq");
    AttributeList attrs;
    attrs.insert("to", domain);
    attrs.insert("type", "set");
    attrs.insert("id", id);
    attrs.insert("xmlns", "status");
    iqNode.setAttributes(attrs);

    ProtocolTreeNode statusNode("status");
    statusNode.setData(status.toUtf8());

    iqNode.addChild(statusNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a query to request a subscription to a user.

    Subscriptions allow servers to send user online status changes in real time
    to this client.

    @param jid           Destination jid.
*/
void Connection::sendPresenceSubscriptionRequest(const QString &jid)
{
    ProtocolTreeNode presenceNode("presence");

    AttributeList attrs;
    attrs.insert("to", jid);
    attrs.insert("type", "subscribe");
    presenceNode.setAttributes(attrs);

    int bytes = out->write(presenceNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a query to request a subscription remove to a user.

    Subscriptions allow servers to send user online status changes in real time
    to this client.

    @param jid           Destination jid.
*/
void Connection::sendUnsubscribeHim(const QString &jid)
{
    ProtocolTreeNode presenceNode("presence");

    AttributeList attrs;
    attrs.insert("to", jid);
    attrs.insert("type", "unsubscribe");
    presenceNode.setAttributes(attrs);

    int bytes = out->write(presenceNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendDeleteFromRoster(const QString &jid)
{
    QString id = makeId("roster_");
    ProtocolTreeNode innerChild("item");
    AttributeList attrs;
    attrs.insert("jid", jid);
    attrs.insert("subscription", "remove");
    innerChild.setAttributes(attrs);
    ProtocolTreeNode child("query");
    attrs.clear();
    attrs.insert("xmlns", "jabber:iq:roster");
    child.setAttributes(attrs);
    child.addChild(innerChild);
    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "set");
    //attrs.insert("xmlns", "jabber:iq:roster");
    iqNode.setAttributes(attrs);
    iqNode.addChild(child);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/** ***********************************************************************
 ** Picture handling methods
 **/

/**
    Sends a query to request to get the current profile picture of a user.

    If the profile picture of a user that is currently stored in the servers
    have the same photoId than the request then the profile picture is not
    sent again by the servers.

    @param jid              Destination jid.
    @param expectedPhotoId  The expected photo Id. If this string is empty then
                            a new photo will always be received.  If the photo Id
                            is the same that the servers have they won't send the same
                            photo again.
    @param largeFormat      True to request the full resolution picture.  False to
                            request just a preview.
*/
void Connection::sendGetPhoto(const QString &jid, const QString &expectedPhotoId, bool largeFormat)
{
    AttributeList attrs;

    QString id = makeId("get_picture_");

    ProtocolTreeNode pictureNode("picture");

    attrs.insert("type", largeFormat ? "image" : "preview");

    if (!expectedPhotoId.isEmpty() && expectedPhotoId != "abook")
        attrs.insert("id", expectedPhotoId);

    pictureNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",jid);
    attrs.insert("xmlns", "w:profile:picture");
    iqNode.setAttributes(attrs);
    iqNode.addChild(pictureNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to set a photo.

    The photo can be the user's own profile photo (if the jid is the same as the
    user's jid) or a group icon (the user has to belong to that group).

    Currently the thumbnail is not supported. It is being ignored and not sent to
    the servers.

    @param jid          Destination jid.
    @param imageBytes   Image bytes. The image has to be in JPEG format.
    @param thumbBytes   Thumbnail image bytes. The thumbnail has to be in JPEG format.
*/
void Connection::sendSetPhoto(const QString &jid, const QByteArray &imageBytes, const QByteArray &thumbBytes)
{
    AttributeList attrs;

    QString id = makeId("set_picture_");

    ProtocolTreeNode pictureNode("picture");
    pictureNode.setData(imageBytes);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("to",jid);
    attrs.insert("xmlns", "w:profile:picture");
    /*if (!thumbBytes.isEmpty()) {
        attrs.insert("type", "image");
    }*/
    iqNode.setAttributes(attrs);
    iqNode.addChild(pictureNode);

    if (!thumbBytes.isEmpty()) {
        attrs.clear();
        ProtocolTreeNode thumbNode("picture");
        attrs.insert("type","preview");
        thumbNode.setData(thumbBytes);
        thumbNode.setAttributes(attrs);
        iqNode.addChild(thumbNode);
    }

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProfileBytes, 0, bytes);
}

/**
    Sends a request to get the photo Ids from a list of jids.

    @param jids         QStringList containing the jids.
*/
void Connection::sendGetPhotoIds(const QStringList &jids)
{
    AttributeList attrs;

    QString id = makeId("get_picture_id_");

    ProtocolTreeNode listNode("list");
    listNode.setAttributes(attrs);

    foreach (QString jid, jids)
    {
        ProtocolTreeNode userNode("user");
        attrs.insert("jid", jid);
        userNode.setAttributes(attrs);
        listNode.addChild(userNode);
    }

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",myJid);
    attrs.insert("xmlns","w:profile:picture");
    iqNode.setAttributes(attrs);
    iqNode.addChild(listNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a notification that a voice note was played

    @param message      Voice note that was played by the user
*/
void Connection::sendVoiceNotePlayed(const FMessage &message)
{
    AttributeList attrs;

    ProtocolTreeNode receivedNode("received");
    attrs.clear();
    attrs.insert("xmlns","urn:xmpp:receipts");
    attrs.insert("type","played");
    receivedNode.setAttributes(attrs);
    ProtocolTreeNode messageNode = getMessageNode(message, receivedNode);

    int bytes = out->write(messageNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}


/** ***********************************************************************
 ** Typing status handling methods
 **/

/**
    Sends a notification that the user is composing a message.

    @param to   Destination jid.
*/
void Connection::sendComposing(const QString &jid, const QString &media)
{
    ProtocolTreeNode composingNode("composing");
    if (!media.isEmpty()) {
        AttributeList attrs;
        attrs.insert("media", media);
        composingNode.setAttributes(attrs);
    }

    ProtocolTreeNode messageNode("chatstate");
    AttributeList attrs;
    attrs.insert("to", jid);
    messageNode.setAttributes(attrs);
    messageNode.addChild(composingNode);

    int bytes = out->write(messageNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a notification that the user has stopped typing a message.

    @param to   Destination jid.
*/
void Connection::sendPaused(const QString &jid, const QString &media)
{
    ProtocolTreeNode pausedNode("paused");
    if (!media.isEmpty()) {
        AttributeList attrs;
        attrs.insert("media", media);
        pausedNode.setAttributes(attrs);
    }

    ProtocolTreeNode messageNode("chatstate");
    AttributeList attrs;
    attrs.insert("to", jid);
    messageNode.setAttributes(attrs);
    messageNode.addChild(pausedNode);

    int bytes = out->write(messageNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}


/** ***********************************************************************
 ** Group handling
 **/

/**
    Sends a request to create a group.

    @param subject  Group name or subject.
    @param id       Id of the request.
*/
void Connection::sendCreateGroupChat(const QString &subject)
{
    ProtocolTreeNode groupNode("group");

    AttributeList attrs;
    attrs.insert("action", "create");
    attrs.insert("subject", subject);
    groupNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    QString id = makeId("create_group_");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("to","g.us");
    attrs.insert("xmlns", "w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(groupNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to add participants to a group.

    @param gjid             Group jid.
    @param participants     QStringList containing the jids of the participants
                            to add.
*/
void Connection::sendAddParticipants(const QString &gjid, const QStringList &participants)
{
    QString id = makeId("add_group_participants_");

    sendVerbParticipants(gjid, participants, id, "add");
}

/**
    Sends a request to remove participants from a group.

    @param gjid             Group jid.
    @param participants     QStringList containing the jids of the participants
                            to remove.
*/
void Connection::sendRemoveParticipants(const QString &gjid, const QStringList &participants)
{
    QString id = makeId("remove_group_participants_");

    sendVerbParticipants(gjid, participants, id, "remove");
}

/**
    Sends a request with participants to a group.

    @param gjid             Group jid.
    @param participants     QStringList containing the jids of the participants
                            to remove.
    @param id               Id of the request.
    @param innerTag         Tag of the inner node.  Usually contains the action
                            to be applied to the participants, like "add" or
                            "remove".
*/
void Connection::sendVerbParticipants(const QString &gjid, const QStringList &participants,
                                      const QString &id, const QString &innerTag)
{
    ProtocolTreeNode innerNode(innerTag);
    AttributeList attrs;

    foreach (QString jid, participants)
    {
        if (jid != myJid) {
            ProtocolTreeNode participantNode("participant");
            attrs.clear();
            attrs.insert("jid", jid);
            participantNode.setAttributes(attrs);
            innerNode.addChild(participantNode);
        }
    }

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("to",gjid);
    attrs.insert("xmlns", "w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(innerNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to retrieve the participants list of a group

    @param gjid             Group jid.
*/
void Connection::sendGetParticipants(const QString &gjid)
{
    ProtocolTreeNode listNode("list");

    QString id = makeId("get_participants_");

    AttributeList attrs;
    listNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",gjid);
    attrs.insert("xmlns", "w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(listNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendGetGroupInfo(const QString &gjid)
{
    ProtocolTreeNode listNode("query");

    QString id = makeId("get_g_info_");

    AttributeList attrs;
    listNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",gjid);
    attrs.insert("xmlns", "w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(listNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to retrieve the participants list of all groups
*/
void Connection::updateGroupChats()
{
    QString id = makeId("get_groups_");
    sendGetGroups(id,"participating");
}

/**
    Sends a request to all groups.

    This method retrieves an specific information from all groups. The type argument
    indicates the information to be retrieved.

    @param id       Id of the request.
    @param type     Type of the operation. Example: "participating" to get the list
                    of all participants of every group the user belongs to.
*/
void Connection::sendGetGroups(const QString &id, const QString &type)
{
    AttributeList attrs;

    ProtocolTreeNode listNode("list");
    attrs.insert("type",type);
    listNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to","g.us");
    attrs.insert("xmlns","w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(listNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to change/set a group subject.

    @param gjid     Group jid.
    @param subject  New subject/name for the group
*/
void Connection::sendSetGroupSubject(const QString &gjid, const QString &subject)
{
    QString id = makeId("set_group_subject_");

    AttributeList attrs;

    ProtocolTreeNode subjectNode("subject");
    attrs.insert("value",subject);
    subjectNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");

    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("to",gjid);
    attrs.insert("xmlns","w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(subjectNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to leave a group.

    @param gjid     Group jid.
*/
void Connection::sendLeaveGroup(const QString &gjid)
{
    QString id = makeId("leave_group_");

    AttributeList attrs;

    ProtocolTreeNode groupNode("group");
    attrs.insert("id",gjid);
    groupNode.setAttributes(attrs);

    ProtocolTreeNode leaveNode("leave");
    leaveNode.addChild(groupNode);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("to","g.us");
    attrs.insert("xmlns","w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(leaveNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendRemoveGroup(const QString &gjid)
{
    QString id = makeId("remove_group_");

    ProtocolTreeNode groupNode("group");
    AttributeList attrs;
    attrs.insert("action", "delete");
    groupNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "set");
    attrs.insert("to", gjid);
    attrs.insert("xmlns", "w:g");
    iqNode.setAttributes(attrs);
    iqNode.addChild(groupNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/** ***********************************************************************
 ** Privacy list methods
 **/

/**
    Sends a request to get the privacy list.
    The privacy list is the same than the blocked contacts list.
*/
void Connection::sendGetPrivacyList()
{
    QString id = makeId("privacylist_");

    AttributeList attrs;

    ProtocolTreeNode listNode("list");
    attrs.insert("name","default");
    listNode.setAttributes(attrs);

    ProtocolTreeNode queryNode("query");
    attrs.clear();
    queryNode.setAttributes(attrs);
    queryNode.addChild(listNode);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("xmlns","jabber:iq:privacy");
    iqNode.setAttributes(attrs);
    iqNode.addChild(queryNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a request to set the privacy blocked list

    @param jidList  List of JIDs that are blocked.
*/
void Connection::sendSetPrivacyBlockedList(const QStringList &jidList)
{
    QString id = makeId("privacy_");

    AttributeList attrs;

    ProtocolTreeNode listNode("list");
    attrs.insert("name","default");
    listNode.setAttributes(attrs);

    int order = 1;
    foreach(QString jid, jidList)
    {
        ProtocolTreeNode itemNode("item");
        attrs.clear();
        attrs.insert("type","jid");
        attrs.insert("value",jid);
        attrs.insert("action","deny");
        attrs.insert("order",QString::number(order++));
        itemNode.setAttributes(attrs);
        listNode.addChild(itemNode);
    }

    ProtocolTreeNode queryNode("query");
    attrs.clear();
    queryNode.setAttributes(attrs);
    queryNode.addChild(listNode);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","set");
    attrs.insert("xmlns","jabber:iq:privacy");
    iqNode.setAttributes(attrs);
    iqNode.addChild(queryNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendGetPrivacySettings()
{
    QString id = makeId("privacysettings_");

    AttributeList attrs;

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("to",JID_DOMAIN);
    attrs.insert("type","get");
    attrs.insert("xmlns","privacy");
    iqNode.setAttributes(attrs);

    ProtocolTreeNode privacyNode("privacy");
    iqNode.addChild(privacyNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendSetPrivacySettings(const QString &name, const QString &value)
{
    QString id = makeId("setprivacy_");

    AttributeList attrs;

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("to",JID_DOMAIN);
    attrs.insert("type","set");
    attrs.insert("xmlns","privacy");
    iqNode.setAttributes(attrs);

    ProtocolTreeNode privacyNode("privacy");
    ProtocolTreeNode categoryNode("category");
    attrs.clear();
    attrs.insert("name", name);
    attrs.insert("value", value);
    categoryNode.setAttributes(attrs);
    privacyNode.addChild(categoryNode);
    iqNode.addChild(privacyNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/** ***********************************************************************
 ** General methods
 **/

/**
    Sends a no operation (ping) to the network.
*/
void Connection::sendNop()
{

    ProtocolTreeNode empty;

    int bytes = out->write(empty);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a ping request to the network.
*/
void Connection::sendPing()
{
    QString id = makeId("ping_");

    AttributeList attrs;
    ProtocolTreeNode pingNode("ping");
    attrs.insert("xmlns","w:p");
    pingNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    iqNode.setAttributes(attrs);
    iqNode.addChild(pingNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends a ping acknowledge (pong) to the network.

    @param id   Id of the ping received.
*/
void Connection::sendPong(const QString &id)
{
    AttributeList attrs;
    ProtocolTreeNode iqNode("iq");

    attrs.insert("id",id);
    attrs.insert("type","result");
    attrs.insert("to",domain);
    iqNode.setAttributes(attrs);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Constructs an id.

    @param prexix   String containing the prefix of the id.
*/
QString Connection::makeId(const QString &prefix)
{
    return prefix + QString::number(++iqid,16);
}

/**
    Changes the user name or alias.

    @param push_name    New user name or alias.
*/
void Connection::setNewUserName(const QString &push_name, bool hide)
{
    this->push_name = push_name;
    sendAvailableForChat(hide);
}

/**
    Sends the client configuration.

    @param platform     Platform that this client is running on.
*/
void Connection::sendClientConfig(const QString &platform)
{
    QString id = makeId("config_");

    AttributeList attrs;

    ProtocolTreeNode configNode("config");
    attrs.insert("platform", platform);
    attrs.insert("lg", language.isEmpty() ? "en" : language);
    attrs.insert("lc", country.isEmpty() ? "US" : country);
    attrs.insert("clear", "1");
    attrs.insert("preview", "1");
    attrs.insert("default", "1");
    attrs.insert("groups", "1");
    attrs.insert("id", "none");
    attrs.insert("version", "3");
    configNode.setAttributes(attrs);

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "set");
    attrs.insert("to", domain);
    attrs.insert("xmlns", "urn:xmpp:whatsapp:push");
    iqNode.setAttributes(attrs);
    iqNode.addChild(configNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::getClientConfig()
{
    QString id = makeId("get_config_");

    AttributeList attrs;

    ProtocolTreeNode configNode("config");

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id", id);
    attrs.insert("type", "get");
    attrs.insert("to", domain);
    attrs.insert("xmlns", "urn:xmpp:whatsapp:push");
    iqNode.setAttributes(attrs);
    iqNode.addChild(configNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendGetServerProperties()
{
    QString id = makeId("get_server_properties_");

    AttributeList attrs;

    ProtocolTreeNode propsNode("props");

    ProtocolTreeNode iqNode("iq");
    attrs.clear();
    attrs.insert("id",id);
    attrs.insert("type","get");
    attrs.insert("to",domain);
    attrs.insert("xmlns","w");
    iqNode.setAttributes(attrs);
    iqNode.addChild(propsNode);

    int bytes = out->write(iqNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

/**
    Sends notification that this client is available for chat (online).
*/
void Connection::sendAvailableForChat(bool hide)
{
    ProtocolTreeNode presenceNode("presence");

    AttributeList attrs;
    attrs.insert("name", push_name);
    attrs.insert("type", hide ? "unavailable" : "available");
    presenceNode.setAttributes(attrs);

    int bytes = out->write(presenceNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendAvailable()
{
    ProtocolTreeNode presenceNode("presence");

    AttributeList attrs;
    attrs.insert("type", "available");
    presenceNode.setAttributes(attrs);

    int bytes = out->write(presenceNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendUnavailable()
{
    ProtocolTreeNode presenceNode("presence");

    AttributeList attrs;
    attrs.insert("type", "unavailable");
    presenceNode.setAttributes(attrs);

    int bytes = out->write(presenceNode);
    counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}

void Connection::sendDeleteAccount()
{
    QString id = makeId("del_acct_");

     AttributeList attrs;

     ProtocolTreeNode removeNode("remove");

     ProtocolTreeNode iqNode("iq");
     attrs.clear();
     attrs.insert("id",id);
     attrs.insert("type","get");
     attrs.insert("to",domain);
     attrs.insert("xmlns","urn:xmpp:whatsapp:account");
     iqNode.setAttributes(attrs);
     iqNode.addChild(removeNode);

     int bytes = out->write(iqNode);
     counters->increaseCounter(DataCounters::ProtocolBytes, 0, bytes);
}
