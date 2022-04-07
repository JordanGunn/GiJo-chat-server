# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
cmake_minimum_required(VERSION 3.20)

project(CPT
        DESCRIPTION "Project"
        LANGUAGES C)

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



# ==============================================================================
#     Add all relevant subdirectories with other CMakeList files
# ==============================================================================
add_subdirectory(common/)
add_subdirectory(cpt_api/)
add_subdirectory(cpt_app/)
add_subdirectory(testing/)
add_subdirectory(interface/)
add_subdirectory(messenger/)
add_subdirectory(networking/)


# ==============================================================================
#    Definition of executable files
# ==============================================================================
add_executable(cpt_chat_server
        ${CPT_COMMON_TARGET}
        ${CHAT_SERVER_TARGET}
        ${CPT_API_SERVER_TGT}
        ${NETWORK_SERVER_TARGET})

add_executable(cpt_chat_client
        ${MESSAGE_SOURCE}
        ${CPT_COMMON_TARGET}
        ${CHAT_CLIENT_TARGET}
        ${CPT_API_CLIENT_TGT}
        ${CLI_INTERFACE_TARGET}
        ${NETWORK_CLIENT_TARGET})

add_executable(cpt_messenger
        ${CPT_COMMON_TARGET}
        ${MESSAGE_TARGET}
        ${MESSAGE_SOURCE})


# ==============================================================================
#    Directory inclusion for SERVER executable
# ==============================================================================
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
target_include_directories(cpt_chat_server PRIVATE networking/tcp/include/)
target_include_directories(cpt_chat_server PRIVATE networking/udp/include/)
target_include_directories(cpt_chat_server PRIVATE cpt_app/server/include/)


# link the local source files
target_link_directories(cpt_chat_server PRIVATE common/src/)
target_link_directories(cpt_chat_server PRIVATE cpt_api/src/)
target_link_directories(cpt_chat_server PRIVATE networking/tcp/src/)
target_link_directories(cpt_chat_server PRIVATE networking/udp/src/)
target_link_directories(cpt_chat_server PRIVATE cpt_app/server/src/)



# ====================================================================
#    Directory inclusion for CLIENT executable
# ====================================================================
# Tell the compiler to check these dirs for any header files
target_include_directories(cpt_chat_client PRIVATE ./include)
target_include_directories(cpt_chat_client PRIVATE /usr/include)
target_include_directories(cpt_chat_client PRIVATE /usr/local/include)

# tell the compiler to check here for 3rd party source files
target_link_directories(cpt_chat_client PRIVATE /usr/lib)
target_link_directories(cpt_chat_client PRIVATE /usr/local/lib)

# tell the compiler where local header files are
target_include_directories(cpt_chat_client PRIVATE messenger/)
target_include_directories(cpt_chat_client PRIVATE common/include/)
target_include_directories(cpt_chat_client PRIVATE cpt_api/include/)
target_include_directories(cpt_chat_client PRIVATE interface/cli/include/)
target_include_directories(cpt_chat_client PRIVATE networking/tcp/include/)
target_include_directories(cpt_chat_client PRIVATE networking/udp/include/)
target_include_directories(cpt_chat_client PRIVATE cpt_app/client/include/)
target_include_directories(cpt_chat_client PRIVATE cpt_api/cpt_voice/include/)
target_include_directories(cpt_chat_client PRIVATE interface/ncurses/include/)
target_include_directories(cpt_chat_client PRIVATE interface/interface/include/)

# link the local source files
target_link_directories(cpt_chat_client PRIVATE messenger/)
target_link_directories(cpt_chat_client PRIVATE common/src/)
target_link_directories(cpt_chat_client PRIVATE cpt_api/src/)
target_link_directories(cpt_chat_client PRIVATE interface/cli/src/)
target_link_directories(cpt_chat_client PRIVATE cpt_app/client/src/)
target_link_directories(cpt_chat_client PRIVATE networking/tcp/src/)
target_link_directories(cpt_chat_client PRIVATE networking/udp/src/)
target_link_directories(cpt_chat_client PRIVATE interface/ncurses/src/)
target_link_directories(cpt_chat_client PRIVATE cpt_api/cpt_voice/src/)
target_link_directories(cpt_chat_client PRIVATE interface/interface/src/)



# ========================================================================
# Directory inclusion for MESSENGER executable
# ========================================================================
# Tell the compiler to check these dirs for any header files
target_include_directories(cpt_messenger PRIVATE /usr/include)
target_include_directories(cpt_messenger PRIVATE /usr/local/include)
target_include_directories(cpt_messenger PRIVATE messenger/)
target_include_directories(cpt_messenger PRIVATE common/include/)

# Tell the compiler to check here for 3rd party source files
target_link_directories(cpt_messenger PRIVATE messenger/)
target_link_directories(cpt_messenger PRIVATE /usr/lib)
target_link_directories(cpt_messenger PRIVATE /usr/local/lib)




# =========================================
# Additional compile options for server
# =========================================
target_compile_features(cpt_chat_server PUBLIC c_std_11)
target_compile_options(cpt_chat_server PRIVATE -g)
target_compile_options(cpt_chat_server PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_server PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(cpt_chat_server PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)




# =========================================
# Additional compile options for client
# =========================================
target_compile_features(cpt_chat_client PUBLIC c_std_11)
target_compile_options(cpt_chat_client PRIVATE -g)
target_compile_options(cpt_chat_client PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_chat_client PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(cpt_chat_client PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)




# =========================================
# Additional compile options for messenger
# =========================================
target_compile_features(cpt_messenger PUBLIC c_std_11)
target_compile_options(cpt_messenger PRIVATE -g)
target_compile_options(cpt_messenger PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(cpt_messenger PRIVATE -Wpedantic -Wall -Wextra)



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



# =============================================
#   Directory for other libs
# =============================================
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(cpt_chat_client PRIVATE Threads::Threads)
target_link_libraries(cpt_messenger PRIVATE Threads::Threads)



# =============================================
#   Honestly no idea what this does.
# =============================================
set_target_properties(cpt_chat_client PROPERTIES OUTPUT_NAME "cpt_chat_client")
install(TARGETS cpt_chat_client DESTINATION bin)
