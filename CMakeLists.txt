cmake_minimum_required(VERSION 3.15)

project(SaveStuff LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_program(XXD_EXECUTABLE xxd REQUIRED)

set(GENERAL_HELP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles/general.txt")
set(GENERAL_HELP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/generalhelp.h")

add_custom_command(
        OUTPUT "${GENERAL_HELP_HEADER}"
        COMMAND ${XXD_EXECUTABLE} -i "general.txt" "${GENERAL_HELP_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles"
        DEPENDS "${GENERAL_HELP_FILE}"
        COMMENT "Generating ${GNEREAL_HELP_HEADER} using xxd..."
)

add_executable(savestuff
        src/main.c
        "${GENERAL_HELP_HEADER}"
)

if(WIN32)
    message(STATUS "Windows detected. Adding third-party dirent compatibility layer.")
    target_include_directories(savestuff PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/compat)
endif()

target_include_directories(savestuff PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)