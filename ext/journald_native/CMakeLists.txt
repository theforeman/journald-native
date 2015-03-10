# dummy cmake file for CLion

cmake_minimum_required(VERSION 2.8.4)
project(journald_native)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES extinit.cpp journald_native.cpp ruby_exception_wrapper.cpp)
add_executable(journald_native ${SOURCE_FILES})
