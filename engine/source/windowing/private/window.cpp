#include <window.hpp>

#include <anton_assert.hpp>
#include <diagnostic_macros.hpp>
#include <exception.hpp>
#include <glfw.hpp>
#include <window_input.hpp>

namespace anton_engine {
    // Window::Window(i32 width, i32 height, bool decorated): window_width(width), window_height(height) {
    //     ANTON_ASSERT(window_width > 0 && window_height > 0, "Window dimensions may not be 0");

    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    //     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //     glfwWindowHint(GLFW_DECORATED, decorated);
    //     window_handle = glfwCreateWindow(window_width, window_height, "GameEngine", nullptr, nullptr);
    //     if (!window_handle) {
    //         throw Exception("GLFW failed to create a window");
    //     }
    //     glfwDefaultWindowHints();

    //     install_input_callbacks(window_handle);

    //     make_context_current();
    //     // TODO: Move out of the window.
    //     opengl::load();
    //     rendering::setup_rendering();
    //     load_builtin_shaders();

    //     glViewport(0, 0, window_width, window_height);
    // }

    Window* create_window(i32 width, i32 height, bool decorated) {
        ANTON_ASSERT(width > 0 && height > 0, "Window dimensions may not be 0");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DECORATED, decorated);
        GLFWwindow* const window = glfwCreateWindow(width, height, "GameEngine", nullptr, nullptr);
        glfwDefaultWindowHints();
        if (window) {
            install_input_callbacks(window);
            return reinterpret_cast<Window*>(window);
        } else {
            return nullptr;
        }
    }

    void destroy_window(Window* const window) {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(window));
    }

    void set_cursor_mode(Window* const window, Cursor_Mode const mode) {
        switch (mode) {
        case Cursor_Mode::normal: {
            glfwSetInputMode(reinterpret_cast<GLFWwindow*>(window), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } break;

        case Cursor_Mode::hidden: {
            glfwSetInputMode(reinterpret_cast<GLFWwindow*>(window), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } break;

        case Cursor_Mode::captured: {
            glfwSetInputMode(reinterpret_cast<GLFWwindow*>(window), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        } break;

        case Cursor_Mode::locked: {
            glfwSetInputMode(reinterpret_cast<GLFWwindow*>(window), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } break;
        }
    }

    Cursor_Mode get_cursor_mode(Window* const window) {
        i32 const glfw_mode = glfwGetInputMode(reinterpret_cast<GLFWwindow*>(window), GLFW_CURSOR);
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

    bool should_close(Window* const window) {
        return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window));
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

    void resize(Window* const window, i32 width, i32 height) {
        glfwSetWindowSize(reinterpret_cast<GLFWwindow*>(window), width, height);
    }

    i32 get_width(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(window), &width, &height);
        return height;
    }

    i32 get_height(Window* const window) {
        i32 width = 0;
        i32 height = 0;
        glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(window), &width, &height);
        return height;
    }

    Dimensions get_window_size(Window* const window) {
        Dimensions dims = {};
        glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(window), &dims.width, &dims.height);
        return dims;
    }

    void set_opacity(Window* const window, float const opacity) {
        glfwSetWindowOpacity(reinterpret_cast<GLFWwindow*>(window), opacity);
    }

    void make_context_current(Window* const window) {
        glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(window));
    }

    void swap_buffers(Window* const window) {
        glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window));
    }

    void enable_vsync(bool const enabled) {
        glfwSwapInterval(enabled);
    }
} // namespace anton_engine
