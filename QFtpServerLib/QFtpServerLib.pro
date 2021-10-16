QT       += network
QT       -= gui

TARGET = QFtpServerLib
TEMPLATE = lib
CONFIG += c++14

DEFINES += QFTPSERVERLIB_LIBRARY

SOURCES += \
    dataconnection.cpp \
    ftpcommand.cpp \
    ftpcontrolconnection.cpp \
    ftplistcommand.cpp \
    ftpretrcommand.cpp \
    ftpserver.cpp \
    ftpstorcommand.cpp \
    sslserver.cpp

HEADERS +=\
        qftpserverlib_global.h \
    dataconnection.h \
    ftpcommand.h \
    ftpcontrolconnection.h \
    ftplistcommand.h \
    ftpretrcommand.h \
    ftpserver.h \
    ftpstorcommand.h \
    sslserver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    certificates.qrc
