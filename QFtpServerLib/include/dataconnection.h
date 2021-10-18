#ifndef PASSIVEDATACONNECTION_H
#define PASSIVEDATACONNECTION_H

#include <qftpserverlib_global.h>

#include <QObject>
#include <QPointer>

// This class encapsulates the logic of the creation of both passive and active
// data connections. In short - there can be only one data connection per
// control connection. If a new one is requested while there is one being used
// currently, the operation using it is aborted. A new data connection starts
// when a PASV or PORT command is issued. Then the user must specify a command
// that uses the data connection, such as LIST or RETR/STOR. If we have
// encryption enabled, the data connection is also first encrypted. Once we
// have the command, and are connected to the client, and the connection has
// been encrypted, we run the command.

class QSslSocket;

BEGIN_FTP_NAMESPACE

class SslServer;
class FtpCommand;

class DataConnection : public QObject
{
    Q_OBJECT
public:
    explicit DataConnection(QObject *parent = 0);

    // Connects to a host. Any existing data connections
    // or commands are aborted.
    void scheduleConnectToHost(const QString &_hostName, int _port, bool _encrypt);

    // Starts listening for new data connections. Any existing data connections
    // or commands are aborted.
    int listen(bool _encrypt);

    // Sets the ftp command. This function can be called only once after each
    // call of listen().
    bool setFtpCommand(FtpCommand *_command);

    // Returns the currently running ftpCommand, if it is already running, but
    // not yet finished. Otherwise returns 0.
    FtpCommand *ftpCommand();

private slots:
    void newConnection();
    void encrypted();
    void connected();

private:
    void startFtpCommand();

private:
    SslServer *_server;
    QSslSocket *_socket;
    QPointer<FtpCommand> _command;
    QString _hostName;
    int _port;
    bool _isSocketReady;
    bool _isWaitingForFtpCommand;
    bool _encrypt;
    bool _isActiveConnection;

};

END_FTP_NAMESPACE

#endif // PASSIVEDATACONNECTION_H
