pkg_check_modules(DRUMSTICK REQUIRED drumstick-alsa)

link_directories(
  ${DRUMSTICK_LIBRARY_DIRS}
)

include_directories( 
  ${DRUMSTICK_INCLUDE_DIRS}
)

qt5_wrap_cpp ( MOC_SRCS programsettings.h )

add_executable( cmdlnsynth main.cpp programsettings.cpp ${MOC_SRCS} )

target_link_libraries( cmdlnsynth
  Qt5::Core
  svoxeas
  ${DRUMSTICK_LIBRARIES}
)


install( TARGETS cmdlnsynth 
	 DESTINATION bin )
