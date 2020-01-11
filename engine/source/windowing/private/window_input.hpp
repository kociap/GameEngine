#ifndef WINDOWING_WINDOW_INPUT_HPP_INCLUDE
#define WINDOWING_WINDOW_INPUT_HPP_INCLUDE

struct GLFWwindow;

namespace anton_engine {
    void install_input_callbacks(GLFWwindow*);
    void process_gamepad_input();
} // namespace anton_engine

#endif // !WINDOWING_WINDOW_INPUT_HPP_INCLUDE
