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

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
void mouse_position_callback(GLFWwindow*, double param_x, double param_y);
void scroll_callback(GLFWwindow*, double offset_x, double offset_y);
void keyboard_callback(GLFWwindow*, int, int, int, int);

Window::Window(uint32_t width, uint32_t height): window_width(width), window_height(height) {
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
    Input_Manager& input_manager = Engine::get_input_manager();
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

    Input_Manager& input_manager = Engine::get_input_manager();
    input_manager.input_event_queue.emplace_back(Key::mouse_x, offset_x);
    input_manager.input_event_queue.emplace_back(Key::mouse_y, offset_y);
}

void scroll_callback(GLFWwindow*, double offset_x, double offset_y) {
    Input_Manager& input_manager = Engine::get_input_manager();
    input_manager.input_event_queue.emplace_back(Key::mouse_scroll, static_cast<float>(offset_y));
}

void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
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
    });
    // clang-format on

    Key mapped_key = keyboard_button_map.at(key);
    float value = static_cast<float>(action); // GLFW_PRESS is 1, GLFW_RELEASE is  0
    Input_Manager& input_manager = Engine::get_input_manager();
    input_manager.input_event_queue.emplace_back(mapped_key, value);
}