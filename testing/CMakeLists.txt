add_subdirectory(include/)
add_subdirectory(src/)

set(TEST_TARGET
        ${TEST_SRC}
        ${TEST_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/testing_main.c
        PARENT_SCOPE)