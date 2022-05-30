cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

add_executable(sfm-rcs-sums
        main.cpp
        rcs_sums.cpp)

target_include_directories(sfm-rcs-sums PRIVATE ${PROJECT_SOURCE_DIR})
set_target_properties(sfm-rcs-sums PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(sfm-rcs-sums PRIVATE
        sfm-coloring-common
        sfm-coloring-data
        sfm-coloring-rendering
)
