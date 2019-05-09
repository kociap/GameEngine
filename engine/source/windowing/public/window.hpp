#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

#include "math/vector2.hpp"
#include <cstdint>

struct GLFWwindow;

class Window {
public:
    Window(uint32_t width, uint32_t height);
    Window(Window const&) = delete;
    Window(Window&&) noexcept = default;
    Window& operator=(Window const&) = delete;
    Window& operator=(Window&&) noexcept = default;
    ~Window();

    bool should_close() const;
    void poll_events() const;
    void swap_buffers() const;
    void lock_cursor() const;
    void unlock_cursor() const;

    // Retrieves the position of the cursor relative to the window's top-left corner
    [[nodiscard]] Vector2 get_cursor_position() const;

    void resize(uint32_t width, uint32_t height);
    [[nodiscard]] uint32_t width() const;
    [[nodiscard]] uint32_t height() const;

private:
    uint32_t window_width = 0;
    uint32_t window_height = 0;
    GLFWwindow* window_handle = nullptr;
};

#endif // !WINDOWING_WINDOW_HPP_INCLUDE