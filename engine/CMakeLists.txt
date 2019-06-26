add_library(${ENGINE_NAME} SHARED)
set_target_properties(${ENGINE_NAME}
    PROPERTIES
    FOLDER ${ENGINE_FOLDER_NAME}
    WINDOWS_EXPORT_ALL_SYMBOLS ON # Export all symbols, so we don't have to do it manually via macros
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)

target_compile_definitions(${ENGINE_NAME} PRIVATE COMPILING_DLL)

find_package(OpenGL REQUIRED)
add_subdirectory(dependencies)
add_subdirectory(source)
add_subdirectory(editor)
add_subdirectory(shaders)

target_link_libraries(${ENGINE_NAME}
    ${OPENGL_LIBRARIES}
    glad    
    glfw
    stb
    assimp
    imgui
)

# get_target_property(ENGINE_INCLUDE_DIRECTORIES ${ENGINE_NAME} INCLUDE_DIRECTORIES)
# add_subdirectory(tools)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES_ROOT} ${HEADER_FILES_ROOT})

if(MSVC)
    # add_custom_command(
    #     TARGET ${PROJECT_NAME}
    #     POST_BUILD
    #     COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/assets_main\" \"${CMAKE_BINARY_DIR}/bin/assets\" /S /I /Q /Y"
    # )

    # add_custom_command(
    #     TARGET ${PROJECT_NAME}
    #     POST_BUILD
    #     COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/data\" \"${CMAKE_BINARY_DIR}/bin\" /S /I /Q /Y"
    # )

    # add_custom_command(
    #     TARGET ${PROJECT_NAME}
    #     POST_BUILD
    #     COMMAND "xcopy" ARGS "\"${CMAKE_SOURCE_DIR}/engine/shaders\" \"${CMAKE_BINARY_DIR}/bin/shaders\" /S /I /Q /Y"
    # )
elseif(FALSE)
    # add linux
endif()


set(ENGINE_LOADER_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
)

add_executable(${ENGINE_LOADER_NAME} ${ENGINE_LOADER_SOURCE_FILES})
set_target_properties(${ENGINE_LOADER_NAME}
    PROPERTIES
    FOLDER ${ENGINE_FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${ENGINE_LOADER_NAME} ${ENGINE_NAME})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ENGINE_LOADER_SOURCE_FILES})

