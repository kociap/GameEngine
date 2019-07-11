set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/module_loader.cpp"
)

set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/build_config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/component_serialization_funcs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine_main.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/module_loader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/subsystem_forward_declarations.hpp"
)

add_subdirectory(core/public)
add_subdirectory(core/private)
add_subdirectory(shaders/public)
add_subdirectory(shaders/private)
add_subdirectory(renderer/public)
add_subdirectory(renderer/private)
add_subdirectory(engine/public)
add_subdirectory(engine/private)
add_subdirectory(physics/public)
add_subdirectory(physics/private)
add_subdirectory(windowing/public)
add_subdirectory(windowing/private)
add_subdirectory(scripts) # TODO Temporary directory to store a bunch of game scripts that e.g. move the camera around

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

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}") # to include engine.hpp
target_sources(${PROJECT_NAME} 
    PRIVATE ${SOURCE_FILES} 
    PRIVATE ${HEADER_FILES}
)