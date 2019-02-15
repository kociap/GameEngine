set(SOURCE_FILES 
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/game_object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/base_component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/renderable_component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/static_mesh_component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/line_component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/directional_light_component.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/camera.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/component_system.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/guid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_core.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh_manager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/plane.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/cube.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/key.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input_core.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time/time_core.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_DIR}")
