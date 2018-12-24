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

function(source_groups_from_paths root_path)
    foreach(file IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${file}")
            file(RELATIVE_PATH file_relative "${root_path}" "${file}")
        else()
            set(file_relative "${file}")
        endif()
        get_filename_component(file_path "${file_relative}" PATH)
        string(REPLACE "/" "\\" file_path "${file_path}")
        source_group("${file_path}" FILES "${file}")
    endforeach()
endfunction(source_groups_from_paths)

target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)
source_groups_from_paths("${CMAKE_CURRENT_SOURCE_DIR}" ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    stb
)