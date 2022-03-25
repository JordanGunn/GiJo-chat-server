# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
cmake_minimum_required(VERSION 3.20)

project(CPT
        DESCRIPTION "Project"
        LANGUAGES C)

# add all relevant subdirectories with other CMakeList files
add_subdirectory(common/)
add_subdirectory(tcp_networking/)
add_subdirectory(cpt_api/)
add_subdirectory(cpt_app/)
add_subdirectory(interface/)
add_subdirectory(testing/)


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
add_executable(cpt_chat_server ${CHAT_SERVER_TARGET} ${TCP_SERVER_TARGETS} ${CPT_API_SERVER_TGT} ${CPT_COMMON_TARGET})
add_executable(cpt_chat_client ${CHAT_CLIENT_TARGET} ${TCP_CLIENT_TARGETS}  ${CPT_API_CLIENT_TGT} ${CPT_COMMON_TARGET} ${CLI_TARGET})
add_executable(test ${TEST_TARGET})

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

# tell the compiler where local header files are
target_include_directories(cpt_chat_server PRIVATE common/include/)
target_include_directories(cpt_chat_server PRIVATE cpt_api/include/)
target_include_directories(cpt_chat_server PRIVATE tcp_networking/include/)
target_include_directories(cpt_chat_server PRIVATE cpt_app/server/include/)


# link the local source files
target_link_directories(cpt_chat_server PRIVATE common/src/)
target_link_directories(cpt_chat_server PRIVATE cpt_api/src/)
target_link_directories(cpt_chat_server PRIVATE tcp_networking/src/)
target_link_directories(cpt_chat_server PRIVATE cpt_app/server/src/)

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

# tell the compiler where local header files are
target_include_directories(cpt_chat_client PRIVATE interface/)
target_include_directories(cpt_chat_client PRIVATE common/include/)
target_include_directories(cpt_chat_client PRIVATE cpt_api/include/)
target_include_directories(cpt_chat_client PRIVATE tcp_networking/include/)
target_include_directories(cpt_chat_client PRIVATE cpt_app/client/include/)

# link the local source files
target_link_directories(cpt_chat_client PRIVATE interface/)
target_link_directories(cpt_chat_client PRIVATE common/src/)
target_link_directories(cpt_chat_client PRIVATE cpt_api/src/)
target_link_directories(cpt_chat_client PRIVATE cpt_app/client/src/)
target_link_directories(cpt_chat_client PRIVATE tcp_networking/src/)


# ==========================================
# Directory inclusion for testing executable
# ==========================================
## Tell the compiler to check these dirs for any header files
target_include_directories(test PRIVATE /usr/include)
target_include_directories(test PRIVATE /usr/local/include)
target_include_directories(test PRIVATE testing/include/)
#
## tell the compiler to check here for 3rd party source files
target_link_directories(test PRIVATE /usr/lib)
target_link_directories(test PRIVATE /usr/local/lib)
target_link_directories(test PRIVATE testing/src/)
target_link_directories(test PRIVATE testing/)

# =========================================
# Additional compile options for server
# =========================================
target_compile_features(cpt_chat_server PUBLIC c_std_11)
target_compile_options(cpt_chat_server PRIVATE -g)
target_compile_options(cpt_chat_server PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_server PRIVATE -Wpedantic -Wall -Wextra)
#target_compile_options(cpt_chat_server PRIVATE "${COMPILE_OPTIONS}")

# =========================================
# Additional compile options for client
# =========================================
target_compile_features(cpt_chat_client PUBLIC c_std_11)
target_compile_options(cpt_chat_client PRIVATE -g)
target_compile_options(cpt_chat_client PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_client PRIVATE -Wpedantic -Wall -Wextra)
#target_compile_options(cpt_chat_client PRIVATE ${COMPILE_OPTIONS})

# =========================================
# Additional compile options for sandbox
## =========================================
#target_compile_features(sandbox PUBLIC c_std_11)
#target_compile_options(sandbox PRIVATE -g)
#target_compile_options(sandbox PRIVATE -fstack-protector-all -ftrapv)
#target_compile_options(sandbox PRIVATE -Wpedantic -Wall -Wextra)
#target_compile_options(cpt_chat_client PRIVATE ${COMPILE_OPTIONS})

# =============================================
# Directory inclusion for dc app framework libs
# =============================================
find_library(LIBM m REQUIRED)
find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UTIL dc_util REQUIRED)
find_library(LIBDC_FSM dc_fsm REQUIRED)
find_library(LIBDC_APPLICATION dc_application REQUIRED)

target_link_libraries(cpt_chat_client PRIVATE ${LIBM})
target_link_libraries(cpt_chat_client PRIVATE ${LIBDC_ERROR})
target_link_libraries(cpt_chat_client PRIVATE ${LIBDC_POSIX})
target_link_libraries(cpt_chat_client PRIVATE ${LIBDC_UTIL})
target_link_libraries(cpt_chat_client PRIVATE ${LIBDC_FSM})
target_link_libraries(cpt_chat_client PRIVATE ${LIBDC_APPLICATION})

set_target_properties(cpt_chat_client PROPERTIES OUTPUT_NAME "cpt_chat_client")
install(TARGETS cpt_chat_client DESTINATION bin)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#target_link_libraries(udp_tester_client PRIVATE Threads::Threads)
