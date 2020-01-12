#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <anton_int.hpp>
#include <math/vector2.hpp>

namespace anton_engine {
    class Window;

    struct Dimensions {
        i32 width;
        i32 height;
    };

    Window* create_window(i32 width, i32 height, bool decorated);
    void destroy_window(Window*);

    bool should_close(Window*);
    void resize(Window*, i32 width, i32 height);
    i32 get_width(Window*);
    i32 get_height(Window*);
    Dimensions get_window_size(Window*);
    void set_opacity(Window*, float);

    void make_context_current(Window*);
    void swap_buffers(Window*);
    void enable_vsync(bool);
} // namespace anton_engine

#endif // !WINDOWING_WINDOW_HPP_INCLUDE
