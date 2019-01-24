add_executable(${PROJECT_NAME}
    engine.cpp
)

set(SOURCE_FILES)
set(HEADER_FILES)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(core/public)
add_subdirectory(core/private)
add_subdirectory(shaders/public)
add_subdirectory(shaders/private)
add_subdirectory(engine/public)
add_subdirectory(engine/private)

target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    stb
    assimp
)