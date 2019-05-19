set(TEMPLATE_IMPLEMENTATION_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/resource_manager.tpp"
)

set(HEADER_FILES ${HEADER_FILES} ${TEMPLATE_IMPLEMENTATION_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/components/camera.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/directional_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/line_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/point_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/spot_light_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/static_mesh_component.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/component_access.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/component_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/ecs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ecs/entity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/input.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input/key_state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/cube.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mesh/plane.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/guid.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/key.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/resource_manager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
