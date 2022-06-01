cmake_minimum_required(VERSION 3.16)
project(SFM_SPARSE_FILTER LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -Wall -Wpedantic")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
endif()

find_package(Eigen3 3.4 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(sfm-sparse-filter
        main.cpp
        cleanup.cpp)

target_include_directories(sfm-sparse-filter PRIVATE ${PROJECT_SOURCE_DIR}
        ${Boost_INCLUDE_DIR})
set_target_properties(sfm-sparse-filter PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS_RELEASE} -s")

target_link_libraries(sfm-sparse-filter PRIVATE
        ${Boost_LIBRARIES}
        sfm-coloring-common)

install(TARGETS sfm-sparse-filter sfm-coloring-common
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)