#include "window.hpp"

#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "debug_macros.hpp"
#include "engine.hpp"
#include "input/input_core.hpp"
#include "key.hpp"

#include <stdexcept>
#include <unordered_map>

// TODO DEBUG remove
#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
void mouse_position_callback(GLFWwindow*, double param_x, double param_y);
void scroll_callback(GLFWwindow*, double offset_x, double offset_y);
void keyboard_callback(GLFWwindow*, int, int, int, int);
void joystick_config_callback(int, int);

Window::Window(uint32_t width, uint32_t height) : window_width(width), window_height(height) {
    GE_assert(window_width > 0 && window_height > 0, "Window dimensions may not be 0");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_handle = glfwCreateWindow(window_width, window_height, "GameEngine", NULL, NULL);
    if (!window_handle) {
        throw std::runtime_error("GLFW failed to create a window");
    }

    glfwMakeContextCurrent(window_handle);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwTerminate();
        throw std::runtime_error("GLAD not initialized");
    }

    glViewport(0, 0, window_width, window_height);
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_handle, mouse_position_callback);
    glfwSetFramebufferSizeCallback(window_handle, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window_handle, mouse_button_callback);
    glfwSetScrollCallback(window_handle, scroll_callback);
    glfwSetKeyCallback(window_handle, keyboard_callback);
    glfwSetJoystickCallback(joystick_config_callback);

    // VSync
    glfwSwapInterval(1);
}

Window::~Window() {
    glfwTerminate();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window_handle);
}

void Window::swap_buffers() const {
    glfwSwapBuffers(window_handle);
}

void Window::poll_events() const {
    glfwPollEvents();

    // Gamepad input
    Input::Manager& input_manager = Engine::get_input_manager();
    for (int32_t joystick_index = GLFW_JOYSTICK_1; joystick_index < GLFW_JOYSTICK_LAST; ++joystick_index) {
        if (glfwJoystickPresent(joystick_index) && glfwJoystickIsGamepad(joystick_index)) {
            int count;
            float const* axes = glfwGetJoystickAxes(joystick_index, &count);
            // Xbox controller mapping for windows build
            input_manager.gamepad_stick_event_queue.emplace_back(joystick_index, Key::gamepad_left_stick_x_axis, axes[0]);
            input_manager.gamepad_stick_event_queue.emplace_back(joystick_index, Key::gamepad_left_stick_y_axis, axes[1]);
            input_manager.gamepad_stick_event_queue.emplace_back(joystick_index, Key::gamepad_right_stick_x_axis, axes[2]);
            input_manager.gamepad_stick_event_queue.emplace_back(joystick_index, Key::gamepad_right_stick_y_axis, axes[3]);
            // Triggers on windows have range -1 (released), 1 (pressed)
            // Remap to 0 (released), 1 (pressed)
            input_manager.gamepad_event_queue.emplace_back(joystick_index, Key::gamepad_left_trigger, (axes[4] + 1.0f) / 2.0f);
            input_manager.gamepad_event_queue.emplace_back(joystick_index, Key::gamepad_right_trigger, (axes[5] + 1.0f) / 2.0f);

            // TODO add mapping for linux build
            // TODO add other controllers if I ever buy them

            /*unsigned char const* buttons = glfwGetJoystickButtons(joystick_index, &count);
            for (int32_t i = 0; i < count; ++i) {
                std::cout << i << ": " << buttons[i] << "\n";
            }*/
        }
    }
}

void Window::resize(uint32_t width, uint32_t height) {
    window_width = width;
    window_height = height;
}

uint32_t Window::width() const {
    return window_width;
}

uint32_t Window::height() const {
    return window_height;
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
    Engine::get_window().resize(width, height);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods) {
    // clang-format off
    static std::unordered_map<int32_t, Key> mouse_button_map({
        {GLFW_MOUSE_BUTTON_1, Key::mouse_0},
        {GLFW_MOUSE_BUTTON_2, Key::mouse_1},
        {GLFW_MOUSE_BUTTON_3, Key::mouse_2},
        {GLFW_MOUSE_BUTTON_4, Key::mouse_3},
        {GLFW_MOUSE_BUTTON_5, Key::mouse_4},
        {GLFW_MOUSE_BUTTON_6, Key::mouse_5},
        {GLFW_MOUSE_BUTTON_7, Key::mouse_6},
        {GLFW_MOUSE_BUTTON_8, Key::mouse_7}
    });
    // clang-format on

    GE_assert(action != GLFW_REPEAT, "Mouse repeat happened"); // For debugging

    Key key = mouse_button_map.at(button);
    float value = static_cast<float>(action); // GLFW_PRESS is 1, GLFW_RELEASE is  0
    Input::Manager& input_manager = Engine::get_input_manager();
    input_manager.input_event_queue.emplace_back(key, value);
}

void mouse_position_callback(GLFWwindow*, double param_x, double param_y) {
    static float last_x = Engine::get_window().height() / 2.0f;
    static float last_y = Engine::get_window().width() / 2.0f;

    float x = static_cast<float>(param_x);
    float y = static_cast<float>(param_y);
    float offset_x = x - last_x;
    float offset_y = last_y - y;
    last_x = x;
    last_y = y;

    Input::Manager& input_manager = Engine::get_input_manager();
    input_manager.mouse_event_queue.emplace_back(offset_x, offset_y, 0);
}

void scroll_callback(GLFWwindow*, double offset_x, double offset_y) {
    Input::Manager& input_manager = Engine::get_input_manager();
    std::cout << "scroll: " << offset_y << "\n";
    input_manager.mouse_event_queue.emplace_back(0, 0, static_cast<float>(offset_y));
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // clang-format off
    static std::unordered_map<int32_t, Key> keyboard_button_map({
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
    });
    // clang-format on

    Key mapped_key = keyboard_button_map.at(key);

    // Temporary for debugging
    if (mapped_key == Key::escape)
        glfwSetWindowShouldClose(window, true);

    if (action != GLFW_REPEAT) {
        float value = static_cast<float>(action); // GLFW_PRESS is 1, GLFW_RELEASE is 0
        Input::Manager& input_manager = Engine::get_input_manager();
        input_manager.input_event_queue.emplace_back(mapped_key, value);
    }
}

void joystick_config_callback(int joy, int joy_event) {
    if (joy_event == GLFW_CONNECTED && glfwJoystickIsGamepad(joy)) {
        std::string joy_name(glfwGetJoystickName(joy));
        std::cout << "Gamepad connected: " << joy_name << " " << GLFW_JOYSTICK_1 << " " << joy << "\n";
    }
}