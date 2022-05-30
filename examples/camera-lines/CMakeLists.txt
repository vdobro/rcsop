cmake_minimum_required(VERSION 3.16)
project(SFM_SPARSE_FILTER LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

add_executable(sfm-camera-lines
        main.cpp
        draw_lines.cpp)

target_include_directories(sfm-camera-lines PRIVATE ${PROJECT_SOURCE_DIR})
set_target_properties(sfm-camera-lines PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(sfm-camera-lines PRIVATE
        sfm-coloring-common
        sfm-coloring-data
        sfm-coloring-rendering
)
