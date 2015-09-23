# Require a recent version of cmake
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

# This project is C++ based.
project(marching-cubes CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# Instruct cmake to find the HPX settings
find_package(HPX)

# build component
add_hpx_component(block
    SOURCES block.cpp
    HEADERS block.hpp)

# build your application using HPX
add_hpx_executable( MarchingCubes
    ESSENTIAL
    HEADERS table.h
    SOURCES main.cpp table.cpp
    COMPONENT_DEPENDENCIES block_component iostreams)

