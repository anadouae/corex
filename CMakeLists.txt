cmake_minimum_required(VERSION 3.20)
project(corex VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(COREX_BUILD_TESTS "Build tests" ON)

find_package(Threads REQUIRED)

add_library(corex
  src/bounded_queue.cpp
  src/worker_pool.cpp
  src/engine.cpp
)
target_include_directories(corex PUBLIC include)
target_link_libraries(corex PUBLIC Threads::Threads)

add_executable(corex_bench benchmarks/bench_main.cpp)
target_link_libraries(corex_bench PRIVATE corex Threads::Threads)

if(COREX_BUILD_TESTS)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  add_executable(corex_tests tests/queue_tests.cpp tests/engine_tests.cpp)
  target_link_libraries(corex_tests PRIVATE corex GTest::gtest_main Threads::Threads)
  include(GoogleTest)
  gtest_discover_tests(corex_tests)
endif()
