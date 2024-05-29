cmake_minimum_required(VERSION 3.24)
project(current_lines)

set(CMAKE_CXX_STANDARD 23)

add_executable(current_lines main.cpp
        Mesh.h Mesh.cpp)