QT       += network
QT       -= gui

TARGET = QFtpServerLib
TEMPLATE = lib
CONFIG += c++14

DEFINES += QFTPSERVERLIB_LIBRARY
INCLUDEPATH += $$PWD/include/

SOURCES += $$PWD/src/*.cpp
HEADERS += $$PWD/include/*.h
RESOURCES += $$PWD/permisions/*.qrc

unix {
    target.path = /usr/lib
    INSTALLS += target
}

