cmake_minimum_required(VERSION 3.12)
project(GameEngine)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(glfw)
add_subdirectory(glad)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif()

set(SOURCE_FILES 
    engine/source/engine.cpp
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_FILES}
)