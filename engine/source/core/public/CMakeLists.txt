set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/math/math.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/transform.hpp
)

source_group("public\\math" FILES ${HEADER_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})