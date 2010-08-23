#-------------------------------------------------
#
# Project created by QtCreator 2010-08-20T17:08:57
#
#-------------------------------------------------

QT       += core gui

TARGET = podcatcher_qt
TEMPLATE = app


SOURCES += main.cpp\
        podcatcherwin.cpp

HEADERS  += podcatcherwin.h

FORMS    += podcatcherwin.ui

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe905eb0b
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
