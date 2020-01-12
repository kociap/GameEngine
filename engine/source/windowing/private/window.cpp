#include <window.hpp>

#include <anton_assert.hpp>
#include <builtin_shaders.hpp>
#include <diagnostic_macros.hpp>
#include <exception.hpp>
#include <glad.hpp>
#include <glfw.hpp>
#include <opengl.hpp>
#include <renderer.hpp>
#include <window_input.hpp>

namespace anton_engine {
    Window::Window(i32 width, i32 height, bool decorated): window_width(width), window_height(height) {
        ANTON_ASSERT(window_width > 0 && window_height > 0, "Window dimensions may not be 0");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, decorated);
        window_handle = glfwCreateWindow(window_width, window_height, "GameEngine", nullptr, nullptr);
        if (!window_handle) {
            throw Exception("GLFW failed to create a window");
        }
        glfwDefaultWindowHints();

        install_input_callbacks(window_handle);

        make_context_current();
        // TODO: Move out of the window.
        opengl::load();
        rendering::setup_rendering();
        load_builtin_shaders();

        glViewport(0, 0, window_width, window_height);
    }

    Window::~Window() {
        glfwDestroyWindow(window_handle);
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(window_handle);
    }

    void Window::swap_buffers() {
        glfwSwapBuffers(window_handle);
    }

    void Window::make_context_current() {
        glfwMakeContextCurrent(window_handle);
    }

    // void Window::lock_cursor() const {
    //     glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // }

    // void Window::unlock_cursor() const {
    //     glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // }

    // Vector2 Window::get_cursor_position() const {
    //     double x, y;
    //     glfwGetCursorPos(window_handle, &x, &y);
    //     return {static_cast<float>(x), static_cast<float>(y)};
    // }

    void Window::resize(i32 width, i32 height) {
        window_width = width;
        window_height = height;
    }

    i32 Window::width() const {
        return window_width;
    }

    i32 Window::height() const {
        return window_height;
    }

    void Window::set_opacity(float const opacity) {
        glfwSetWindowOpacity(window_handle, opacity);
    }
} // namespace anton_engine
