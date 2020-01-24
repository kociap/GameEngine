#ifndef WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE
#define WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE

#include <glfw.hpp>
#include <window.hpp>

namespace anton_engine::windowing {
    class Window {
    public:
        window_resize_function window_resize_callback;
        cursor_pos_function cursor_pos_callback;
        mouse_button_function mouse_button_callback;
        scroll_function scroll_callback;
        key_function key_callback;
        void* window_resize_callback_user_data;
        void* cursor_pos_callback_user_data;
        void* mouse_button_callback_user_data;
        void* scroll_callback_user_data;
        void* key_callback_user_data;
        GLFWwindow* glfw_window;
    };

    void poll_user_input_devices_state();
} // namespace anton_engine::windowing

#endif // !WINDOWING_WINDOW_INTERNAL_HPP_INCLUDE
