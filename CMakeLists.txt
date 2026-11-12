cmake_minimum_required(VERSION 3.21)
project( sonivoxeas
         VERSION 1.7.1
         DESCRIPTION "Sonivox EAS for Linux and Qt" )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_QT5 "Choose Qt5 instead of Qt6. By default uses Qt6" OFF)
option(INSTALL_DEPENDENCIES "Deploy dependencies" OFF)

include(GNUInstallDirs)

if (USE_QT5)
    find_package(QT NAMES Qt5)
else()
    find_package(QT NAMES Qt6)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
find_package(Drumstick 2.11 COMPONENTS ALSA REQUIRED)
message(STATUS "Using Drumstick version: ${Drumstick_VERSION}")
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED IMPORTED_TARGET libpulse-simple)

set(sonivox_SHARED_LIBS ON) # set this OFF to use static sonivox
find_package(sonivox 4.0 CONFIG)
if (NOT sonivox_FOUND)
    message(FATAL_ERROR "Sonivox library not found")
    # alternative: use sonivox as a git submodule:
    add_subdirectory(sonivox EXCLUDE_FROM_ALL)
endif()
message(STATUS "Using Sonivox version: ${sonivox_VERSION}")
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)

if (INSTALL_DEPENDENCIES AND QT_VERSION VERSION_GREATER_EQUAL 6.5)
    qt_generate_deploy_app_script(
        TARGET guisynth
        OUTPUT_SCRIPT deploy_script
        INCLUDE_PLUGINS xcb wayland
        NO_UNSUPPORTED_PLATFORM_ERROR
        NO_TRANSLATIONS
    )
    install( SCRIPT ${deploy_script} )

    install( CODE
      "include(${CMAKE_SOURCE_DIR}/cmake/CreateAppImage.cmake)
       create_appimage(
            EXE \"guisynth\"
            ICON \"sonivoxeas.png\"
            DESKTOP \"sonivoxeas.desktop\"
            WORKDIR \"${CMAKE_BINARY_DIR}\"
            APPDIR \"Linux-SonivoxEas-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.AppDir\"
            ARCH \"${CMAKE_SYSTEM_PROCESSOR}\"
       )"
    )
endif()

if (DEFINED ENV{GITHUB_OUTPUT})
    file(APPEND
        "$ENV{GITHUB_OUTPUT}"
        "config_version=${PROJECT_VERSION}\n"
    )
endif()
