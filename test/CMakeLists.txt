find_package(Python3 COMPONENTS Interpreter)
find_package(PythonLibs 3 REQUIRED)

execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/terraform.py"
    "-m" "terraform"
    "-b" "${EFG_PYTHON_SO_LOCATION}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(commands)
