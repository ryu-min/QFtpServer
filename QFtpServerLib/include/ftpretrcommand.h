#ifndef FTPRETRCOMMAND_H
#define FTPRETRCOMMAND_H

#include "ftpcommand.h"

class QFile;

// Implements the RETR command. Used to download files from the ftp server.

class FtpRetrCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpRetrCommand(QObject *parent, const QString &_fileName, qint64 _seekTo = 0);
    ~FtpRetrCommand();

private slots:
    void refillSocketBuffer(qint64 bytes=0);

private:
    void startImplementation();

    QString _fileName;
    qint64  _seekTo;
    QFile * _file;
};

#endif // FTPRETRCOMMAND_H
