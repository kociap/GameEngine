set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/dock_widget.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")