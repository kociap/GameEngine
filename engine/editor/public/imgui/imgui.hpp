#ifndef EDITOR_IMGUI_IMGUI_HPP_INCLUDE
#define EDITOR_IMGUI_IMGUI_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>
#include <core/color.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector4.hpp>
#include <windowing/window.hpp>

#include <rendering/fonts.hpp> // because of Font_Face;

namespace anton_engine::imgui {
    class Context;
    class Viewport;

    class Widget_Style {
    public:
        Color background_color;
    };

    class Font_Style {
    public:
        rendering::Font_Face* face;
        // Font size in points
        u32 size;
        u32 h_dpi;
        u32 v_dpi;
    };

    class Button_Style {
    public:
        Color border_color;
        Color background_color;
        Vector4 border;
        Vector4 padding;
        Font_Style font;
    };

    class Style {
    public:
        Color background_color;
        Color preview_guides_color;
        Color preview_color;
        Widget_Style widgets;
        Button_Style buttons;
    };

    class Settings {
    public:
        f32 window_drop_area_width;
    };

    Context* create_context();
    void destroy_context(Context*);

    // Main viewport's native window is not owned by imgui.
    void set_main_viewport_native_window(Context&, windowing::Window*);

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
        // texture 0 means no texture is associated with this draw command
        u32 texture;
    };

    atl::Slice<Viewport* const> get_viewports(Context&);
    windowing::Window* get_viewport_native_window(Context&, Viewport&);
    atl::Slice<Draw_Command const> get_viewport_draw_commands(Context&, Viewport&);

    atl::Slice<Vertex const> get_vertex_data(Context&);
    atl::Slice<u32 const> get_index_data(Context&);

    void begin_window(Context&, atl::String_View, bool new_viewport = false);
    void end_window(Context&);

    enum class Button_State {
        inactive, hot, clicked,
    };

    // Generic widget to group other widgets and manage layout.
    void begin_widget(Context&, atl::String_View identifier, Widget_Style options);
    void end_widget(Context&);

    void text(Context&, atl::String_View text, Font_Style font);
    Button_State button(Context&, atl::String_View text, Button_Style style, Button_Style hovered_style, Button_Style active_style);

    // Modifiers

    // Get style of current widget or window.
    Style get_style(Context&);
    // Set style of current widget or window.
    void set_style(Context&, Style);

    void set_window_border_area(Context&, f32);

    void set_window_size(Context&, Vector2);

    // Relative to window's viewport.
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
