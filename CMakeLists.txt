cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(EFG-python)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

option(BUILD_PYTHON_TEST "" OFF)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(PythonLibs 3 REQUIRED)

add_subdirectory(src)

if(BUILD_PYTHON_TEST)
add_subdirectory(test)
endif()
