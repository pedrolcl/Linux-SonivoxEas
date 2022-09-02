add_executable( cmdlnsynth main.cpp )

get_target_property( SONIVOX_HEADERS sonivox::sonivox INTERFACE_INCLUDE_DIRECTORIES )

target_include_directories( cmdlnsynth PRIVATE ${SONIVOX_HEADERS} )

target_link_libraries( cmdlnsynth
    Qt${QT_VERSION_MAJOR}::Core
    Drumstick::ALSA
    svoxeas
)

target_compile_definitions( cmdlnsynth PRIVATE
    VERSION=${PROJECT_VERSION}
    $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

install( TARGETS cmdlnsynth 
         DESTINATION ${CMAKE_INSTALL_BINDIR} )
