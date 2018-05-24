#------------------------
#
# Sonivox EAS Synthesizer
#
#------------------------
include(../global.pri)

QT       += core
QT       -= gui
TARGET   = cmdlnsynth
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

DEPENDPATH += ../libsvoxeas
INCLUDEPATH += ../libsvoxeas \
               ../sonivox/host_src
SOURCES += main.cpp programsettings.cpp
HEADERS += programsettings.h
QMAKE_LFLAGS += -L../libsvoxeas
LIBS += -lsvoxeas
QMAKE_RPATHDIR = $$OUT_PWD/../libsvoxeas
CONFIG += link_pkgconfig
PKGCONFIG += drumstick-alsa
