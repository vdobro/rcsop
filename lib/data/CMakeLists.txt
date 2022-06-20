cmake_minimum_required(VERSION 3.16)
project(RCSOP_DATA LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

include(FetchContent)
FetchContent_Declare(matio
        GIT_REPOSITORY https://github.com/tbeu/matio.git
        GIT_TAG f87216a4f33c658400f25f7306cba483ce3bac1c)
FetchContent_MakeAvailable(matio)

find_package(Eigen3 3.4 REQUIRED)

include(GenerateExportHeader)
add_library(rcsop-data SHARED
        $<INSTALL_INTERFACE:include>
        src/az_data.cpp
        src/basic_rcs_map.cpp
        src/input_data_collector.cpp
        src/input_image.cpp
        src/point_cloud_provider.cpp
        src/azimuth_rcs_data_collection.cpp
        src/observer_provider.cpp
        src/utils/rcs_data_utils.cpp
        src/azimuth_minimap_provider.cpp)

generate_export_header(rcsop-data)

target_include_directories(rcsop-data
        PUBLIC
            ${MATIO_PUBLIC_HEADERS}
            ${PROJECT_SOURCE_DIR}/include
            ${RCSOP_COMMON_PUBLIC_INCLUDE_DIR}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(rcsop-data
        PUBLIC
            matio
            rcsop-common)
set_target_properties(rcsop-data PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

install(TARGETS rcsop-data
        LIBRARY DESTINATION lib)
