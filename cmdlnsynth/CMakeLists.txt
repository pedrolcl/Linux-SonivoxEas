add_executable( cmdlnsynth main.cpp )

target_link_libraries( cmdlnsynth
    Qt5::Core
    Drumstick::ALSA
    svoxeas
)

target_compile_definitions( cmdlnsynth PRIVATE
    VERSION=${PROJECT_VERSION}
    $<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

install( TARGETS cmdlnsynth 
         DESTINATION ${CMAKE_INSTALL_BINDIR} )
