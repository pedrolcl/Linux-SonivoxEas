set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set( HEADERS
    programsettings.h
    synthcontroller.h
    synthrenderer.h
    filewrapper.h
)

set( SOURCES
    programsettings.cpp
    synthcontroller.cpp
    synthrenderer.cpp
    filewrapper.cpp
)

add_library( svoxeas SHARED ${HEADERS} ${SOURCES} )

set_target_properties( svoxeas PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_link_libraries( svoxeas PRIVATE
    sonivox::sonivox
    Qt${QT_VERSION_MAJOR}::Core
    Drumstick::ALSA
    PkgConfig::PULSE
)

target_include_directories( svoxeas PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions( svoxeas PRIVATE
    VERSION=${PROJECT_VERSION}
    $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

install( TARGETS svoxeas
         DESTINATION ${CMAKE_INSTALL_LIBDIR} )

install ( FILES ${HEADERS}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
