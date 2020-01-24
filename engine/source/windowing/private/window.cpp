#include <window.hpp>

#include <anton_assert.hpp>
#include <anton_stl/vector.hpp>
#include <diagnostic_macros.hpp>
#include <exception.hpp>
#include <glfw.hpp>
#include <window_internal.hpp>

namespace anton_engine::windowing {
    class Windowing {
    public:
        joystick_function joystick_callback;
        void* joystick_callback_user_data;
        anton_stl::Vector<Window*> windows;
    };

    static Windowing windowing;

    bool init() {
        if (!glfwInit()) {
            return false;
        }

        windowing.joystick_callback = nullptr;
        windowing.joystick_callback_user_data = nullptr;

        return true;
    }

    void terminate() {
        windowing.joystick_callback = nullptr;
        windowing.joystick_callback_user_data = nullptr;

        for (Window* window: windowing.windows) {
            destroy_window(window);
        }

        glfwTerminate();
    }

    void poll_events() {
        glfwPollEvents();
        poll_user_input_devices_state();
    }

    static void _window_resize_callback(GLFWwindow* const glfw_window, int const width, int const height) {
        for (Window* const window: windowing.windows) {
            if (window->glfw_window == glfw_window && window->window_resize_callback) {
                window->window_resize_callback(window, width, height, window->window_resize_callback_user_data);
            }
        }
    }

    static void _mouse_button_callback(GLFWwindow* const glfw_window, int const button, int const action, int const /* mods */) {
        // clang-format off
        static std::unordered_map<i32, Key> mouse_button_map({
            {GLFW_MOUSE_BUTTON_LEFT, Key::left_mouse_button},
            {GLFW_MOUSE_BUTTON_RIGHT, Key::right_mouse_button},
            {GLFW_MOUSE_BUTTON_MIDDLE, Key::middle_mouse_button},
            {GLFW_MOUSE_BUTTON_4, Key::thumb_mouse_button_1},
            {GLFW_MOUSE_BUTTON_5, Key::thumb_mouse_button_2},
            {GLFW_MOUSE_BUTTON_6, Key::mouse_button_6},
            {GLFW_MOUSE_BUTTON_7, Key::mouse_button_7},
            {GLFW_MOUSE_BUTTON_8, Key::mouse_button_8}
        });
        // clang-format on

        if (auto button_iter = mouse_button_map.find(button); button_iter != mouse_button_map.end()) {
            for (Window* const window: windowing.windows) {
                if (window->glfw_window == glfw_window && window->mouse_button_callback) {
                    window->mouse_button_callback(window, button_iter->second, action, window->mouse_button_callback_user_data);
                }
            }
        }
    }

    static void _cursor_pos_callback(GLFWwindow* const glfw_window, double const x, double const y) {
        for (Window* const window: windowing.windows) {
            if (window->glfw_window == glfw_window && window->cursor_pos_callback) {
                window->cursor_pos_callback(window, x, y, window->cursor_pos_callback_user_data);
            }
        }
    }

    static void _scroll_callback(GLFWwindow* const glfw_window, double const offset_x, double const offset_y) {
        for (Window* const window: windowing.windows) {
            if (window->glfw_window == glfw_window && window->scroll_callback) {
                window->scroll_callback(window, offset_x, offset_y, window->scroll_callback_user_data);
            }
        }
    }

    static void _key_callback(GLFWwindow* const glfw_window, int const key, int const /* scancode */, int const action, int const /* mods */) {
        static std::unordered_map<i32, Key> keyboard_button_map{
            {GLFW_KEY_A, Key::a},
            {GLFW_KEY_B, Key::b},
            {GLFW_KEY_C, Key::c},
            {GLFW_KEY_D, Key::d},
            {GLFW_KEY_E, Key::e},
            {GLFW_KEY_F, Key::f},
            {GLFW_KEY_G, Key::g},
            {GLFW_KEY_H, Key::h},
            {GLFW_KEY_I, Key::i},
            {GLFW_KEY_J, Key::j},
            {GLFW_KEY_K, Key::k},
            {GLFW_KEY_L, Key::l},
            {GLFW_KEY_M, Key::m},
            {GLFW_KEY_N, Key::n},
            {GLFW_KEY_O, Key::o},
            {GLFW_KEY_P, Key::p},
            {GLFW_KEY_Q, Key::q},
            {GLFW_KEY_R, Key::r},
            {GLFW_KEY_S, Key::s},
            {GLFW_KEY_T, Key::t},
            {GLFW_KEY_U, Key::u},
            {GLFW_KEY_V, Key::v},
            {GLFW_KEY_W, Key::w},
            {GLFW_KEY_X, Key::x},
            {GLFW_KEY_Y, Key::y},
            {GLFW_KEY_Z, Key::z},
            {GLFW_KEY_ESCAPE, Key::escape},
            {GLFW_KEY_LEFT_ALT, Key::left_alt},
            {GLFW_KEY_RIGHT_ALT, Key::right_alt},
            {GLFW_KEY_TAB, Key::tab},
            {GLFW_KEY_SPACE, Key::spacebar},
            {GLFW_KEY_LEFT_SHIFT, Key::left_shift},
            {GLFW_KEY_RIGHT_SHIFT, Key::right_shift},
            {GLFW_KEY_F1, Key::f1},
            {GLFW_KEY_F2, Key::f2},
            {GLFW_KEY_F3, Key::f3},
            {GLFW_KEY_F4, Key::f4},
            {GLFW_KEY_F5, Key::f5},
            {GLFW_KEY_F6, Key::f6},
            {GLFW_KEY_F7, Key::f7},
            {GLFW_KEY_F8, Key::f8},
            {GLFW_KEY_F9, Key::f9},
            {GLFW_KEY_F10, Key::f10},
            {GLFW_KEY_F11, Key::f11},
            {GLFW_KEY_F12, Key::f12},
            {GLFW_KEY_KP_0, Key::numpad_0},
            {GLFW_KEY_KP_1, Key::numpad_1},
            {GLFW_KEY_KP_2, Key::numpad_2},
            {GLFW_KEY_KP_3, Key::numpad_3},
            {GLFW_KEY_KP_4, Key::numpad_4},
            {GLFW_KEY_KP_5, Key::numpad_5},
            {GLFW_KEY_KP_6, Key::numpad_6},
            {GLFW_KEY_KP_7, Key::numpad_7},
            {GLFW_KEY_KP_8, Key::numpad_8},
            {GLFW_KEY_KP_9, Key::numpad_9},
            {GLFW_KEY_KP_DIVIDE, Key::numpad_slash},
            {GLFW_KEY_KP_DECIMAL, Key::numpad_period},
            {GLFW_KEY_KP_MULTIPLY, Key::numpad_asteriks},
            {GLFW_KEY_KP_SUBTRACT, Key::numpad_minus},
            {GLFW_KEY_KP_ADD, Key::numpad_plus},
            {GLFW_KEY_KP_ENTER, Key::numpad_enter},
            {GLFW_KEY_NUM_LOCK, Key::num_lock},
        };

        if (auto key_iter = keyboard_button_map.find(key); key_iter != keyboard_button_map.end()) {
            for (Window* const window: windowing.windows) {
                if (window->glfw_window == glfw_window && window->key_callback) {
                    window->key_callback(window, key_iter->second, action, window->key_callback_user_data);
                }
            }
        }
    }

    static void _joystick_callback(int const joy, int const joy_event) {
        if (windowing.joystick_callback) {
            windowing.joystick_callback(joy, joy_event, windowing.joystick_callback_user_data);
        }
    }

    Window* create_window(f32 const width, f32 const height, Window* const share, bool const create_context, bool const decorated) {
        ANTON_ASSERT(width > 0 && height > 0, "Window dimensions may not be 0");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, decorated);
        if (!create_context) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
        GLFWwindow* const glfw_window = glfwCreateWindow(width, height, "GameEngine", nullptr, share ? share->glfw_window : nullptr);
        glfwDefaultWindowHints();
        if (glfw_window) {
            glfwSetCursorPosCallback(glfw_window, _cursor_pos_callback);
            glfwSetFramebufferSizeCallback(glfw_window, _window_resize_callback);
            glfwSetMouseButtonCallback(glfw_window, _mouse_button_callback);
            glfwSetScrollCallback(glfw_window, _scroll_callback);
            glfwSetKeyCallback(glfw_window, _key_callback);
            glfwSetJoystickCallback(_joystick_callback);

            Window* window = new Window;
            window->glfw_window = glfw_window;
            window->window_resize_callback = nullptr;
            window->cursor_pos_callback = nullptr;
            window->mouse_button_callback = nullptr;
            window->scroll_callback = nullptr;
            window->key_callback = nullptr;
            window->window_resize_callback_user_data = nullptr;
            window->cursor_pos_callback_user_data = nullptr;
            window->mouse_button_callback_user_data = nullptr;
            window->scroll_callback_user_data = nullptr;
            window->key_callback_user_data = nullptr;
            windowing.windows.emplace_back(window);
            return window;
        } else {
            return nullptr;
        }
    }

    void destroy_window(Window* const window) {
        for (i64 i = 0; i < windowing.windows.size(); ++i) {
            if (windowing.windows[i] == window) {
                windowing.windows.erase_unsorted_unchecked(i);
                break;
            }
        }

        glfwDestroyWindow(window->glfw_window);
        delete window;
    }

    void set_cursor_mode(Window* const window, Cursor_Mode const mode) {
        switch (mode) {
        case Cursor_Mode::normal: {
            glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } break;

        case Cursor_Mode::hidden: {
            glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } break;

        case Cursor_Mode::captured: {
            glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        } break;

        case Cursor_Mode::locked: {
            glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } break;
        }
    }

    Cursor_Mode get_cursor_mode(Window* const window) {
        i32 const glfw_mode = glfwGetInputMode(window->glfw_window, GLFW_CURSOR);
        switch (glfw_mode) {
        case GLFW_CURSOR_NORMAL: {
            return Cursor_Mode::normal;
        } break;

        case GLFW_CURSOR_HIDDEN: {
            return Cursor_Mode::hidden;
        } break;

        case GLFW_CURSOR_DISABLED: {
            return Cursor_Mode::locked;
        } break;

        case GLFW_CURSOR_CAPTURED:
            return Cursor_Mode::captured;
        }

        return Cursor_Mode::normal;
    }

    void set_window_resize_callback(Window* const window, window_resize_function const cb, void* user_data) {
        window->window_resize_callback = cb;
        window->window_resize_callback_user_data = user_data;
    }

    void set_cursor_pos_callback(Window* const window, cursor_pos_function const cb, void* user_data) {
        window->cursor_pos_callback = cb;
        window->cursor_pos_callback_user_data = user_data;
    }

    void set_mouse_button_callback(Window* const window, mouse_button_function const cb, void* user_data) {
        window->mouse_button_callback = cb;
        window->mouse_button_callback_user_data = user_data;
    }

    void set_scroll_callback(Window* const window, scroll_function const cb, void* user_data) {
        window->scroll_callback = cb;
        window->scroll_callback_user_data = user_data;
    }

    void set_key_callback(Window* const window, key_function const cb, void* user_data) {
        window->key_callback = cb;
        window->key_callback_user_data = user_data;
    }

    void set_joystick_callback(joystick_function const cb, void* user_data) {
        windowing.joystick_callback = cb;
        windowing.joystick_callback_user_data = user_data;
    }

    Vector2 get_cursor_pos(Window* const window) {
        double x;
        double y;
        glfwGetCursorPos(window->glfw_window, &x, &y);
        return Vector2(x, y);
    }

    bool should_close(Window* const window) {
        return glfwWindowShouldClose(window->glfw_window);
    }

    void resize(Window* const window, f32 width, f32 height) {
        glfwSetWindowSize(window->glfw_window, width, height);
    }

    f32 get_width(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        glfwGetWindowSize(window->glfw_window, &width, &height);
        return height;
    }

    f32 get_height(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        glfwGetWindowSize(window->glfw_window, &width, &height);
        return height;
    }

    Vector2 get_window_size(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        glfwGetWindowSize(window->glfw_window, &width, &height);
        return {(f32)width, (f32)height};
    }

    Vector2 get_window_pos(Window* const window) {
        i32 x, y;
        glfwGetWindowPos(window->glfw_window, &x, &y);
        return {(f32)x, (f32)y};
    }

    void set_window_pos(Window* const window, Vector2 const pos) {
        glfwSetWindowPos(window->glfw_window, pos.x, pos.y);
    }

    void set_opacity(Window* const window, float const opacity) {
        glfwSetWindowOpacity(window->glfw_window, opacity);
    }

    void focus_window(Window* const window) {
        glfwFocusWindow(window->glfw_window);
    }

    void make_context_current(Window* const window) {
        glfwMakeContextCurrent(window->glfw_window);
    }

    void swap_buffers(Window* const window) {
        glfwSwapBuffers(window->glfw_window);
    }

    void enable_vsync(bool const enabled) {
        glfwSwapInterval(enabled);
    }
} // namespace anton_engine::windowing
