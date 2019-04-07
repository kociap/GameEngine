set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/framebuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/uniform_buffer.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl_enums_defs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl_enums_undefs.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")