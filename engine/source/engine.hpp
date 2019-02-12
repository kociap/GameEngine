#ifndef ENGINE_HPP_INCLUDE
#define ENGINE_HPP_INCLUDE

class Time_Core;
class Component_System;
class Window;
class Mesh_Manager;
namespace renderer {
    class Renderer;
}
namespace Input {
    class Manager;
}

class Game_Object;

#include <vector>

class Engine {
private:
    static Input::Manager* input_manager;
    static renderer::Renderer* renderer;
    static Time_Core* time_core;
    static Component_System* component_system;
    static Window* main_window;
    static Mesh_Manager* mesh_manager;

public:
    static void init(int argc, char** argv);
    static void terminate();

    static void loop();
    static bool should_close();

    static Input::Manager& get_input_manager();
    static Window& get_window();
    static renderer::Renderer& get_renderer();
    static Component_System& get_component_system();
    static Mesh_Manager& get_mesh_manager();
    static Time_Core& get_time_manager();

	static std::vector<Game_Object> game_objects;

private:
    static void update_components();
};

#endif // !ENGINE_HPP_INCLUDE