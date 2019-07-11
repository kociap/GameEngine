set(GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
set(GLFW_BUILD_TESTS CACHE BOOL OFF)
set(GLFW_INSTALL CACHE BOOL OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(stb)
add_subdirectory(assimp)
add_subdirectory(imgui)
add_subdirectory(zlib)

set_target_properties(glfw assimp imgui stb glad zlib_lib zlibstatic_lib
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
)

if(MSVC)
    set_target_properties(glfw assimp imgui stb glad zlib_lib zlibstatic_lib
        PROPERTIES
        COMPILE_OPTIONS "/W0"
    )
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")
    set_target_properties(glfw assimp imgui stb glad zlib_lib zlibstatic_lib
        PROPERTIES
        COMPILE_OPTIONS "-Wno-everything"
    )
endif()