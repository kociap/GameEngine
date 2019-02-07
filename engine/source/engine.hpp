#ifndef ENGINE_HPP_INCLUDE
#define ENGINE_HPP_INCLUDE

class Input_Manager;
class Time_Core;
class Component_System;
class Window;
namespace renderer {
    class Renderer;
}

class Engine {
private:
    static Input_Manager* input_manager;
    static renderer::Renderer* renderer;
    static Time_Core* time_core;
    static Component_System* component_system;
    static Window* main_window; // Temporary hack until I create window class

public:
    static void init(int argc, char** argv);
    static void terminate();

    static void loop();
    static bool should_close();

    static Input_Manager& get_input_manager();
    static Window& get_window();
    static renderer::Renderer& get_renderer();
    static Component_System& get_component_system();

private:
    static void update_components();
};

#endif // !ENGINE_HPP_INCLUDE