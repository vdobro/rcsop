cmake_minimum_required(VERSION 3.22)
project(RCSOP_COMMON LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        rcsop-common-test

        hello_test.cc
        observer_test.cc)

target_link_libraries(
        rcsop-common-test
        PUBLIC rcsop-common GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(rcsop-common-test)
