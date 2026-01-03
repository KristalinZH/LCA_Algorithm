cmake_minimum_required(VERSION 3.28)
project(LCA)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

add_library(LCA INTERFACE)

target_include_directories(LCA
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(LCA INTERFACE cxx_std_11)

add_executable(LCATESTS
    tests/LCA_tests.cpp
)

target_link_libraries(LCATESTS
    LCA
    gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(LCATESTS)