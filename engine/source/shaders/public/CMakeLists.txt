set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/shader_file.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader_exceptions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")