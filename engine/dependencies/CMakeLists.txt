set(GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
set(GLFW_BUILD_TESTS CACHE BOOL OFF)
set(GLFW_INSTALL CACHE BOOL OFF)

add_subdirectory(freetype)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(zlib)

set_target_properties(glfw glad zlib zlibstatic freetype
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
)

set_target_properties(glfw glad zlib zlibstatic freetype
    PROPERTIES
    COMPILE_OPTIONS ${ENGINE_CXX_DEPENDENCIES_FLAGS}
)