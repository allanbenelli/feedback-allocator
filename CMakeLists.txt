# Set the project name to the name of the directory the CMakeLists.txt file is in
get_filename_component(NAME "${CMAKE_SOURCE_DIR}" NAME)

project(${NAME})

cmake_minimum_required(VERSION 3.10.2)

# Default to Release build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 14)

# Find Boost library
find_package(Boost REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "This project requires the Boost library, and will not be compiled.")
endif()

# Add your source files here
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(${NAME} ${SOURCES})

# Include directories
target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# Compiler options
target_compile_options(${NAME} PRIVATE -pedantic -Wall -Wextra -Wpedantic)
target_compile_options(${NAME} PRIVATE $<$<CONFIG:Debug>:-Og -fsanitize=address -fsanitize=undefined>)

# Link libraries
target_link_libraries(${NAME} PRIVATE "$<$<CONFIG:Debug>:-Og -fsanitize=address -fsanitize=undefined>")
