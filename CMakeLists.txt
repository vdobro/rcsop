cmake_minimum_required(VERSION 3.16)
project(sfm_colors LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wpedantic -Werror")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Eigen3 3.4 REQUIRED)
find_package(MATIO REQUIRED)
find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${MATIO_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
link_directories(${COLMAP_LINK_DIRS} ${MATIO_LIBRARIES})

add_executable(sfm_colors main.cpp
        utils.cpp
        cleanup.cpp
        slices.cpp
        matlab/read_rcs.cpp)

target_include_directories(sfm_colors PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/matlab)

target_link_libraries(sfm_colors PRIVATE ${COLMAP_LIBRARIES} ${MATIO_LIBRARIES})

