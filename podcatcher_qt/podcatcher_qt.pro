#-------------------------------------------------
#
# Project created by QtCreator 2010-08-20T17:08:57
#
#-------------------------------------------------

QT       += core gui

TARGET = podcatcher_qt
TEMPLATE = app


SOURCES += .\src\main.cpp \
        .\src\podcatcherwin.cpp

HEADERS  += .\inc\podcatcherwin.h

FORMS    += .\forms\podcatcherwin.ui

INCLUDEPATH += ./inc
INCLUDEPATH += ./ui
INCLUDEPATH += ./moc

CONFIG += mobility
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe905eb0b
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

BLD_INF_RULES.//includes += "$${LITERAL_HASH}include \"../engine/group/bld.inf\""

LIBS += -lpodcastengine.dll
