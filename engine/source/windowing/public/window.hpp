#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include <anton_int.hpp>
#include <math/vector2.hpp>

struct GLFWwindow;

namespace anton_engine {
    class Window {
    public:
        Window(i32 width, i32 height, bool decorated);
        Window(Window const&) = delete;
        Window(Window&&) noexcept = default;
        Window& operator=(Window const&) = delete;
        Window& operator=(Window&&) noexcept = default;
        ~Window();

        bool should_close() const;
        void swap_buffers();
        void make_context_current();

        void resize(i32 width, i32 height);
        [[nodiscard]] i32 width() const;
        [[nodiscard]] i32 height() const;
        void set_opacity(float);

    private:
        i32 window_width = 0;
        i32 window_height = 0;
        GLFWwindow* window_handle = nullptr;
    };
} // namespace anton_engine

#endif // !WINDOWING_WINDOW_HPP_INCLUDE
