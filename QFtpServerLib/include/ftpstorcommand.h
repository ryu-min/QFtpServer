#ifndef FTPSTORCOMMAND_H
#define FTPSTORCOMMAND_H

#include "ftpcommand.h"

class QFile;

// Implements the STOR and APPE commands. Used to upload files to the ftp
// server.

class FtpStorCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpStorCommand(QObject *parent, const QString &_fileName, bool _appendMode = false, qint64 _seekTo = 0);
    ~FtpStorCommand();

private slots:
    void acceptNextBlock();

private:
    void startImplementation();

    QString  _fileName;
    QFile    *_file;
    bool    _appendMode;
    qint64  _seekTo;
    bool    _success;
};

#endif // FTPSTORCOMMAND_H
