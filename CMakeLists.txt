cmake_minimum_required(VERSION 3.12)
project(GameEngine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

set(GAMEENGINE_FOLDER_NAME "Engine")

set(GLFW_BUILD_EXAMPLES CACHE BOOL OFF)
set(GLFW_BUILD_TESTS CACHE BOOL OFF)
set(GLFW_INSTALL CACHE BOOL OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

find_package(OpenGL REQUIRED)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(stb)
add_subdirectory(assimp)
add_subdirectory(engine)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
elseif("${CMAKE_CXX_COMPILER_ID}" matches "GCC" or "${CMAKE_CXX_COMPILER_ID}" matches "Clang")

endif()