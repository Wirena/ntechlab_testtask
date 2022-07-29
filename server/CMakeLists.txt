cmake_minimum_required(VERSION 3.10)
project(ntechlab_testtask)
set(CMAKE_CXX_STANDARD 20)

set(BOOST_INCLUDE_DIR ../third_party/boost/)

add_executable(${PROJECT_NAME} main.cpp TcpListner.hpp fail.h HTTPConnection.cpp HTTPConnection.h HTTPServer.cpp HTTPServer.h)

add_compile_definitions(BOOST_NO_EXCEPTIONS)

#target_compile_options(${PROJECT_NAME} PUBLIC -fno-exceptions )

target_link_libraries(${PROJECT_NAME} pthread)

include_directories( ${BOOST_INCLUDE_DIR})