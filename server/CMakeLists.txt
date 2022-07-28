cmake_minimum_required(VERSION 3.10)
project(ntechlab_testtask)
set(CMAKE_CXX_STANDARD 20)

set(BEAST_INCLUDE_DIR ../third_party/boost/)

include_directories(${BEAST_INCLUDE_DIR})

add_executable(ntechlab_testtask main.cpp)

target_link_libraries(ntechlab_testtask pthread)
