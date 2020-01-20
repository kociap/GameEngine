#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <anton_int.hpp>
#include <key.hpp>
#include <math/vector2.hpp>

namespace anton_engine::windowing {
    class Window;

    enum class Cursor_Mode {
        // Visible and unrestricted
        normal,
        // Invisible, but unrestricted
        hidden,
        // Confined to the content area of a window
        captured,
        // Invisible and locked in place. Provides virtual and unlimited cursor movement
        locked,
    };

    Window* create_window(f32 width, f32 height, bool decorated);
    void destroy_window(Window*);

    void set_cursor_mode(Window*, Cursor_Mode);
    Cursor_Mode get_cursor_mode(Window*);

    using window_resize_function = void (*)(Window*, f32 width, f32 height, void* user_data);
    using cursor_pos_function = void (*)(Window*, f32 x, f32 y, void* user_data);
    // TODO: enum for action?
    // TODO: add mods?
    using mouse_button_function = void (*)(Window*, Key, i32 action, void* user_data);
    using scroll_function = void (*)(Window*, f32 x_offset, f32 y_offset, void* user_data);
    using key_function = void (*)(Window*, Key width, i32 action, void* user_data);
    using joystick_function = void (*)(i32 joystick, i32 action, void* user_data);

    void set_window_resize_callback(Window*, window_resize_function, void* user_data = nullptr);
    void set_cursor_pos_callback(Window*, cursor_pos_function, void* user_data = nullptr);
    void set_mouse_button_callback(Window*, mouse_button_function, void* user_data = nullptr);
    void set_scroll_callback(Window*, scroll_function, void* user_data = nullptr);
    void set_key_callback(Window*, key_function, void* user_data = nullptr);
    void set_joystick_callback(joystick_function, void* user_data = nullptr);

    // Retrieves cursor position relative to the given window
    Vector2 get_cursor_pos(Window*);

    bool should_close(Window*);
    void resize(Window*, f32 width, f32 height);
    f32 get_width(Window*);
    f32 get_height(Window*);
    Vector2 get_window_size(Window*);
    void set_opacity(Window*, float);

    void make_context_current(Window*);
    void swap_buffers(Window*);
    void enable_vsync(bool);
} // namespace anton_engine::windowing

#endif // !WINDOWING_WINDOW_HPP_INCLUDE
