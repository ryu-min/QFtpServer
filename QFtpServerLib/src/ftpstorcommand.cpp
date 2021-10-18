#include "ftpstorcommand.h"
#include <QFile>
#include <QSslSocket>

BEGIN_FTP_NAMESPACE

FtpStorCommand::FtpStorCommand(QObject *parent,
                               const QString &fileName,
                               bool appendMode,
                               qint64 seekTo)
    : FtpCommand(parent)
    , _fileName(fileName)
    , _file(nullptr)
    , _appendMode(appendMode)
    , _seekTo(seekTo)
    , _success(false)

{}

FtpStorCommand::~FtpStorCommand()
{
    if (_started) {
        if (_success) {
            emit reply("226 Closing data connection.");
        } else {
            emit reply("451 Requested action aborted: local error in processing.");
        }
    }
}

void FtpStorCommand::startImplementation()
{
    _file = new QFile(_fileName, this);
    if (!_file->open(_appendMode ? QIODevice::Append : QIODevice::WriteOnly)) {
        deleteLater();
        return;
    }
    _success = true;
    emit reply("150 File status okay; about to open data connection.");
    if (_seekTo) {
        _file->seek(_seekTo);
    }
    connect(_socket, SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void FtpStorCommand::acceptNextBlock()
{
    const QByteArray &bytes = _socket->readAll();
    int bytesWritten = _file->write(bytes);
    if (bytesWritten != bytes.size()) {
        emit reply("451 Requested action aborted. Could not write data to file.");
        deleteLater();
    }
}

END_FTP_NAMESPACE
