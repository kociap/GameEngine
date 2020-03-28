add_subdirectory(freetype)
add_subdirectory(glad)
add_subdirectory(zlib)
add_subdirectory(mimas)

set_target_properties(glad zlib freetype mimas
    PROPERTIES
    FOLDER "${ENGINE_DEPENDENCIES_FOLDER}"
    POSITION_INDEPENDENT_CODE TRUE
)

target_compile_options(glad PRIVATE ${ANTON_DEPENDENCIES_COMPILE_FLAGS})
target_compile_options(zlib PRIVATE ${ANTON_DEPENDENCIES_COMPILE_FLAGS})
target_compile_options(freetype PRIVATE ${ANTON_DEPENDENCIES_COMPILE_FLAGS})
target_compile_options(mimas PRIVATE ${ANTON_DEPENDENCIES_COMPILE_FLAGS})

target_link_options(glad PRIVATE ${ANTON_DEPENDENCIES_LINK_FLAGS})
target_link_options(zlib PRIVATE ${ANTON_DEPENDENCIES_LINK_FLAGS})
target_link_options(freetype PRIVATE ${ANTON_DEPENDENCIES_LINK_FLAGS})
target_link_options(mimas PRIVATE ${ANTON_DEPENDENCIES_LINK_FLAGS})
