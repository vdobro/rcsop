cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING_COMMON LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

find_package(Eigen3 3.4 REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS})
link_directories(${COLMAP_LINK_DIRS})

file(GLOB SRC_FILES *.cpp *.h utils/*.cpp utils/*.h)

include(GenerateExportHeader)
add_library(sfm-coloring-common SHARED ${SRC_FILES})
generate_export_header(sfm-coloring-common)

target_include_directories(sfm-coloring-common PUBLIC
        ${EIGEN3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR})

set_target_properties(sfm-coloring-common PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")
target_link_libraries(sfm-coloring-common PUBLIC ${COLMAP_LIBRARIES})