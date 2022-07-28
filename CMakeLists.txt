cmake_minimum_required(VERSION 3.9)
project( EASSynth
         VERSION 1.4.0
         DESCRIPTION "Sonivox EAS for Linux and Qt" )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_QT "Choose which Qt major version (5 or 6) to prefer. By default uses whatever is found")

include(GNUInstallDirs)

if (USE_QT)
    if (NOT (USE_QT EQUAL 5 OR USE_QT EQUAL 6))
        message(FATAL_ERROR "Wrong Qt major version. Only 5 and 6 are valid options")
    endif()
endif()

if (USE_QT EQUAL 5)
    find_package(QT NAMES Qt5)
elseif (USE_QT EQUAL 6)
    find_package(QT NAMES Qt6)
else()
    find_package(QT NAMES Qt5 Qt6)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.0 COMPONENTS ALSA REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED IMPORTED_TARGET libpulse-simple)

find_package(sonivox CONFIG)
if (NOT sonivox_FOUND)
    add_subdirectory(sonivox)
endif()
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)
