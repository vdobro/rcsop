cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(RCSOP LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

add_subdirectory(lib)
add_subdirectory(launcher)