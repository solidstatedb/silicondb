cmake_minimum_required (VERSION 3.14)
project(silicondb)

SET(CMAKE_CXX_STANDARD 17)

find_package (Threads)

include_directories(./include)
link_libraries(Threads::Threads)

add_executable (silicondb silicondb/main.cc)

include(CTest)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()
link_libraries(gtest_main)

add_executable(map_test test/map.cc)
gtest_discover_tests(map_test)
