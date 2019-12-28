set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine_main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/module_loader.cpp"
)

set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/build_config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/module_loader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/subsystem_forward_declarations.hpp"
)

add_subdirectory(core/public)
add_subdirectory(core/private)
add_subdirectory(shaders/public)
add_subdirectory(shaders/private)
add_subdirectory(rendering/public)
add_subdirectory(rendering/private)
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

set(PRIVATE_INCLUDE_DIRECTORIES_ROOT
    ${PRIVATE_INCLUDE_DIRECTORIES_ROOT}
    ${PRIVATE_INCLUDE_DIRECTORIES}
    PARENT_SCOPE
)

set(PUBLIC_INCLUDE_DIRECTORIES_ROOT
    ${PUBLIC_INCLUDE_DIRECTORIES_ROOT}
    ${PUBLIC_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
