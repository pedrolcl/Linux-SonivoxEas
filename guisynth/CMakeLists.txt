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

target_link_libraries( guisynth
  Qt5::Widgets
  Drumstick::ALSA
  svoxeas
)

target_compile_definitions( guisynth PRIVATE
    VERSION=${PROJECT_VERSION}
    $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

install( TARGETS guisynth 
         DESTINATION ${CMAKE_INSTALL_BINDIR} )
