#---------------------------
#
# Sonivox EAS library for Qt
#
#---------------------------

QT += core
QT -= gui
TEMPLATE = lib
TARGET = svoxeas
CONFIG(debug, debug|release) {
    message(DEBUG)
} else {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

DEPENDPATH += ../sonivox
INCLUDEPATH += ../sonivox/host_src

HEADERS += \
    synthcontroller.h \
    synthrenderer.h \
    filewrapper.h

SOURCES += \
    synthcontroller.cpp \
    synthrenderer.cpp \
    filewrapper.cpp

QMAKE_LFLAGS += -L../sonivox
LIBS += -lsonivox

CONFIG += link_pkgconfig
PKGCONFIG += drumstick-alsa \
   ao \
   alsa
