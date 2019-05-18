set(SOURCE_FILES 
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/components/camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/component_container.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/ecs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/cube.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/plane.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_core.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/guid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/key.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/object.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_core.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_DIR}")
