cmake_minimum_required(VERSION 3.16)
project(sfm_colors LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(COLMAP 3.8 CONFIG REQUIRED)
include_directories(${COLMAP_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
link_directories(${COLMAP_LINK_DIRS})

#find_package(ROOT 6.26 CONFIG REQUIRED)

add_executable(sfm_colors main.cpp
        utils.cpp
        cleanup.cpp
        slices.cpp)

target_include_directories(sfm_colors PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sfm_colors PRIVATE ${COLMAP_LIBRARIES})

