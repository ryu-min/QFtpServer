#ifndef FTPCOMMAND_H
#define FTPCOMMAND_H

#include <QObject>

#include <qftpserverlib_global.h>

class QSslSocket;

// The base class for all FTP commands that need a data connection.

BEGIN_FTP_NAMESPACE

class FtpCommand : public QObject
{
    Q_OBJECT
public:
    explicit FtpCommand(QObject *parent = 0);

signals:
    void reply(const QString &details);

public:
    void start(QSslSocket *socket);

protected:
    virtual void startImplementation() = 0;

protected:
    QSslSocket* _socket;
    bool _started;
};

END_FTP_NAMESPACE

#endif // FTPCOMMAND_H
