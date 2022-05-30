cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING_DATA LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

find_package(Eigen3 3.4 REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(matio
        GIT_REPOSITORY https://github.com/tbeu/matio.git
        GIT_TAG f87216a4f33c658400f25f7306cba483ce3bac1c)
FetchContent_MakeAvailable(matio)

file(GLOB SRC_FILES *.cpp *.h)

include(GenerateExportHeader)
add_library(sfm-coloring-data SHARED ${SRC_FILES})
generate_export_header(sfm-coloring-data)

target_include_directories(sfm-coloring-data PUBLIC
        ${PROJECT_BINARY_DIR}
        ${MATIO_PUBLIC_HEADERS}
        ${EIGEN3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR})

target_link_libraries(sfm-coloring-data PUBLIC matio)
set_target_properties(sfm-coloring-data PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

