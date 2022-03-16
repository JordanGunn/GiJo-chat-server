# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
cmake_minimum_required(VERSION 3.20)

project(CPT
        VERSION 0.1
        DESCRIPTION "Project"
        LANGUAGES C)

# add all relevant subdirectories with other CMakeList files
add_subdirectory(${CPT_SOURCE_DIR}/src/app)
add_subdirectory(${CPT_SOURCE_DIR}/include/app)
add_subdirectory(${CPT_SOURCE_DIR}/src/cpt_api)
add_subdirectory(${CPT_SOURCE_DIR}/include/cpt_api)
add_subdirectory(${CPT_SOURCE_DIR}/src/tcp_networking)
add_subdirectory(${CPT_SOURCE_DIR}/include/tcp_networking)

# To be honest I have no idea what this does
add_compile_definitions(_POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)

# if on apple
if(APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
endif()

# set the linter
find_program(LINT "clang-tidy")
IF(LINT)
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*,-llvmlibc-restrict-system-libc-headers,-cppcoreguidelines-init-variables,-clang-analyzer-security.insecureAPI.strcpy,-concurrency-mt-unsafe,-android-cloexec-accept,-android-cloexec-dup,-google-readability-todo,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cert-dcl03-c,-hicpp-static-assert,-misc-static-assert,-altera-struct-pack-align,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling;--quiet")
ENDIF()


# Make a executables
add_executable(cpt_chat_server src/app/cpt_chat_server.c ${SERVER_SRC_LIST})
add_executable(cpt_chat_client src/app/cpt_chat_client.c ${CLIENT_SRC_LIST})

target_include_directories(cpt_chat_server PUBLIC ${SERVER_HEADER_LIST})
target_include_directories(cpt_chat_server PUBLIC ${CLIENT_HEADER_LIST})

# =========================================
# Directory inclusion for server executable
# =========================================
# Tell the compiler to check these dirs for any header files
target_include_directories(cpt_chat_server PRIVATE ./include)
target_include_directories(cpt_chat_server PRIVATE /usr/include)
target_include_directories(cpt_chat_server PRIVATE /usr/local/include)

# tell the compiler to check here for 3rd party source files
target_link_directories(cpt_chat_server PRIVATE /usr/lib)
target_link_directories(cpt_chat_server PRIVATE /usr/local/lib)

# =========================================
# Directory inclusion for client executable
# =========================================
# Tell the compiler to check these dirs for any header files
target_include_directories(cpt_chat_client PRIVATE ./include)
target_include_directories(cpt_chat_client PRIVATE /usr/include)
target_include_directories(cpt_chat_client PRIVATE /usr/local/include)

# tell the compiler to check here for 3rd party source files
target_link_directories(cpt_chat_client PRIVATE /usr/lib)
target_link_directories(cpt_chat_client PRIVATE /usr/local/lib)

SET(COMPILE_OPTIONS
    -Wdouble-promotion -Wformat-nonliteral
    -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default
    -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal
    -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined
    -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wstrict-prototypes
    -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Winline
    -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs
    -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings )

# =========================================
# Additional compile options for server
# =========================================
target_compile_features(cpt_chat_server PUBLIC c_std_11)
target_compile_options(cpt_chat_server PRIVATE -g)
target_compile_options(cpt_chat_server PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_server PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(cpt_chat_server PRIVATE "${COMPILE_OPTIONS}")

# =========================================
# Additional compile options for client
# =========================================
target_compile_features(cpt_chat_client PUBLIC c_std_11)
target_compile_options(cpt_chat_client PRIVATE -g)
target_compile_options(cpt_chat_client PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_client PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(cpt_chat_client PRIVATE ${COMPILE_OPTIONS})


# =========================================
# All necessary libraries for executables
# =========================================
find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UTIL dc_util REQUIRED)
find_library(LIBDC_FSM dc_fsm REQUIRED)
find_library(LIBDC_NETWORK dc_network REQUIRED)
find_library(LIBDC_APPLICATION dc_application REQUIRED)
find_library(LIBCONFIG config REQUIRED)


# =========================================
# Link the libraries for client executable
# =========================================
target_link_libraries(cpt_chat_server ${LIBDC_ERROR})
target_link_libraries(cpt_chat_server ${LIBDC_POSIX})
target_link_libraries(cpt_chat_server ${LIBDC_UTIL})
target_link_libraries(cpt_chat_server ${LIBDC_FSM})
target_link_libraries(cpt_chat_server ${LIBDC_NETWORK})
target_link_libraries(cpt_chat_server ${LIBDC_APPLICATION})
target_link_libraries(cpt_chat_server ${LIBCONFIG})

# =========================================
# Link the libraries for server executable
# =========================================
target_link_libraries(cpt_chat_client ${LIBDC_ERROR})
target_link_libraries(cpt_chat_client ${LIBDC_POSIX})
target_link_libraries(cpt_chat_client ${LIBDC_UTIL})
target_link_libraries(cpt_chat_client ${LIBDC_FSM})
target_link_libraries(cpt_chat_client ${LIBDC_NETWORK})
target_link_libraries(cpt_chat_client ${LIBDC_APPLICATION})
target_link_libraries(cpt_chat_client ${LIBCONFIG})
