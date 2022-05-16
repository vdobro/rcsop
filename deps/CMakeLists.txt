set(CMAKE_BUILD_TYPE Release)

include(FetchContent)
FetchContent_Declare(matio
        GIT_REPOSITORY https://github.com/tbeu/matio.git
        GIT_TAG f87216a4f33c658400f25f7306cba483ce3bac1c)
FetchContent_MakeAvailable(matio)
