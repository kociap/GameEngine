#include <window.hpp>

#include <anton_assert.hpp>
#include <build_config.hpp>
#include <builtin_shaders.hpp>
#include <diagnostic_macros.hpp>
#include <glad.hpp>
#include <glfw.hpp>
#include <opengl.hpp>
#include <renderer.hpp>
#include <window_input.hpp>

#include <stdexcept>

namespace anton_engine {
    Window::Window(int32_t width, int32_t height): window_width(width), window_height(height) {
        ANTON_ASSERT(window_width > 0 && window_height > 0, "Window dimensions may not be 0");

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_handle = glfwCreateWindow(window_width, window_height, "GameEngine", nullptr, nullptr);
        if (!window_handle) {
            throw std::runtime_error("GLFW failed to create a window");
        }

        glfwMakeContextCurrent(window_handle);

        install_input_callbacks(window_handle);

        // TODO: Move out of the window.
        opengl::load();
        rendering::setup_rendering();
        load_builtin_shaders();

        glViewport(0, 0, window_width, window_height);

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

    void Window::resize(int32_t width, int32_t height) {
        window_width = width;
        window_height = height;
    }

    int32_t Window::width() const {
        return window_width;
    }

    int32_t Window::height() const {
        return window_height;
    }
} // namespace anton_engine
