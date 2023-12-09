function(FETCH_DEP LIB_NAME TAG URL)
    include(FetchContent)
    message(STATUS "fetching: ${LIB_NAME}")
    FetchContent_Declare(
        ${LIB_NAME}
        GIT_REPOSITORY ${URL}
        GIT_TAG        ${TAG}
    )
    FetchContent_MakeAvailable(${LIB_NAME})
endfunction()

execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/terraform.py"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(external)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(WRAPPER_NAME efg)
pybind11_add_module(${WRAPPER_NAME} ${SRC_FILES})
target_compile_definitions(${WRAPPER_NAME} PRIVATE
-D VERSION_INFO="${VERSION_INFO}"
)
target_include_directories(${WRAPPER_NAME} 
PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/header
PRIVATE 
    ${PYBIND11_INCLUDE_DIRS}
)
target_link_libraries(${WRAPPER_NAME} PRIVATE EFG)

if(BUILD_PYTHON_TEST)
    get_target_property(TMP_VAR ${WRAPPER_NAME} SUFFIX)
    SET(EFG_PYTHON_SO_LOCATION  "${CMAKE_CURRENT_BINARY_DIR}/${WRAPPER_NAME}${TMP_VAR}" CACHE INTERNAL "EFG_PYTHON_LIB_LOCATION")
endif()
