set(SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/quaternion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/transform.cpp
)


foreach(src_file IN LISTS SOURCE_FILES)
source_group(private\\math FILES ${src_file})
endforeach()

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})