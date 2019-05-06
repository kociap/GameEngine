set(GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
set(GLFW_BUILD_TESTS CACHE BOOL OFF)
set(GLFW_INSTALL CACHE BOOL OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(stb)
add_subdirectory(assimp)
include("ImGuiCmakeLists.cmake")

set_target_properties(glfw
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}/GLFW"
)

set_target_properties(assimp
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}/assimp"
)