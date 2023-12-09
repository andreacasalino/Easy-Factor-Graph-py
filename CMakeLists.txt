cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(EFG-python)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

option(BUILD_PYTHON_TEST "" OFF)

if(DEFINED PYTHON_EXECUTABLE)
    # indeed, this env var is defined by the setup.py script
    # if here the package is being built in order to generate a wheel
    set(PYTHON_CMD ${PYTHON_EXECUTABLE})
else()
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    find_package(PythonLibs 3 REQUIRED)
    set(PYTHON_CMD "${Python3_EXECUTABLE}")
endif()

add_subdirectory(src)

if(BUILD_PYTHON_TEST)
add_subdirectory(test)
endif()
