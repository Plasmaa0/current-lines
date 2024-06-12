cmake_minimum_required(VERSION 3.24)
project(current_lines C CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
add_compile_options(-Wall -Wextra -Wpedantic)

add_library(current_lines-utils STATIC
        src/Utils/Color.h
        src/Utils/Nimpl.h
        src/Utils/Nimpl.cpp
        src/Utils/Logger.h
        src/Utils/Logger.cpp
        src/Utils/Formatter.h)
target_include_directories(current_lines-utils PRIVATE src)


add_library(current_lines-geometry STATIC
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
        src/Geometry/BoundingBox.h
        src/Geometry/Plane.cpp
        src/Geometry/Plane.h)
target_include_directories(current_lines-geometry PRIVATE src)


add_library(current_lines-element STATIC
        src/Element/Common/Element.cpp
        src/Element/Common/Element.h
        src/Element/Common/ElementFactory.cpp
        src/Element/Common/ElementFactory.h

        src/Element/2D/Quadrangle.cpp
        src/Element/2D/Quadrangle.h
        src/Element/2D/Element2D.cpp
        src/Element/2D/Element2D.h
        src/Element/2D/Triangle.cpp
        src/Element/2D/Triangle.h

        src/Element/3D/Tetrahedron.cpp
        src/Element/3D/Tetrahedron.h
        src/Element/3D/Hexahedron.cpp
        src/Element/3D/Hexahedron.h
)
target_link_libraries(current_lines-element PRIVATE current_lines-geometry current_lines-utils)
target_include_directories(current_lines-element PRIVATE src)


add_library(current_lines-mesh STATIC
        src/Mesh/Mesh.cpp
        src/Mesh/Mesh.h)
target_link_libraries(current_lines-mesh PRIVATE current_lines-element current_lines-geometry current_lines-utils)
target_include_directories(current_lines-mesh PRIVATE src)


add_library(current_lines-generator STATIC
        src/CurrentLineGenerator/CurrentLineGenerator.h
        src/CurrentLineGenerator/CurrentLineGenerator.cpp
        src/CurrentLineGenerator/CurrentLine.cpp
        src/CurrentLineGenerator/CurrentLine.h
)
target_link_libraries(current_lines-generator PRIVATE current_lines-mesh current_lines-element current_lines-geometry current_lines-utils)
target_include_directories(current_lines-generator PRIVATE src)


add_executable(current_lines src/main.cpp)
target_include_directories(current_lines PRIVATE src)

target_link_libraries(current_lines
        current_lines-generator
        current_lines-mesh)