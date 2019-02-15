add_executable(${PROJECT_NAME}
    main.cpp
    engine.cpp
    engine.hpp
    subsystem_forward_declarations.hpp
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
add_subdirectory(renderer/public)
add_subdirectory(renderer/private)
add_subdirectory(engine/public)
add_subdirectory(engine/private)
add_subdirectory(windowing/private)
add_subdirectory(windowing/public)
add_subdirectory(scripts) # Temporary directory to store a bunch of game scriptsthat e.g. move the camera around

target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}") # to include engine.hpp

target_link_libraries(${PROJECT_NAME}
    glad    
    glfw
    stb
    assimp
)

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