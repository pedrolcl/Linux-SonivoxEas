set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets REQUIRED)

set( SOURCES 
  main.cpp
  mainwindow.cpp
)

set( HEADERS mainwindow.h )
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
)

install( TARGETS guisynth 
	 DESTINATION bin )
