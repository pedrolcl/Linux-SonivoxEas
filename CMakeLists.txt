cmake_minimum_required(VERSION 3.9)
project( EASSynth
         VERSION 1.7.0
         DESCRIPTION "Sonivox EAS for Linux and Qt" )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_QT5 "Choose Qt5 instead of Qt6. By default uses Qt6")

include(GNUInstallDirs)

if (USE_QT5)
    find_package(QT NAMES Qt5)
else()
    find_package(QT NAMES Qt6)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.10 COMPONENTS ALSA REQUIRED)
message(STATUS "Using Drumstick version: ${Drumstick_VERSION}")
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED IMPORTED_TARGET libpulse-simple)

set(sonivox_SHARED_LIBS ON)
find_package(sonivox 4.0 CONFIG REQUIRED)
if (NOT sonivox_FOUND)
    message(FATAL_ERROR "Sonivox library not found")
    #alternative: use sonivox as a git submodule:
    #add_subdirectory(sonivox)
endif()
message(STATUS "Using Sonivox version: ${sonivox_VERSION}")
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)
