#ifndef EDITOR_IMGUI_IMGUI_HPP_INCLUDE
#define EDITOR_IMGUI_IMGUI_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/slice.hpp>
#include <anton_stl/string_view.hpp>
#include <color.hpp>
#include <math/vector2.hpp>

namespace anton_engine::imgui {
    class Context;

    class Style {
    public:
        Color background_color;
    };

    Context* create_context();
    void destroy_context(Context*);

    void set_default_style_default_dark(Context&);
    void set_default_style(Context&, Style);
    Style get_default_style(Context&);

    void begin_frame(Context&);
    void end_frame(Context&);

    class Input_State {
    public:
        Vector2 cursor_position;
        bool left_mouse_button;
        bool right_mouse_button;
    };

    void set_input_state(Context&, Input_State);
    Input_State get_input_state(Context&);

    class Vertex {
    public:
        Vector2 position;
        Vector2 uv;
        struct Color {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        } color;
    };

    class Draw_Command {
    public:
        u32 element_count;
        u32 vertex_offset;
        u32 index_offset;
        u32 texture;
    };

    anton_stl::Slice<Vertex const> get_vertex_data(Context&);
    anton_stl::Slice<u32 const> get_index_data(Context&);
    anton_stl::Slice<Draw_Command const> get_draw_commands(Context&);

    void begin_window(Context&, anton_stl::String_View);
    void end_window(Context&);

    // Generic widget to group other widgets into and manage layout.
    void begin_widget(Context&, anton_stl::String_View);
    void end_widget(Context&);

    // Modifiers

    // Get style of current widget or window.
    Style get_style(Context&);
    // Set style of current widget or window.
    void set_style(Context&, Style);

    void set_window_border_area(Context&, f32);

    void set_window_size(Context&, Vector2);

    void set_window_pos(Context&, Vector2);

    void set_width(Context&, f32 width);
    void set_height(Context&, f32 height);

    // State queries

    bool is_window_hot(Context&);
    bool is_window_active(Context&);

    // bool hovered(Context&);
    // bool clicked(Context&);
} // namespace anton_engine::imgui

#endif // !EDITOR_IMGUI_IMGUI_HPP_INCLUDE
