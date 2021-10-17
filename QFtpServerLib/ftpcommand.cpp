#include "ftpcommand.h"

#include <QSslSocket>

FtpCommand::FtpCommand(QObject *parent)
    : QObject(parent)
    , _socket(nullptr)
    , _started(false)
{}

void FtpCommand::start(QSslSocket *socket)
{
    _started = true;
    this->_socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    startImplementation();
}
