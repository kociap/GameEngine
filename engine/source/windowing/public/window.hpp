#ifndef WINDOWING_WINDOW_HPP_INCLUDE
#define WINDOWING_WINDOW_HPP_INCLUDE

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

    void resize(uint32_t width, uint32_t height);
    uint32_t width() const;
    uint32_t height() const;

private:
    uint32_t window_width = 0;
    uint32_t window_height = 0;
    GLFWwindow* window_handle = nullptr;
};

#endif // !WINDOWING_WINDOW_HPP_INCLUDE