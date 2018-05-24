set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets REQUIRED)
pkg_check_modules(DRUMSTICK REQUIRED drumstick-alsa)

link_directories(
  ${DRUMSTICK_LIBRARY_DIRS}
)

include_directories(
  ${DRUMSTICK_INCLUDE_DIRS}
)

set( SOURCES 
  main.cpp
  mainwindow.cpp
  programsettings.cpp
)

set( HEADERS mainwindow.h programsettings.h )
set( FORMS mainwindow.ui )
set( RESOURCES guisynth.qrc )

qt5_wrap_cpp ( MOC_SRCS ${HEADERS} )
qt5_wrap_ui ( UI_SRCS ${FORMS} )
qt5_add_resources ( RES_SRCS ${RESOURCES} )

add_executable( guisynth
  ${SOURCES}
  ${MOC_SRCS}
  ${UI_SRCS} 
  ${RES_SRCS}
)

target_link_libraries( guisynth
  Qt5::Widgets
  svoxeas
  ${DRUMSTICK_LIBRARIES}
)

install( TARGETS guisynth 
	 DESTINATION bin )
