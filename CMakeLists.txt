cmake_minimum_required(VERSION 3.24)
project(current_lines)

set(CMAKE_CXX_STANDARD 23)

add_executable(current_lines Cell.h Coords.h CurrentLineGenerator.h main.cpp Mesh.h Utils.h
Color.h CrossPoints.h Line.h Mesh.cpp Node.h VectorField.h Interpolation.h)
