add_executable(${PROJECT_NAME}
    engine.cpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)

add_subdirectory(core)

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    core
)