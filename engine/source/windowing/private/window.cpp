#include <window.hpp>

#include <opengl.hpp>
// GLAD must be before GLFW
// Or define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <build_config.hpp>
#include <debug_macros.hpp>
#include <stdexcept>

void framebuffer_size_callback(GLFWwindow* const, int width, int height);
void mouse_button_callback(GLFWwindow* const, int button, int action, int mods);
void mouse_position_callback(GLFWwindow* const, double param_x, double param_y);
void scroll_callback(GLFWwindow* const, double offset_x, double offset_y);
void keyboard_callback(GLFWwindow* const, int, int, int, int);
void joystick_config_callback(int, int);
void process_gamepad_input();

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

    opengl::load_functions();
    opengl::load_constants();

    opengl::viewport(0, 0, window_width, window_height);
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
    process_gamepad_input();
}

void Window::lock_cursor() const {
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::unlock_cursor() const {
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Vector2 Window::get_cursor_position() const {
    double x, y;
    glfwGetCursorPos(window_handle, &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
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