cmake_minimum_required(VERSION 3.16)
project(RCS_OVERLAY_PLOTTER LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(COLMAP REQUIRED)

add_executable(rcs-overlay-plotter
        main.cpp
        launcher.cpp
        utils/point_scoring.cpp
        utils/task_utils.cpp
        tasks/test_task.cpp
        tasks/azimuth_rcs_plotter.cpp
        tasks/rcs_slices.cpp
        tasks/rcs_sums.cpp
        tasks/sparse_filter.cpp)

target_include_directories(rcs-overlay-plotter PRIVATE ${PROJECT_SOURCE_DIR})
set_target_properties(rcs-overlay-plotter PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(rcs-overlay-plotter PRIVATE
        rcsop-common
        rcsop-data
        rcsop-rendering
        Boost::program_options)

include(GNUInstallDirs)
install(TARGETS rcs-overlay-plotter RUNTIME)
install(IMPORTED_RUNTIME_ARTIFACTS rcs-overlay-plotter)
