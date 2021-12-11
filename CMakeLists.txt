# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
cmake_minimum_required(VERSION 3.20)
project(client-server-practice)

set(SERVER_SOURCE_LIST
        "server.c")

set(CLIENT_SOURCE_LIST
        "client.c")

add_compile_definitions(_POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)

if(APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
endif()

find_program(LINT "clang-tidy")
IF(LINT)
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*,-llvmlibc-restrict-system-libc-headers,-cppcoreguidelines-init-variables,-clang-analyzer-security.insecureAPI.strcpy,-concurrency-mt-unsafe,-android-cloexec-accept,-android-cloexec-dup,-google-readability-todo,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cert-dcl03-c,-hicpp-static-assert,-misc-static-assert,-altera-struct-pack-align,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling;--quiet")
ENDIF()

# Make a executables
add_executable(jg_server ${SERVER_SOURCE_LIST})
add_executable(jg_client  ${CLIENT_SOURCE_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(jg_server PRIVATE ../include)
target_include_directories(jg_server PRIVATE /usr/include)
target_include_directories(jg_server PRIVATE /usr/local/include)
target_link_directories(jg_server PRIVATE /usr/lib)
target_link_directories(jg_server PRIVATE /usr/local/lib)

target_include_directories(jg_client PRIVATE ../include)
target_include_directories(jg_client PRIVATE /usr/include)
target_include_directories(jg_client PRIVATE /usr/local/include)
target_link_directories(jg_client PRIVATE /usr/lib)
target_link_directories(jg_client PRIVATE /usr/local/lib)

# All users of this library will need at least C11
target_compile_features(jg_server PUBLIC c_std_11)
target_compile_options(jg_server PRIVATE -g)
target_compile_options(jg_server PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(jg_server PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(jg_server PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)

target_compile_features(jg_client PUBLIC c_std_11)
target_compile_options(jg_client PRIVATE -g)
target_compile_options(jg_client PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(jg_client PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(jg_client PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)

find_library(LIBM m REQUIRED)
if (LIBM)
    target_link_libraries(jg_server ${LIBM})
    target_link_libraries(jg_client ${LIBM})
endif ()

find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UTIL dc_util REQUIRED)
find_library(LIBDC_FSM dc_fsm REQUIRED)
find_library(LIBDC_NETWORK dc_network REQUIRED)
find_library(LIBDC_APPLICATION dc_application REQUIRED)
find_library(LIBCONFIG config REQUIRED)

target_link_libraries(jg_server ${LIBDC_ERROR})
target_link_libraries(jg_client ${LIBDC_ERROR})
target_link_libraries(jg_server ${LIBDC_POSIX})
target_link_libraries(jg_client ${LIBDC_POSIX})
target_link_libraries(jg_server ${LIBDC_UTIL})
target_link_libraries(jg_client ${LIBDC_UTIL})
target_link_libraries(jg_server ${LIBDC_FSM})
target_link_libraries(jg_client ${LIBDC_FSM})
target_link_libraries(jg_server ${LIBDC_NETWORK})
target_link_libraries(jg_client ${LIBDC_NETWORK})
target_link_libraries(jg_server ${LIBDC_APPLICATION})
target_link_libraries(jg_client ${LIBDC_APPLICATION})
target_link_libraries(jg_server ${LIBCONFIG})
target_link_libraries(jg_client ${LIBCONFIG})

# set_target_properties(jg_server PROPERTIES OUTPUT_NAME "dcecho")
# install(TARGETS dc_echo DESTINATION bin)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)

add_custom_target(
        format
        COMMAND clang-format
        -i
        ${HEADER_LIST}
        ${HEADER_SYS_LIST}
        ${SOURCE_LIST}
)
