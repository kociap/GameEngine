#ifndef RENDERER_OPENGL_HPP_INCLUDE
#define RENDERER_OPENGL_HPP_INCLUDE

#include <cstdint>

namespace opengl {
    int32_t get_max_combined_texture_units();

    void active_texture(uint32_t index);
    void bind_texture(uint32_t tex_enum, uint32_t handle);

    void load_opengl_parameters();
} // namespace opengl

#endif // !RENDERER_OPENGL_HPP_INCLUDE
