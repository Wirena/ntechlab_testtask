cmake_minimum_required(VERSION 3.10)
project(ntechlab_testtask)
set(CMAKE_CXX_STANDARD 20)

set(BOOST_INCLUDE_DIR ../third_party/boost/)
set(LIBJPEG_INCLUDE_DIR ../third_party/libjpeg/include)
set(LIBJPEG_STATIC_LIB_REL ../third_party/libjpeg/libjpeg.a)
get_filename_component(LIBJPEG_STATIC_LIB_ABS ${LIBJPEG_STATIC_LIB_REL} ABSOLUTE)

add_executable(${PROJECT_NAME} main.cpp TcpListener.hpp fail.h HTTPConnection.cpp HTTPConnection.h HTTPServer.cpp HTTPServer.h Responses.cpp Responses.h JPEGBase.cpp JPEGBase.h JPEGReader.cpp JPEGReader.h JPEGWriter.cpp JPEGWriter.h)

add_compile_definitions(BOOST_NO_EXCEPTIONS)

#target_compile_options(${PROJECT_NAME} PUBLIC -fno-exceptions )

target_link_libraries(${PROJECT_NAME} ${LIBJPEG_STATIC_LIB_ABS} pthread)

include_directories( ${BOOST_INCLUDE_DIR} ${LIBJPEG_INCLUDE_DIR} )