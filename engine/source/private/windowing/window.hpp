#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <core/math/vector2.hpp>
#include <core/types.hpp>
#include <engine/key.hpp>

namespace anton_engine::windowing {
    class Window;
    class Display;
    class OpenGL_Context;
    class Cursor;

    enum class Standard_Cursor {
        arrow,
        resize_right,
        resize_left,
        resize_top,
        resize_bottom,
        resize_top_left,
        resize_top_right,
        resize_bottom_left,
        resize_bottom_right,
    };

    // Returns true if windowing has been successfully initialized.
    bool init();
    void terminate();
    void poll_events();

    Display* get_primary_display();
    void fullscreen_window(Window* window, Display* display);

    Window* create_window(f32 width, f32 height, bool decorated);
    void destroy_window(Window*);

    void clip_cursor(Window*, Rect_i32 const*);
    void lock_cursor(Window*);
    void unlock_cursor(Window*);
    void set_cursor(Window* window, Cursor* cursor);
    Cursor* create_standard_cursor(Standard_Cursor cursor);
    void destroy_cursor(Cursor* cursor);

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

    bool close_requested(Window*);
    void resize(Window*, f32 width, f32 height);
    Vector2 get_window_size(Window*);
    Vector2 get_window_pos(Window*);
    void set_window_pos(Window*, Vector2);
    void set_size(Window* window, Vector2 size);
    bool is_active(Window* window);

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
