cmake_minimum_required(VERSION 3.0)
project(eassynth)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(Qt5Core REQUIRED)
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG_OUTPUT")

add_subdirectory(sonivox)
add_subdirectory(libsvoxeas)
add_subdirectory(cmdlnsynth)
add_subdirectory(guisynth)
