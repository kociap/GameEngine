if(WIN32) # TODO other platforms
    set(PLATFORM_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/time/time_win.cpp"
    )
endif()

set(SOURCE_FILES 
    ${SOURCE_FILES} ${PLATFORM_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/component_serialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/ecs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/cube.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/plane.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/guid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/key.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_platform.hpp"
    PARENT_SCOPE
)

set(PRIVATE_INCLUDE_DIRECTORIES
    ${PRIVATE_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
