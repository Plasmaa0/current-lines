cmake_minimum_required(VERSION 3.24)
project(current_lines C CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})


add_library(current_lines-utils SHARED
        src/Utils/Color.h
        src/Utils/Nimpl.h
        src/Utils/Nimpl.cpp)
target_include_directories(current_lines-utils PRIVATE src)


add_library(current_lines-geometry SHARED
        src/Geometry/Coords.h
        src/Geometry/Coords.cpp
        src/Geometry/CrossPoints.h
        src/Geometry/CrossPoints.cpp
        src/Geometry/Line.h
        src/Geometry/Line.cpp
        src/Geometry/Node.h
        src/Geometry/Node.cpp
        src/Geometry/VectorField.h
        src/Geometry/VectorField.cpp
        src/Geometry/Geometry.h
        src/Geometry/Element.cpp
        src/Geometry/Element.h)
target_include_directories(current_lines-geometry PRIVATE src)


add_library(current_lines-mesh SHARED
        src/Mesh/Mesh.cpp
        src/Mesh/Mesh.h)
target_include_directories(current_lines-mesh PRIVATE src)


add_library(current_lines-generator SHARED
        src/CurrentLineGenerator/CurrentLineGenerator.h
        src/CurrentLineGenerator/CurrentLineGenerator.cpp
        src/CurrentLineGenerator/Interpolation.h
        src/CurrentLineGenerator/Interpolation.cpp)
target_include_directories(current_lines-generator PRIVATE src)


add_executable(current_lines src/main.cpp)
target_include_directories(current_lines PUBLIC src)

target_link_libraries(current_lines current_lines-generator current_lines-geometry current_lines-mesh current_lines-utils)