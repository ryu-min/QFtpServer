#include "ftpcommand.h"

#include <QSslSocket>

BEGIN_FTP_NAMESPACE

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

END_FTP_NAMESPACE
