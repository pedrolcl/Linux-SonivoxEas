set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set( SOURCES main.cpp mainwindow.cpp )
set( HEADERS mainwindow.h )
set( FORMS mainwindow.ui )
set( RESOURCES guisynth.qrc )

add_executable( guisynth
  ${SOURCES}
  ${HEADERS}
  ${FORMS}
  ${RESOURCES}
)

get_target_property( SONIVOX_HEADERS sonivox::sonivox INTERFACE_INCLUDE_DIRECTORIES )

target_include_directories( guisynth PRIVATE ${SONIVOX_HEADERS} )

target_link_libraries( guisynth
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Widgets
  Drumstick::ALSA
  svoxeas
)

target_compile_definitions( guisynth PRIVATE
    VERSION=${PROJECT_VERSION}
    $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

install( TARGETS guisynth 
         DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
         DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
         RENAME sonivoxeas.png )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/icon256.png
         DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
         RENAME sonivoxeas.png )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/icon512.png
         DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
         RENAME sonivoxeas.png )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/guisynth.desktop
         DESTINATION ${CMAKE_INSTALL_DATADIR}/applications 
         RENAME sonivoxeas.desktop )
