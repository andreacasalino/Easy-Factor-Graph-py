function(FETCH_DEP LIB_NAME TAG URL)
    include(FetchContent)
    message(STATUS "fetching: ${LIB_NAME} , version ${TAG}")
    FetchContent_Declare(
        ${LIB_NAME}
        GIT_REPOSITORY ${URL}
        GIT_TAG        ${TAG}
    )
    FetchContent_MakeAvailable(${LIB_NAME})
endfunction()

if (WIN32)
	set(LIB_OPT OFF CACHE BOOL "" FORCE)
else()
	set(LIB_OPT ON CACHE BOOL "" FORCE)
endif (WIN32)

execute_process(
    COMMAND ${PYTHON_CMD} "${CMAKE_CURRENT_SOURCE_DIR}/terraform.py"
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
target_link_libraries(${WRAPPER_NAME} PRIVATE EFG-Core)

if(BUILD_PYTHON_TEST)
    get_target_property(TMP_VAR ${WRAPPER_NAME} SUFFIX)
    SET(EFG_PYTHON_SO_LOCATION  "${CMAKE_CURRENT_BINARY_DIR}/${WRAPPER_NAME}${TMP_VAR}" CACHE INTERNAL "EFG_PYTHON_LIB_LOCATION")
endif()
