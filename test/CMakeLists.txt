execute_process(
    COMMAND ${PYTHON_CMD} "${CMAKE_CURRENT_SOURCE_DIR}/terraform.py"
    "-m" "terraform"
    "-b" "${EFG_PYTHON_SO_LOCATION}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(commands)
