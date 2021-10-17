#include "dataconnection.h"
#include "sslserver.h"
#include "ftpcommand.h"
#include <QSslSocket>

DataConnection::DataConnection(QObject *parent)
    : QObject(parent)
    , _server(new SslServer(this))
    , _socket(nullptr)
    , _command()
    , _hostName()
    , _port(0)
    , _isSocketReady(false)
    , _isWaitingForFtpCommand(false)
    , _encrypt(false)
    , _isActiveConnection(false)
{
    connect(_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void DataConnection::scheduleConnectToHost(const QString &hostName, int port, bool encrypt)
{
    this->_encrypt = encrypt;
    delete _socket;
    this->_hostName = hostName;
    this->_port = port;
    _isSocketReady = false;
    _isWaitingForFtpCommand = true;
    _isActiveConnection = true;
}

int DataConnection::listen(bool encrypt)
{
    this->_encrypt = encrypt;
    delete _socket;
    _socket = 0;
    delete _command;
    _command = 0;
    _isSocketReady = false;
    _isWaitingForFtpCommand = true;
    _isActiveConnection = false;
    _server->close();
    _server->listen();
    return _server->serverPort();
}

bool DataConnection::setFtpCommand(FtpCommand *command)
{
    if (!_isWaitingForFtpCommand) {
        return false;
    }
    _isWaitingForFtpCommand = false;
    this->_command = command;
    command->setParent(this);

    if (_isActiveConnection) {
        _socket = new QSslSocket(this);
        connect(_socket, SIGNAL(connected()), SLOT(connected()));
        _socket->connectToHost(_hostName, _port);
    } else {
        startFtpCommand();
    }
    return true;
}

FtpCommand *DataConnection::ftpCommand()
{
    if (_isSocketReady) {
        return _command;
    }
    return 0;
}

void DataConnection::newConnection()
{
    _socket = (QSslSocket *) _server->nextPendingConnection();
    _server->close();
    if (_encrypt) {
        connect(_socket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        SslServer::setLocalCertificateAndPrivateKey(_socket);
        _socket->startServerEncryption();
    } else {
        encrypted();
    }
}

void DataConnection::encrypted()
{
    _isSocketReady = true;
    startFtpCommand();
}

void DataConnection::connected()
{
    if (_encrypt) {
        connect(_socket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        SslServer::setLocalCertificateAndPrivateKey(_socket);
        _socket->startServerEncryption();
    } else {
        encrypted();
    }
}

void DataConnection::startFtpCommand()
{
    if (_command && _isSocketReady) {
        _command->start(_socket);
        _socket = 0;
    }
}
