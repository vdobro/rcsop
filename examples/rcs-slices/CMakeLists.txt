cmake_minimum_required(VERSION 3.16)
project(SFM_SPARSE_FILTER LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(Eigen3 3.4 REQUIRED)

find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS})
link_directories(${COLMAP_LINK_DIRS})

file(GLOB SRC_FILES *.cpp)
add_executable(sfm-rcs-slices ${SRC_FILES})

target_include_directories(sfm-rcs-slices PRIVATE
        ${EIGEN3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR})

set_target_properties(sfm-rcs-slices PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")
target_link_libraries(sfm-rcs-slices PRIVATE
        sfm-coloring-common
        sfm-coloring-data
        sfm-coloring-rendering
)
