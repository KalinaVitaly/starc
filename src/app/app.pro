#-------------------------------------------------
#
# Project created by QtCreator 2019-08-27T08:03:13
#
#-------------------------------------------------

TEMPLATE = app

CONFIG += c++11
QT += core gui widgets

TARGET = starcapp

DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR = ../_build/

INCLUDEPATH += ..
LIBS += -L$$DESTDIR

SOURCES += \
        application.cpp \
        main.cpp

HEADERS += \
    application.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
