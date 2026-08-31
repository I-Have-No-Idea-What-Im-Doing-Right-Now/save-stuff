cmake_minimum_required(VERSION 3.15)

project(SaveStuff LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_program(XXD_EXECUTABLE xxd REQUIRED)

set(GENERAL_HELP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles/general.txt")
set(GENERAL_HELP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/generalhelp.h")
set(VERSION_HELP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles/version.txt")
set(VERSION_HELP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/versionhelp.h")
set(BACKUP_HELP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles/backup.txt")
set(BACKUP_HELP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/backuphelp.h")
set(RESTORE_HELP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles/restore.txt")
set(RESTORE_HELP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/restorehelp.h")

add_custom_command(
        OUTPUT "${GENERAL_HELP_HEADER}"
        COMMAND ${XXD_EXECUTABLE} -i "general.txt" "${GENERAL_HELP_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles"
        DEPENDS "${GENERAL_HELP_FILE}"
        COMMENT "Generating ${GENERAL_HELP_HEADER} using xxd..."
)

add_custom_command(
        OUTPUT "${VERSION_HELP_HEADER}"
        COMMAND ${XXD_EXECUTABLE} -i "version.txt" "${VERSION_HELP_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles"
        DEPENDS "${VERSION_HELP_FILE}"
        COMMENT "Generating ${VERSION_HELP_HEADER} using xxd..."
)

add_custom_command(
        OUTPUT "${BACKUP_HELP_HEADER}"
        COMMAND ${XXD_EXECUTABLE} -i "backup.txt" "${BACKUP_HELP_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles"
        DEPENDS "${BACKUP_HELP_FILE}"
        COMMENT "Generating ${BACKUP_HELP_HEADER} using xxd..."
)

add_custom_command(
        OUTPUT "${RESTORE_HELP_HEADER}"
        COMMAND ${XXD_EXECUTABLE} -i "restore.txt" "${RESTORE_HELP_HEADER}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/helpfiles"
        DEPENDS "${RESTORE_HELP_FILE}"
        COMMENT "Generating ${RESTORE_HELP_HEADER} using xxd..."
)

add_executable(savestuff
        src/main.c
        "${GENERAL_HELP_HEADER}"
        "${VERSION_HELP_HEADER}"
        "${BACKUP_HELP_HEADER}"
        "${RESTORE_HELP_HEADER}"
        src/commands.h
        src/ParseArgs.c
        src/ParseArgs.h
        src/savestuff.c
        src/crossplatlimits.h
        src/savestuff.h
        src/backups.c
        src/backups.h
        src/crossplatmkdir.h
        src/cleanup.h
        src/helpmessages.c
        src/helpmessages.h
)

if(WIN32)
    message(STATUS "Windows detected. Adding third-party dirent compatibility layer.")
    target_include_directories(savestuff PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/compat)
endif()

target_include_directories(savestuff PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)