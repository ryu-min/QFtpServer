#include "ftpretrcommand.h"
#include <QFile>
#include <QSslSocket>

BEGIN_FTP_NAMESPACE

FtpRetrCommand::FtpRetrCommand(QObject *parent,
                               const QString &fileName,
                               qint64 seekTo)
    : FtpCommand(parent)
    , _fileName(fileName)
    , _seekTo(seekTo)
    , _file(nullptr)
{}

FtpRetrCommand::~FtpRetrCommand()
{
    if (_started) {
        if (_file && _file->isOpen() && _file->atEnd()) {
            emit reply("226 Closing data connection.");
        } else {
            emit reply("550 Requested action not taken; file unavailable.");
        }
    }
}

void FtpRetrCommand::startImplementation()
{
    _file = new QFile(_fileName, this);
    if (!_file->open(QIODevice::ReadOnly)) {
        deleteLater();
        return;
    }
    emit reply("150 File status okay; about to open data connection.");
    if (_seekTo) {
        _file->seek(_seekTo);
    }

    // For encryted SSL sockets, we need to use the encryptedBytesWritten()
    // signal, see the QSslSocket documentation to for reasons why.
    if (_socket->isEncrypted()) {
        connect(_socket, SIGNAL(encryptedBytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    } else {
        connect(_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    }

    refillSocketBuffer(128*1024);
}

void FtpRetrCommand::refillSocketBuffer(qint64 bytes)
{
    if (!_file->atEnd()) {
        _socket->write(_file->read(bytes));
    } else {
        _socket->disconnectFromHost();
    }
}

END_FTP_NAMESPACE
