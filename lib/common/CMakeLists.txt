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

include(GenerateExportHeader)
add_library(sfm-coloring-common SHARED
        $<INSTALL_INTERFACE:include>
        src/camera.cpp
        src/image_point.cpp
        src/scored_point.cpp
        src/sparse_cloud.cpp
        src/chronometer.cpp)

generate_export_header(sfm-coloring-common)

target_include_directories(sfm-coloring-common
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${COLMAP_INCLUDE_DIRS}
            ${EIGEN3_INCLUDE_DIR}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include)

target_link_directories(sfm-coloring-common
        PUBLIC
            ${COLMAP_LINK_DIRS})

set_target_properties(sfm-coloring-common PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")
target_link_libraries(sfm-coloring-common
        PUBLIC
            ${COLMAP_LIBRARIES}
)

install(TARGETS sfm-coloring-common
        LIBRARY DESTINATION lib)
