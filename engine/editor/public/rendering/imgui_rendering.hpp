#ifndef EDITOR_RENDERING_IMGUI_RENDERING_HPP_INCLUDE
#define EDITOR_RENDERING_IMGUI_RENDERING_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/slice.hpp>
#include <imgui/imgui.hpp>

namespace anton_engine::imgui {
    class Draw_Elements_Command {
    public:
        u32 count;
        u32 instance_count;
        u32 first_index;
        u32 base_vertex;
        u32 base_instance;
    };

    void setup_rendering();
    void bind_buffers();
    // Fills count, base_vertex and first_index
    Draw_Elements_Command write_geometry(anton_stl::Slice<Vertex const>, anton_stl::Slice<u32 const>);
    void add_draw_command(Draw_Elements_Command);
    void commit_draw();

} // namespace anton_engine::imgui

#endif // !EDITOR_RENDERING_IMGUI_RENDERING_HPP_INCLUDE
