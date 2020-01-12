#include <windowing_internal.hpp>

#include <build_config.hpp>
#include <glfw.hpp>

namespace anton_engine {
    bool init_windowing() {
        if (!glfwInit()) {
            return false;
        }

        return true;
    }

    void terminate_windowing() {
        glfwTerminate();
    }

    void poll_events() {
        glfwPollEvents();
    }

    void enable_vsync(bool enabled) {
        glfwSwapInterval(enabled);
    }
} // namespace anton_engine
