set(TEMPLATE_IMPLEMENTATION_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/vector.tpp"
)

set(HEADER_FILES ${HEADER_FILES} ${TEMPLATE_IMPLEMENTATION_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/math/math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/transform.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/pool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/vector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/iterator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/color.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_macros.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/macro_undefs.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handle.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/singleton.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/path.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/simple_xml_parser.hpp"
    PARENT_SCOPE
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")