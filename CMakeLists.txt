cmake_minimum_required(VERSION 3.0)
project(eassynth)

find_package(Qt5Core REQUIRED)
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")

add_subdirectory(sonivox)
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)
