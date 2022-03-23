#add_subdirectory(src/)
#add_subdirectory(include/)

SET(CLI_TARGET
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_dc_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_dc_app.h
        PARENT_SCOPE)