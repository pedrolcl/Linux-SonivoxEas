pkg_check_modules(AO REQUIRED ao)
pkg_check_modules(DRUMSTICK REQUIRED drumstick-alsa)

link_directories(
    ${AO_LIBRARY_DIRS}
    ${DRUMSTICK_LIBRARY_DIRS}
)

set( HEADERS
    synthcontroller.h
    synthrenderer.h
    filewrapper.h
)

set( SOURCES
    synthcontroller.cpp 
    synthrenderer.cpp
    filewrapper.cpp
)

qt5_wrap_cpp( MOC_SRCS ${HEADERS} )

add_library( svoxeas SHARED ${MOC_SRCS} ${SOURCES} )

set_target_properties( svoxeas PROPERTIES 
    VERSION 1.1.0 
    SOVERSION 1
)

target_link_libraries( svoxeas 
    sonivox 
    Qt5::Core 
    ${AO_LIBRARIES}
    ${DRUMSTICK_LIBRARIES}
)

target_include_directories( svoxeas
    PUBLIC 
      ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE 
      ${AO_INCLUDE_DIRS}
      ${DRUMSTICK_INCLUDE_DIRS}
)

install( 
    TARGETS svoxeas 
    DESTINATION lib${LIB_SUFFIX} 
)
