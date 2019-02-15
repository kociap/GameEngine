set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/shaderfile.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exceptions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader_manager.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")