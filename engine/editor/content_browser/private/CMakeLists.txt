set(SOURCE_FILES
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/common.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/obj.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/png.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/tga.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/asset_importing.cpp"
    PARENT_SCOPE
)

set(HEADER_FILES 
    ${HEADER_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/common.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/image.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/mesh.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/obj.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/png.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/importers/tga.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/postprocess.hpp"
    PARENT_SCOPE
)

set(PRIVATE_INCLUDE_DIRECTORIES
    ${PRIVATE_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)
