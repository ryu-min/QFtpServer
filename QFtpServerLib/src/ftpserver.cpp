#include "ftpserver.h"
#include "ftpcontrolconnection.h"
#include "sslserver.h"

#include <QDebug>
#include <QNetworkInterface>
#include <QSslSocket>

BEGIN_FTP_NAMESPACE

FtpServer::FtpServer(QObject *parent,
                     const QString &rootPath,
                     int port,
                     const QString &userName,
                     const QString &password,
                     bool readOnly,
                     bool onlyOneIpAllowed)
    : QObject(parent)
    , _server(new SslServer(this))
    , _userName(userName)
    , _password(password)
    , _rootPath(rootPath)
    , _encounteredIps()
    , _readOnly(readOnly)
    , _onlyOneIpAllowed(onlyOneIpAllowed)
{
    // In Qt4, QHostAddress::Any listens for IPv4 connections only, but as of
    // Qt5, it now listens on all available interfaces, and
    // QHostAddress::AnyIPv4 needs to be used if we want only IPv4 connections.
#if QT_VERSION >= 0x050000
    _server->listen(QHostAddress::AnyIPv4, port);
#else
    _server->listen(QHostAddress::Any, port);
#endif
    connect(_server, SIGNAL(newConnection()), this, SLOT(startNewControlConnection()));
}

bool FtpServer::isListening()
{
    return _server->isListening();
}

QString FtpServer::lanIp()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            return address.toString();
        }
    }
    return "";
}

void FtpServer::startNewControlConnection()
{
    QSslSocket *socket = (QSslSocket *) _server->nextPendingConnection();

    // If this is not a previously encountered IP emit the newPeerIp signal.
    QString peerIp = socket->peerAddress().toString();
    qDebug() << "connection from" << peerIp;
    if (!_encounteredIps.contains(peerIp)) {
        // If we don't allow more than one IP for the client, we close
        // that connection.
        if (_onlyOneIpAllowed && !_encounteredIps.isEmpty()) {
            delete socket;
            return;
        }

        emit newPeerIp(peerIp);
        _encounteredIps.insert(peerIp);
    }

    // Create a new FTP control connection on this socket.
    new FtpControlConnection(this, socket, _rootPath, _userName, _password, _readOnly);
}

END_FTP_NAMESPACE
