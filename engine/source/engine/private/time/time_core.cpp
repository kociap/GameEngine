#include "time_core.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

void Time_Core::update_time() {
    ++frame_count;
    float current_time = static_cast<float>(glfwGetTime());
    unscaled_delta_time = time_scale * (current_time - unscaled_frame_time);
    unscaled_previous_frame_time = unscaled_frame_time;
    unscaled_frame_time = current_time;
    delta_time = unscaled_delta_time * time_scale;
    previous_frame_time = frame_time;
    frame_time += delta_time;
}