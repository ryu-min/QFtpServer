#include "ftplistcommand.h"

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QSslSocket>

BEGIN_FTP_NAMESPACE

FtpListCommand::FtpListCommand(QObject *parent,
                               const QString &fileName,
                               bool nameListOnly)
    : FtpCommand(parent)
    , _listDirectory(fileName)
    , _nameListOnly(nameListOnly)
    , _timer(nullptr)
    , _list(nullptr)
    , _index(0)
{}

FtpListCommand::~FtpListCommand()
{
    if (_started) {
        emit reply("226 Closing data connection.");
    }
}

void FtpListCommand::startImplementation()
{
    QFileInfo info(_listDirectory);

    if (!info.isReadable()) {
        emit reply("425 File or directory is not readable or doesn't exist.");
        _socket->disconnectFromHost();
        return;
    }

    emit reply("150 File status okay; about to open data connection.");

    _index = 0;
    _list = new QFileInfoList;
    if (!info.isDir()) {
        *_list = (QFileInfoList() << info);
    } else {
        *_list = QDir(_listDirectory).entryInfoList();
    }

    // Start the timer.
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(listNextBatch()));
    _timer->start(0);
}

QString padded(QString s, int n)
{
    while (s.size() < n) {
        s = ' ' + s;
    }
    return s;
}

QString FtpListCommand::fileListingString(const QFileInfo &fi)
{
    // This is how the returned list looks. It is like what is returned by
    // 'ls -l':
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
    if (!_nameListOnly) {
        // Directory/symlink/file.
        if (fi.isSymLink()) {
            line += 'l';
        } else if (fi.isDir()) {
            line += 'd';
        } else {
            line += '-';
        }

        // Permissions.
        QFile::Permissions p = fi.permissions();
        line += (p & QFile::ReadOwner) ? 'r' : '-';
        line += (p & QFile::WriteOwner) ? 'w' : '-';
        line += (p & QFile::ExeOwner) ? 'x' : '-';
        line += (p & QFile::ReadGroup) ? 'r' : '-';
        line += (p & QFile::WriteGroup) ? 'w' : '-';
        line += (p & QFile::ExeGroup) ? 'x' : '-';
        line += (p & QFile::ReadOther) ? 'r' : '-';
        line += (p & QFile::WriteOther) ? 'w' : '-';
        line += (p & QFile::ExeOther) ? 'x' : '-';

        // Owner/group.
        QString owner = fi.owner();
        if (owner.isEmpty()) {
            owner = "unknown";
        }
        QString group = fi.group();
        if (group.isEmpty()) {
            group = "unknown";
        }
        line += ' ' + padded(owner, 10) + ' ' + padded(group, 10);

        // File size.
        line += ' ' + padded(QString::number(fi.size()), 14);

        // Last modified - note we **must** use english locale, otherwise FTP clients won't understand us.
        QLocale locale(QLocale::English);
        QDateTime lm = fi.lastModified();
        if (lm.date().year() != QDate::currentDate().year()) {
            line += ' ' + locale.toString(lm.date(), "MMM dd  yyyy");
        } else {
            line += ' ' + locale.toString(lm.date(), "MMM dd") + ' ' + locale.toString(lm.time(), "hh:mm");
        }
    }
    line += ' ' + fi.fileName();
    line += "\r\n";
    return line;
}

void FtpListCommand::listNextBatch()
{
    // List next 10 items.
    int stop = qMin(_index + 10, _list->size());
    while (_index < stop) {
        QString line = fileListingString(_list->at(_index));
        _socket->write(line.toUtf8());
        _index++;
    }

    // If all files have been listed, then finish.
    if (_list->size() == stop) {
        delete _list;
        _timer->stop();
        _socket->disconnectFromHost();
    }
}

END_FTP_NAMESPACE
