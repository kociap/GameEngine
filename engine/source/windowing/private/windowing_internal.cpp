#include <windowing_internal.hpp>

#include <build_config.hpp>
#include <glfw.hpp>

namespace anton_engine {
    bool init_windowing() {
        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        return true;
    }

    void terminate_windowing() {
        glfwTerminate();
    }
} // namespace anton_engine
