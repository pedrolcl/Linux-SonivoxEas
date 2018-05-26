pkg_check_modules(DRUMSTICK REQUIRED drumstick-alsa)

link_directories(
  ${DRUMSTICK_LIBRARY_DIRS}
)

include_directories( 
  ${DRUMSTICK_INCLUDE_DIRS}
)

add_executable( cmdlnsynth main.cpp )

target_link_libraries( cmdlnsynth
  Qt5::Core
  svoxeas
  ${DRUMSTICK_LIBRARIES}
)


install( TARGETS cmdlnsynth 
	 DESTINATION bin )
