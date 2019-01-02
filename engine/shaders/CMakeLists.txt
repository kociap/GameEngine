set(SHADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/basicvertex.vert"
)

add_custom_target("shaders"
    SOURCES ${SHADER_FILES}
)

set_target_properties("shaders"
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)