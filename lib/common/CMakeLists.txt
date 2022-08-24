cmake_minimum_required(VERSION 3.22)
project(RCSOP_COMMON LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

find_package(Eigen3 3.4 REQUIRED)
find_package(CGAL REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)

include(GenerateExportHeader)
add_library(rcsop-common SHARED
        $<INSTALL_INTERFACE:include>
        src/colors.cpp
        src/model_camera.cpp
        src/image_point.cpp
        src/scored_point.cpp
        src/sparse_cloud.cpp
        src/dense_cloud.cpp
        src/chronometer.cpp
        src/observer.cpp
        src/scored_cloud.cpp
        src/logging.cpp
        src/texture.cpp
        src/gauss.cpp
        src/colmap_observer_camera.cpp
        src/rcs.cpp
        src/id_point.cpp
        src/base_point_cloud.cpp
        src/observer_position.cpp)

generate_export_header(rcsop-common)

target_include_directories(rcsop-common
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${COLMAP_INCLUDE_DIRS}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include)
target_link_directories(rcsop-common
        PUBLIC
            ${COLMAP_LINK_DIRS})

set_target_properties(rcsop-common PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")
target_link_libraries(rcsop-common
        PUBLIC
            ${COLMAP_LIBRARIES}
            Eigen3::Eigen
            CGAL::CGAL
)

install(TARGETS rcsop-common
        LIBRARY DESTINATION lib)

add_subdirectory(test)