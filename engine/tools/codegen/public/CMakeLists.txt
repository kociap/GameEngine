set(HEADER_FILES ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/component_header_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/file.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")