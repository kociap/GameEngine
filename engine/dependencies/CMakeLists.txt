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

set_target_properties(glfw assimp imgui stb glad
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
)