cmake_minimum_required(VERSION 3.16)
project(sfm_colors LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Wall -Wpedantic -Werror")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

add_subdirectory(deps)

find_package(Eigen3 3.4 REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

link_directories(${COLMAP_LINK_DIRS})

add_executable(sfm_colors
        main.cpp
        utils.cpp
        cleanup.cpp
        slices.cpp
        scored_point.cpp
        mat_reader.cpp
        colors.cpp
        draw_lines.cpp
        rcs_sums.cpp
        render_points.cpp)

target_include_directories(sfm_colors PRIVATE
        ${COLMAP_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR})

set_target_properties(sfm_colors PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(sfm_colors PRIVATE
        ${COLMAP_LIBRARIES}
        matio
        sfml-graphics)
