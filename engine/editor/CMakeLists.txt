set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_renderer.hpp"    
)

add_subdirectory(leveleditor/public)
add_subdirectory(leveleditor/private)

set(SOURCE_FILES_ROOT
    ${SOURCE_FILES_ROOT}
    ${SOURCE_FILES}
    PARENT_SCOPE
)

set(HEADER_FILES_ROOT
    ${HEADER_FILES_ROOT}
    ${HEADER_FILES}
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}") # to include editor.hpp
target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)
