#ifndef WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE
#define WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE

#include <mimas/mimas_gl.h>
#include <window.hpp>

namespace anton_engine::windowing {
    class Window {
    public:
        Mimas_Window* mimas_window = nullptr;
        struct {
            window_resize_function window_resize = nullptr;
            window_activate_function window_activate = nullptr;
            cursor_pos_function cursor_pos = nullptr;
            mouse_button_function mouse_button = nullptr;
            scroll_function scroll = nullptr;
            key_function key = nullptr;
            void* window_resize_data = nullptr;
            void* window_activate_data = nullptr;
            void* cursor_pos_data = nullptr;
            void* mouse_button_data = nullptr;
            void* scroll_data = nullptr;
            void* key_data = nullptr;
        } callbacks;
    };

    void poll_user_input_devices_state();
} // namespace anton_engine::windowing

#endif // !WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE
