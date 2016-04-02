#---------------------------
#
# Sonivox EAS library for Qt
#
#---------------------------

QT += core
QT -= gui
TEMPLATE = lib
TARGET = svoxeas

DEPENDPATH += ../sonivox
INCLUDEPATH += ../sonivox/host_src

HEADERS += \
    synthcontroller.h \
    synthrenderer.h

SOURCES += \
    synthcontroller.cpp \
    synthrenderer.cpp

QMAKE_LFLAGS += -L../sonivox
LIBS += -lsonivox

CONFIG += link_pkgconfig
PKGCONFIG += drumstick-alsa \
   libpulse-simple \
   alsa
