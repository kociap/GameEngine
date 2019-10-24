set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/framebuffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glad.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl_enums_defs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl_enums_undefs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opengl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer.hpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/uniform_buffer.hpp"
    PARENT_SCOPE
)

set(PUBLIC_INCLUDE_DIRECTORIES
    ${PUBLIC_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
