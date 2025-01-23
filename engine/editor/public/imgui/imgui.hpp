#pragma once

#include <anton/math/vec2.hpp>
#include <anton/math/vec4.hpp>
#include <anton/slice.hpp>
#include <anton/string_view.hpp>
#include <core/color.hpp>
#include <core/types.hpp>
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
    Vec4 border;
    Vec4 padding;
    Font_Style font;
  };

  class Style {
  public:
    Color background_color;
    Color preview_guides_color;
    Color preview_color;
    Widget_Style widgets;
    Button_Style button;
    Button_Style hot_button;
    Button_Style active_button;
  };

  class Settings {
  public:
    f32 window_drop_area_width;
  };

  Context* create_context(Font_Style default_font);
  void destroy_context(Context*);

  // Main viewport's native window is not owned by imgui.
  void set_main_viewport_native_window(Context& ctx, windowing::Window*);

  void set_default_font(Context& ctx, Font_Style);
  void set_default_style_default_dark(Context& ctx);
  void set_default_style(Context& ctx, Style);
  Style get_default_style(Context& ctx);

  void begin_frame(Context& ctx);
  void end_frame(Context& ctx);

  class Input_State {
  public:
    Vec2 cursor_position;
    bool left_mouse_button;
    bool right_mouse_button;
  };

  void set_input_state(Context& ctx, Input_State);
  Input_State get_input_state(Context& ctx);

  class Vertex {
  public:
    Vec2 position;
    Vec2 uv;
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
    u64 texture;
  };

  anton::Slice<Viewport* const> get_viewports(Context& ctx);
  windowing::Window* get_viewport_native_window(Context& ctx, Viewport&);
  anton::Slice<Draw_Command const> get_viewport_draw_commands(Context& ctx,
                                                              Viewport&);

  anton::Slice<Vertex const> get_vertex_data(Context& ctx);
  anton::Slice<u32 const> get_index_data(Context& ctx);

  void begin_window(Context& ctx, anton::String_View);
  void end_window(Context& ctx);

  enum class Button_State {
    inactive,
    hot,
    clicked,
  };

  // Generic widget to group other widgets and manage layout.
  void begin_widget(Context& ctx, anton::String_View identifier,
                    Widget_Style options);
  void end_widget(Context& ctx);

  void text(Context& ctx, anton::String_View text, Font_Style font);
  Button_State button(Context& ctx, anton::String_View text);
  Button_State button(Context& ctx, anton::String_View text, Button_Style style,
                      Button_Style hovered_style, Button_Style active_style);
  void image(Context& ctx, u64 texture, Vec2 size, Vec2 uv_top_left,
             Vec2 uv_bottom_right);

  // Modifiers

  // Get style of current widget or window.
  Style get_style(Context& ctx);
  // Set style of current widget or window.
  void set_style(Context& ctx, Style);

  void set_window_border_area(Context& ctx, f32);

  // Sets the content size of the current window.
  // This function has any effect only when the current window is the only one in
  // a viewport, in which case both the window and the viewport are resized.
  // Otherwise this function has no effect.
  //
  void set_window_size(Context& ctx, Vec2 size);

  // Sets the screen position of the current window.
  // This function has any effect only when the current window is the only one in
  // a viewport, in which case both the window and the viewport are repositioned.
  // Otherwise this function has no effect.
  //
  void set_window_pos(Context& ctx, Vec2 screen_pos);

  void set_width(Context& ctx, f32 width);
  void set_height(Context& ctx, f32 height);
  Vec2 get_window_dimensions(Context& ctx);
  Vec2 get_cursor_position(Context& ctx);

  // State queries

  bool is_window_hot(Context& ctx);
  bool is_window_active(Context& ctx);
} // namespace anton_engine::imgui
