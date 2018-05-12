#------------------------
#
# Sonivox EAS Synthesizer
#
#------------------------

QT       += core
QT       -= gui
TARGET   = cmdlnsynth
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app

DEPENDPATH += ../libsvoxeas
INCLUDEPATH += ../libsvoxeas \
               ../sonivox/host_src

SOURCES += main.cpp

QMAKE_LFLAGS += -L../libsvoxeas
LIBS += -lsvoxeas

QMAKE_RPATHDIR = $$OUT_PWD/../libsvoxeas

CONFIG += link_pkgconfig
PKGCONFIG += drumstick-alsa
