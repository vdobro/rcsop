cmake_minimum_required(VERSION 3.16)
project(SFM_SPARSE_FILTER LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

find_package(Eigen3 3.4 REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(PCL 1.3 REQUIRED COMPONENTS common io segmentation reconstruction)

add_executable(sfm-dense-filter
        main.cpp
        filter-ground-plane.cpp
        filter-outliers.cpp)

target_include_directories(sfm-dense-filter PRIVATE ${Boost_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})
set_target_properties(sfm-dense-filter PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries (sfm-dense-filter PRIVATE
        Boost::program_options
        ${PCL_LIBRARIES})

include_directories(${PCL_INCLUDE_DIRS})
add_definitions(${PCL_DEFINITIONS})

install(TARGETS sfm-dense-filter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)