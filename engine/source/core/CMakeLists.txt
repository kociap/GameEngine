project(core CXX)

add_library(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)

add_subdirectory(private)
add_subdirectory(public)