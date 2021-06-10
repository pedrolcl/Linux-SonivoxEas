cmake_minimum_required(VERSION 3.9)
project( EASSynth
         VERSION 1.3.0
         DESCRIPTION "Sonivox EAS for Linux and Qt" )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.0 COMPONENTS ALSA REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED IMPORTED_TARGET libpulse-simple)

include(GNUInstallDirs)

add_subdirectory(sonivox)
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)
