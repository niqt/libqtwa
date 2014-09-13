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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QHash>

#include "util/messagedigest.h"
#include "util/datacounters.h"

#include "protocoltreenode.h"
#include "bintreenodewriter.h"
#include "bintreenodereader.h"
#include "protocolexception.h"
#include "loginexception.h"
#include "keystream.h"
#include "fmessage.h"
#include "funstore.h"

#include "libqtwa.h"

/**
    @class      Connection

    @brief      The Connection class provides the WhatsApp functionality.
                It represents a connection to the WhatsApp servers and provides
                functions to send and receive WhatsApp messages
*/

class LIBQTWA Connection : public QObject
{
    Q_OBJECT

public:

    /** ***********************************************************************
     ** Enumerations
     **/

    // Message Status types
    enum ChatMessageType {
        Unknown,
        MessageReceived,
        MessageStatusUpdate
    };

    /** ***********************************************************************
     ** Public members
     **/

    // Next authentication challenge
    QByteArray nextChallenge;

    // Account creation date (unixtime)
    QString creation;

    // Account expiration date (unixtime)
    QString expiration;

    // Account kind (free / paid)
    QString kind;

    // Account status (active / expired)
    QString accountstatus;


    /** ***********************************************************************
     ** Constructors and destructors
     **/

    // Create a Connection object
    explicit Connection(const QString &server, int port, const QString &domain, const QString &resource,
                        const QString &user, const QString &push_name, const QByteArray &password,
                        const QByteArray &challenge, const QString &language, const QString &country,
                        const QString &mcc, const QString &mnc, const QString version,
                        DataCounters *counters, QObject *parent = 0);

    // Destroy a Connection object
    ~Connection();


    /** ***********************************************************************
     ** General Public Methods
     **/

    // Login to the WhatsApp servers
    void login(const QByteArray &nextChallenge);

private slots:
    void connectedToServer();
    void connectionClosed();

    void socketError(QAbstractSocket::SocketError error);

    // Read next node
    void readNode();

public slots:

    /** ***********************************************************************
     ** Public slots methods
     **/

    void init();

    void disconnectAndDelete();
    void finalCleanup();

    void checkActivity();

    /** ***********************************************************************
     ** Message handling
     **/

    // Sends a FMessage
    void sendMessage(const FMessage &message);


    /** ***********************************************************************
     ** User handling
     **/

    // Sends a query to sync phone numbers
    void sendSyncContacts(const QStringList &numbers);

    // Sends a query to request the time when a user was last seen online.
    void sendQueryLastOnline(const QString &jid);

    // Sends a query to get the current status of a user
    void sendGetStatus(const QStringList &jids);

    void sendSetStatus(const QString &status);

    // Sends a query to request a subscription to a user
    void sendPresenceSubscriptionRequest(const QString &jid);

    // Sends a query to request a subscription remove to a user
    void sendUnsubscribeHim(const QString &jid);

    void sendDeleteFromRoster(const QString &jid);


    /** ***********************************************************************
     ** Picture handling
     **/

    // Sends a query to request to get the current profile picture of a user
    void sendGetPhoto(const QString &jid, const QString &expectedPhotoId, bool largeFormat);

    // Sends a request to set a photo
    void sendSetPhoto(const QString &jid, const QByteArray &imageBytes, const QByteArray &thumbBytes);

    // Sends a request to get the photo Ids from a list of jids
    void sendGetPhotoIds(const QStringList &jids);

    /** ***********************************************************************
     ** Voice notes handling
     **/

    // Sends a notification that a voice note was played
    void sendVoiceNotePlayed(const FMessage &message);

    /** ***********************************************************************
     ** Group handling
     **/

    // Sends a request to create a group
    void sendCreateGroupChat(const QString &subject);

    // Sends a request to add participants to a group
    void sendAddParticipants(const QString &gjid, const QStringList &participants);

    // Sends a request to remove participants from a group
    void sendRemoveParticipants(const QString &gjid, const QStringList &participants);

    // Sends a request with participants to a group
    void sendVerbParticipants(const QString &gjid, const QStringList &participants,
                              const QString &id, const QString &innerTag);

    // Sends a request to retrieve the participants list of a group
    void sendGetParticipants(const QString &gjid);

    // Sends a request to retrieve the info of a group
    void sendGetGroupInfo(const QString &gjid);

    // Sends a request to retrieve the participants list of all groups
    void updateGroupChats();

    // Sends a request to change/set a group subject
    void sendSetGroupSubject(const QString &gjid, const QString &subject);

    // Sends a request to leave a group
    void sendLeaveGroup(const QString &gjid);

    void sendRemoveGroup(const QString &gjid);


    /** ***********************************************************************
     ** Typing status handling
     **/

    // Sends a notification that the user is composing a message
    void sendComposing(const QString &jid, const QString &media);

    // Sends a notification that the user has stopped typing a message
    void sendPaused(const QString &jid, const QString &media);


    /** ***********************************************************************
     ** Privacy list handling
     **/

    // Sends a request to get the privacy list
    void sendGetPrivacyList();

    // Sends a request to set the privacy list
    void sendSetPrivacyBlockedList(const QStringList &jidList);

    void sendGetPrivacySettings();
    void sendSetPrivacySettings(const QString &name, const QString &value);


    /** ***********************************************************************
     ** General slots
     **/

    // Sends a no operation (ping) to the network
    void sendNop();

    // Sends a ping request to th3e network
    void sendPing();

    // Changes the user name or alias
    void setNewUserName(const QString &push_name, bool hide = false);

    void sendDeleteAccount();

    void sendGetServerProperties();

    // Sends notification that this client is available for chat (online)
    void sendAvailableForChat(bool hide = false);
    void sendAvailable();
    void sendUnavailable();

    // Sends the client configuration
    void sendClientConfig(const QString &platform);
    void getClientConfig();

private:

    /** ***********************************************************************
     ** Private members
     **/

    // Get the unixtime of the last socket successfully read
    qint64 getLastTreeReadTimestamp();

    uint lastActivity;

    // Store with messages waiting for acks
    static FunStore store;

    // TCP Socket with the TCP connection established
    QTcpSocket *socket;

    // XMPP domain.  Usually "s.whatsapp.net"
    QString domain;

    // Conenction server: c.whatsapp.net, c1.whatsapp.net, c2.whatsapp.net, c3.whatsapp.net
    QString server;

    // Connection port: 443
    int port;

    // Client language and country identification data
    QString mcc;
    QString mnc;

    QString language;
    QString country;

    // Android client version
    QString version;

    // Client resource identifier
    QString resource;

    // Phone number
    QString user;

    // User name or alias
    QString push_name;

    // User password
    QByteArray password;

    // User jid
    QString myJid;

    // XMPP dictionary
    QStringList dictionary;
    QByteArray challenge;

    // Timestamp of the last successfully node read
    qint64 lastTreeRead;

    // Unique identifier for <iq> nodes
    int iqid;

    // Pointer to the DataCounters where network counters are being kept
    DataCounters *counters;

    // Writer stream to send nodes
    BinTreeNodeWriter *out;

    // Reader stream to receive nodes
    BinTreeNodeReader *in;

    // Writer crypto stream
    KeyStream *outputKey;

    // Reader crypto stream
    KeyStream *inputKey;

    /** ***********************************************************************
     ** Private methods
     **/

    // Reading socket data
    bool read();

    // Parse a <message> node
    void parseMessageInitialTagAlreadyChecked(ProtocolTreeNode &messageNode);


    /** ***********************************************************************
     ** Authentication
     **/

    // Sends the features supported by this client to the WhatsApp servers
    int sendFeatures();

    // Sends the authentication request
    int sendAuth();

    // Constructs the authentication data to be sent
    QByteArray getAuthBlob(QByteArray &nonce);

    // Reads the features from the node and the challenge data
    QByteArray readFeaturesUntilChallengeOrSuccess(int *bytes);

    // Sends authentication response
    int sendResponse(QByteArray &challengeData);

    // Read authentication success node
    int readSuccess();

    // Parses authentication success node
    void parseSuccessNode(const ProtocolTreeNode &node);

    void sendCleanDirty(const QStringList &categories);

    void sendGetDirty();


    /** ***********************************************************************
     ** Message handling
     **/

    // Converts FMessage::MediaWAType to string
    QString getMediaWAType(int type);

    // Sends a text message
    void sendMessageWithBody(const FMessage &message);

    // Sends a request to send a multimedia message
    void requestMessageWithMedia(const FMessage &message);

    // Sends a multimedia message
    void sendMessageWithMedia(const FMessage &message);

    // Constructs a message node
    ProtocolTreeNode getMessageNode(const FMessage &message, const ProtocolTreeNode &child);

    // Send a message received acknowledgement
    void sendMessageReceived(const FMessage &message, const QString &type = "");

    // Send a notification received acknowledgement
    void sendNotificationReceived(const QString &to, const QString &id, const QString &from, const QString &participant, const QString &type, const ProtocolTreeNode &childNode = ProtocolTreeNode());

    // Constructs a receipt acknowledge node
    void getReceiptAck(ProtocolTreeNode &node, const QString &to, const QString &id,
                       const QString &receiptType);

    // Sends a receipt acknowledging a delivered message notification received
    void sendDeliveredReceiptAck(const QString &to, const QString &id, const QString &type);

    // Sends a receipt acknowledging a received message notification received
    void sendReceiptAck(QString id, QString type);


    /** ***********************************************************************
     ** Group handling
     **/

    // Sends a request to all groups
    void sendGetGroups(const QString &id, const QString &type);


    /** ***********************************************************************
     ** General methods
     **/

    // Sends a ping acknowledge (pong) to the network
    void sendPong(const QString &id);

    // Constructs an id
    QString makeId(const QString &prefix);

signals:
    // Connected to server
    void connected();

    // Disconnected from server
    void disconnected();

    // Successfully authenticated
    void authSuccess(const QString &creation, const QString &expiration,
                   const QString &kind, const QString status,
                   const QByteArray &nextChallenge);

    // Authentication failed
    void authFailed();

    // Auth data correct, but account ahs been expired
    void accountExpired(const QVariantMap &result);

    /** ***********************************************************************
     ** Message handling
     **/

    // Message received
    void messageReceived(const FMessage &message);

    // Message status update
    void messageStatusUpdate(const QString &jid, const QString &msgid, int msgstatus);

    // Upload of Multimedia message accepted
    void mediaUploadAccepted(const FMessage &message);


    /** ***********************************************************************
     ** Typing status handling
     **/

    // User is typing
    void composing(const QString &jid, const QString &media);

    // User stopped typing
    void paused(const QString &jid);


    /** ***********************************************************************
     ** User handling
     **/

    // Received pushname from user
    void updatePushname(QString jid, QString pushname);

    // User availability
    void available(const QString &jid, bool online);

    // Last seen timestamp of user
    void lastOnline(const QString &jid, qint64 timestamp);

    // User status update
    void userStatusUpdated(const QString &jid, const QString &message, int timestamp);


    /** ***********************************************************************
     ** Contact handling
     **/

    // Notification about new contact. You synced it's number in past, but it just registered in WhatsApp
    void contactAdded(const QString &jid);

    // Received successfully synced contacts list
    void contactsSynced(const QVariantList &contacts);

    // Received contacts status messages
    void contactsStatus(const QVariantList &contacts);

    // Received end of synchronization data
    void syncFinished();


    /** ***********************************************************************
     ** Picture handling
     **/

    // User photo updated notification received
    void photoIdReceived(const QString &jid, const QString &alias, const QString &author, const QString &timestamp, const QString &photoId, const QString &notificationId, bool offile = false);

    // User photo has been deleted
    void photoDeleted(const QString &jid, const QString &alias, const QString &author, const QString &timestamp, const QString &notificationId, bool offline = false);

    // User photo has been received
    void photoReceived(const QString &from, const QByteArray &data,
                       const QString &photoId, bool largeFormat);


    /** ***********************************************************************
     ** Group handling
     **/

    // Received a group from the list of groups obtained by updateGroupChats()
    void groupInfoFromList(const QString &id, const QString &from, const QString &author,
                           const QString &newSubject, const QString &creation,
                           const QString &subjectOwner, const QString &subjectTimestamp);

    // Received a new group subject (and possibly a new group)
    void groupNewSubject(const QString &from, const QString &author, const QString &authorName,
                         const QString &newSubject, const QString &creation, const QString &notificationId, bool offline = false);

    // User added to a group
    void groupAddUser(const QString &from, const QString &jid, const QString &timestamp, const QString &notificationId, bool offline = false);

    // User removed from a group
    void groupRemoveUser(const QString &from, const QString &jid, const QString &timestamp, const QString &notificationId, bool offline = false);

    // Group left
    void groupLeft(const QString &gjid);

    // User belongs to group
    void groupUser(const QString &gjid, const QString &jid);

    // Participants list belongs to group
    void groupUsers(const QString &gjid, const QStringList &jids);

    // Group error (not in the group anymore or group doesn't exist anymore)
    void groupError(const QString &gjid);

    // Group created by you
    void groupCreated(const QString &jid);


    /** ***********************************************************************
     ** Privacy list handling
     **/

    // Privacy list received
    void privacyListReceived(const QStringList &list);

    // Privacy settings received
    void privacySettingsReceived(const QVariantMap &values);

    /** ***********************************************************************
     ** General signals
     **/

    // Offline messages count received notification
    void notifyOfflineMessages(int count);

    // Stream error, connection will be closed soon
    void streamError();
};


#endif // CONNECTION_H
