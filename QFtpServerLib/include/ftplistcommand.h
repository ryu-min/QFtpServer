#ifndef FTPLISTCOMMAND_H
#define FTPLISTCOMMAND_H

#include "ftpcommand.h"

class QTimer;
class QFileInfo;

typedef QList<QFileInfo> QFileInfoList;

// Implements the LIST command. Prints out a listing of the given directory, in
// the same format as the unix 'ls -l' command.

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

#endif // FTPLISTCOMMAND_H
