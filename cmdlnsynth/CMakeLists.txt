add_executable( cmdlnsynth main.cpp )

target_link_libraries( cmdlnsynth
  Qt5::Core
  svoxeas
)

install( TARGETS cmdlnsynth 
	 DESTINATION bin )
