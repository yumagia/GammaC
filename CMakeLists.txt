cmake_minimum_required(VERSION 3.10.0)
project(GAMMAC VERSION 0.1.0 LANGUAGES C CXX)
include(CMakePrintHelpers)

include_directories(common)

add_executable(GAMMAC main.cpp)
target_include_directories(GAMMAC INTERFACE grak/gbsp)
target_include_directories(GAMMAC INTERFACE grak/grad)
target_include_directories(GAMMAC INTERFACE grak/gvis)


include(CTest)
enable_testing()

