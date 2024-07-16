cmake_minimum_required(VERSION 3.24)
project(bmf VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
add_compile_options(-Wall -Wextra -Wpedantic)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
add_executable(bmf
    AbstractResultsFileLoader.cpp AbstractResultsFileLoader.h
    BMFFileLoader.cpp BMFFileLoader.h
    Mesh.cpp Mesh.h
    NodalFields.cpp NodalFields.h
    SMRFileLoader.cpp SMRFileLoader.h
    main.cpp
)
