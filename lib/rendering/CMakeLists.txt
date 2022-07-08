cmake_minimum_required(VERSION 3.16)
project(RCSOP_RENDERING LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

include(GenerateExportHeader)
add_library(rcsop-rendering SHARED
        src/colors.cpp
        src/observer_renderer.cpp
        src/sfml_renderer.cpp)
generate_export_header(rcsop-data)

target_include_directories(rcsop-rendering
        PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(rcsop-rendering
        PUBLIC rcsop-common
        PRIVATE sfml-graphics)
set_target_properties(rcsop-rendering PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

install(TARGETS rcsop-rendering
        LIBRARY DESTINATION lib)


