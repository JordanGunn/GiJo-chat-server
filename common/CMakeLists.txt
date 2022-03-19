add_subdirectory(src)
add_subdirectory(include)

SET(CPT_COMMON_TARGET
        ${CPT_COMMON_SOURCE}
        ${CPT_COMMON_HEADERS}
        PARENT_SCOPE)