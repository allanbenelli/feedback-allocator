# Set the project name to the name of the directory the CMakeLists.txt file is in
get_filename_component(NAME "${CMAKE_SOURCE_DIR}" NAME)

project(${NAME})

cmake_minimum_required(VERSION 3.10.2)

# Default to Release build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 14)

find_package(CGAL QUIET COMPONENTS Core)

if (NOT CGAL_FOUND)
    message(FATAL_ERROR "This program requires the CGAL library, and will not be compiled.")
endif()

find_package(Boost REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "This project requires the Boost library, and will not be compiled.")
endif()

set(CGAL_CURRENT_SOURCE_DIR "src/")
create_single_source_cgal_program("src/main.cpp")
target_include_directories("main" PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_options("main" PRIVATE -pedantic -Wall -Wextra -Wpedantic)
target_compile_options("main" PRIVATE $<$<CONFIG:Debug>:-Og -fsanitize=address -fsanitize=undefined>)
target_link_libraries("main" PRIVATE "$<$<CONFIG:Debug>:-Og -fsanitize=address -fsanitize=undefined>")
