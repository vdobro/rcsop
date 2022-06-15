cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING_RENDERING LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

include(GenerateExportHeader)
add_library(sfm-coloring-rendering SHARED
        src/render_points.cpp
        src/colors.cpp
        src/observer_renderer.cpp)
generate_export_header(sfm-coloring-data)

target_include_directories(sfm-coloring-rendering
        PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(sfm-coloring-rendering
        PUBLIC sfm-coloring-common
        PRIVATE sfml-graphics)
set_target_properties(sfm-coloring-rendering PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

install(TARGETS sfm-coloring-rendering
        LIBRARY DESTINATION lib)


