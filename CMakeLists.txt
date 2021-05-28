cmake_minimum_required (VERSION 3.2)
project(silicondb)

SET(CMAKE_CXX_STANDARD 17)

find_package (Threads)

add_executable (silicondb silicondb/main.cc)
target_include_directories(silicondb PUBLIC ./include)
target_link_libraries(silicondb Threads::Threads)
