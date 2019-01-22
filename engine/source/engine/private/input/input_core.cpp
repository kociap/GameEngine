#include "input/input_core.hpp"

void Input_Manager::process_events() {
    // Hardwire esc key to close the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}