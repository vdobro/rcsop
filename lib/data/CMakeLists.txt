cmake_minimum_required(VERSION 3.16)
project(SFM_COLORING_DATA LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

include(FetchContent)
FetchContent_Declare(matio
        GIT_REPOSITORY https://github.com/tbeu/matio.git
        GIT_TAG f87216a4f33c658400f25f7306cba483ce3bac1c)
FetchContent_MakeAvailable(matio)

find_package(Eigen3 3.4 REQUIRED)
find_package(PCL 1.3 REQUIRED COMPONENTS common io)

include(GenerateExportHeader)
add_library(sfm-coloring-data SHARED
        $<INSTALL_INTERFACE:include>
        src/az_data.cpp
        src/rcs_data.cpp
        src/relative_points.cpp)
generate_export_header(sfm-coloring-data)

target_include_directories(sfm-coloring-data
        PUBLIC
            ${MATIO_PUBLIC_HEADERS}
            ${PROJECT_SOURCE_DIR}/include
            ${SFM_COLORING_COMMON_PUBLIC_INCLUDE_DIR}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(sfm-coloring-data
        PUBLIC
            matio
            sfm-coloring-common)
set_target_properties(sfm-coloring-data PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

install(TARGETS sfm-coloring-data
        LIBRARY DESTINATION lib)
