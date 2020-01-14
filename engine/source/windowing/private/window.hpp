#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <anton_int.hpp>
#include <math/vector2.hpp>

namespace anton_engine {
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

    bool should_close(Window*);
    void resize(Window*, f32 width, f32 height);
    f32 get_width(Window*);
    f32 get_height(Window*);
    Vector2 get_window_size(Window*);
    void set_opacity(Window*, float);

    void make_context_current(Window*);
    void swap_buffers(Window*);
    void enable_vsync(bool);
} // namespace anton_engine

#endif // !WINDOWING_WINDOW_HPP_INCLUDE
