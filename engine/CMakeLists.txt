add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${GAMEENGINE_FOLDER_NAME}"
)
    
add_subdirectory(dependencies)
add_subdirectory(source)
add_subdirectory(editor)
add_subdirectory(shaders)

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    stb
    assimp
    imgui
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES_ROOT} ${HEADER_FILES_ROOT})

if(MSVC)
    # change binary output directory
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/assets_main\" \"${CMAKE_BINARY_DIR}/bin/assets\" /S /I /Q /Y"
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/data\" \"${CMAKE_BINARY_DIR}/bin\" /S /I /Q /Y"
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/engine/shaders\" \"${CMAKE_BINARY_DIR}/bin/shaders\" /S /I /Q /Y"
    )
elseif(FALSE)
    # add linux
endif()
