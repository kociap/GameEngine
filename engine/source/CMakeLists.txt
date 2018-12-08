add_executable(${PROJECT_NAME}
    engine.cpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(core)
add_subdirectory(shaders)

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    core
    shaders
)