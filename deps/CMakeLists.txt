set(CMAKE_BUILD_TYPE Release)

add_subdirectory(matio)
target_include_directories(matio PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/matio/src
        ${CMAKE_CURRENT_BINARY_DIR}/deps/matio/src)
