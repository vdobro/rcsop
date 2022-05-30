cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING_RENDERING LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

find_package(Eigen3 3.4 REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS})

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

file(GLOB SRC_FILES *.cpp *.h)
add_library(sfm-coloring-rendering SHARED ${SRC_FILES})

target_include_directories(sfm-coloring-rendering PUBLIC
        ${EIGEN3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR})

target_link_libraries(sfm-coloring-rendering PRIVATE sfml-graphics)
set_target_properties(sfm-coloring-rendering PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")