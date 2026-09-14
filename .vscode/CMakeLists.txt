cmake_minimum_required(VERSION 3.15)
project(my_pybind_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Look for Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Include workspace directory
target_include_directories(my_module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
