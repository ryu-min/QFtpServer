#ifndef FTPLISTCOMMAND_H
#define FTPLISTCOMMAND_H

#include "ftpcommand.h"
#include <qftpserverlib_global.h>

class QTimer;
class QFileInfo;

typedef QList<QFileInfo> QFileInfoList;

// Implements the LIST command. Prints out a listing of the given directory, in
// the same format as the unix 'ls -l' command.

BEGIN_FTP_NAMESPACE

class FtpListCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpListCommand(QObject *parent, const QString &_listDirectory, bool _nameListOnly = false);
    ~FtpListCommand();

private:
    void startImplementation() override;
    QString fileListingString(const QFileInfo &fi);

private slots:
    void listNextBatch();

private:
    QString _listDirectory;
    bool _nameListOnly;
    QTimer *_timer;
    QFileInfoList *_list;
    int _index;
};

END_FTP_NAMESPACE

#endif // FTPLISTCOMMAND_H
