set(GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
set(GLFW_BUILD_TESTS CACHE BOOL OFF)
set(GLFW_INSTALL CACHE BOOL OFF)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(stb)
add_subdirectory(imgui)
add_subdirectory(zlib)

set_target_properties(glfw imgui stb glad zlib zlibstatic
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
)

if(MSVC)
    set_target_properties(glfw imgui stb glad zlib zlibstatic
        PROPERTIES
        COMPILE_OPTIONS "/W0"
    )
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")
    set_target_properties(glfw imgui stb glad zlib zlibstatic
        PROPERTIES
        COMPILE_OPTIONS "-Wno-everything"
    )
endif()