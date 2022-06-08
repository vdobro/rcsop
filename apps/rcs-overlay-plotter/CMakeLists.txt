cmake_minimum_required(VERSION 3.16)
project(RCS_OVERLAY_PLOTTER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(rcs-overlay-plotter
        main.cpp
        input_data_collector.cpp
        test_task.cpp)

target_include_directories(rcs-overlay-plotter PRIVATE ${PROJECT_SOURCE_DIR})
set_target_properties(rcs-overlay-plotter PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(rcs-overlay-plotter PRIVATE
        sfm-coloring-common
        sfm-coloring-data
        sfm-coloring-rendering
        Boost::program_options)
