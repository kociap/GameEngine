#ifndef RENDERER_OPENGL_HPP_INCLUDE
#define RENDERER_OPENGL_HPP_INCLUDE

#include <cstdint>

namespace opengl {
    int32_t get_max_combined_texture_units();

    void active_texture(uint32_t index);
    void bind_texture(uint32_t tex_enum, uint32_t handle);
    void bind_vertex_array(uint32_t handle);
    void vertex_array_attribute(uint32_t index, uint32_t count, uint32_t type, uint32_t stride, uint32_t offset, bool normalized = false);
    void enable_vertex_array_attribute(uint32_t index);
    void draw_elements(uint32_t mode, uint32_t count);
    void draw_elements_instanced(uint32_t mode, uint32_t indices_count, uint32_t instances);

    void load_opengl_parameters();
} // namespace opengl

#endif // !RENDERER_OPENGL_HPP_INCLUDE
