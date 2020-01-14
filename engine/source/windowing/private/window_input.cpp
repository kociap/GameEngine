#include <window_input.hpp>

#include <anton_stl/string.hpp>
#include <glfw.hpp>
#include <input/input_internal.hpp>
#include <key.hpp>
#include <logging.hpp>
#include <window.hpp>

#include <unordered_map>

namespace anton_engine {
    void framebuffer_size_callback(GLFWwindow* const window, int const width, int const height) {
        resize(reinterpret_cast<Window*>(window), width, height);
    }

    void mouse_button_callback(GLFWwindow* const, int const button, int const action, int const /* mods */) {
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

        Key key = mouse_button_map.at(button);
        float value = static_cast<float>(action); // GLFW_PRESS is 1, GLFW_RELEASE is  0
        input::add_event(key, value);
    }

    void mouse_position_callback(GLFWwindow* const window, double const param_x, double const param_y) {
        Vector2 const dimensions = get_window_size(reinterpret_cast<Window*>(window));
        // TODO: Per window?
        static float last_x = dimensions.x / 2.0f;
        static float last_y = dimensions.y / 2.0f;

        float x = static_cast<float>(param_x);
        float y = static_cast<float>(param_y);
        float offset_x = x - last_x;
        float offset_y = last_y - y;
        last_x = x;
        last_y = y;

        input::add_event(Key::mouse_x, offset_x);
        input::add_event(Key::mouse_y, offset_y);
    }

    void scroll_callback(GLFWwindow* const, double const /* offset_x */, double const offset_y) {
        input::add_event(Key::mouse_scroll, static_cast<float>(offset_y));
    }

    void keyboard_callback(GLFWwindow* const, int const key, int const /* scancode */, int const action, int const /* mods */) {
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

        if (auto key_iter = keyboard_button_map.find(key); action != GLFW_REPEAT && key_iter != keyboard_button_map.end()) {
            float value = static_cast<float>(action); // GLFW_PRESS is 1, GLFW_RELEASE is 0
            input::add_event(key_iter->second, value);
        }
    }

    void joystick_config_callback(int const joy, int const joy_event) {
        if (joy_event == GLFW_CONNECTED && glfwJoystickIsGamepad(joy)) {
            anton_stl::String joy_name(glfwGetJoystickName(joy));
            ANTON_LOG_INFO("Gamepad connected: " + joy_name + " " + anton_stl::to_string(joy));
        }
    }

    void process_gamepad_input() {
        // Gamepad input
        for (i32 joystick_index = GLFW_JOYSTICK_1; joystick_index < GLFW_JOYSTICK_LAST; ++joystick_index) {
            if (glfwJoystickPresent(joystick_index) && glfwJoystickIsGamepad(joystick_index)) {
                int count;
                float const* axes = glfwGetJoystickAxes(joystick_index, &count);
                // Xbox controller mapping for windows build
                input::add_gamepad_event(joystick_index, Key::gamepad_left_stick_x_axis, axes[0]);
                input::add_gamepad_event(joystick_index, Key::gamepad_left_stick_y_axis, axes[1]);
                input::add_gamepad_event(joystick_index, Key::gamepad_right_stick_x_axis, axes[2]);
                input::add_gamepad_event(joystick_index, Key::gamepad_right_stick_y_axis, axes[3]);
                // Triggers on windows have range -1 (released), 1 (pressed)
                // Remap to 0 (released), 1 (pressed)
                input::add_gamepad_event(joystick_index, Key::gamepad_left_trigger, (axes[4] + 1.0f) / 2.0f);
                input::add_gamepad_event(joystick_index, Key::gamepad_right_trigger, (axes[5] + 1.0f) / 2.0f);

                // TODO add mapping for linux build
                // TODO add other controllers if I ever buy them

                /*unsigned char const* buttons = glfwGetJoystickButtons(joystick_index, &count);
                for (i32 i = 0; i < count; ++i) {
                    std::cout << i << ": " << buttons[i] << "\n";
                }*/
            }
        }
    }

    void install_input_callbacks(GLFWwindow* window) {
        glfwSetCursorPosCallback(window, mouse_position_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetKeyCallback(window, keyboard_callback);
        glfwSetJoystickCallback(joystick_config_callback);
    }
} // namespace anton_engine
