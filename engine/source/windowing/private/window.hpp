#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <anton_int.hpp>
#include <key.hpp>
#include <math/vector2.hpp>

namespace anton_engine::windowing {
    class Window;
    class OpenGL_Context;

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

    // Returns true if windowing has been successfully initialized.
    bool init();
    void terminate();
    void poll_events();

    Window* create_window(f32 width, f32 height, Window* share, bool create_context, bool decorated);
    void destroy_window(Window*);

    void set_cursor_mode(Window*, Cursor_Mode);
    Cursor_Mode get_cursor_mode(Window*);

    using window_resize_function = void (*)(Window*, f32 width, f32 height, void* user_data);
    using window_activate_function = void (*)(Window*, bool activated, void* user_data);
    using cursor_pos_function = void (*)(Window*, f32 x, f32 y, void* user_data);
    // TODO: enum for action?
    // TODO: add mods?
    using mouse_button_function = void (*)(Window*, Key, i32 action, void* user_data);
    using scroll_function = void (*)(Window*, f32 x_offset, f32 y_offset, void* user_data);
    using key_function = void (*)(Window*, Key key, i32 action, void* user_data);
    using joystick_function = void (*)(i32 joystick, i32 action, void* user_data);

    void set_window_resize_callback(Window*, window_resize_function, void* user_data = nullptr);
    void set_window_activate_callback(Window*, window_activate_function, void* user_data = nullptr);
    void set_cursor_pos_callback(Window*, cursor_pos_function, void* user_data = nullptr);
    void set_mouse_button_callback(Window*, mouse_button_function, void* user_data = nullptr);
    void set_scroll_callback(Window*, scroll_function, void* user_data = nullptr);
    void set_key_callback(Window*, key_function, void* user_data = nullptr);
    void set_joystick_callback(joystick_function, void* user_data = nullptr);

    // Returns whether the specified key has been pressed.
    bool get_key(Key);
    // Retrieves cursor position.
    Vector2 get_cursor_pos();

    bool should_close(Window*);
    void resize(Window*, f32 width, f32 height);
    Vector2 get_window_size(Window*);
    Vector2 get_window_pos(Window*);
    void set_window_pos(Window*, Vector2);
    void set_opacity(Window*, float);
    void focus_window(Window*);

    enum class OpenGL_Profile {
        core,
        compatibility,
    };

    OpenGL_Context* create_context(i32 major, i32 minor, OpenGL_Profile);
    void destroy_context(OpenGL_Context*);
    void make_context_current(OpenGL_Context*, Window*);
    void swap_buffers(Window*);
    void enable_vsync(bool);
} // namespace anton_engine::windowing

#endif // !WINDOWING_WINDOW_HPP_INCLUDE
