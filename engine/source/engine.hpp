#ifndef ENGINE_HPP_INCLUDE
#define ENGINE_HPP_INCLUDE

#include "subsystem_forward_declarations.hpp"

class Engine {
private:
    static Input::Manager* input_manager;
    static renderer::Renderer* renderer;
    static Time_Core* time_core;
    static Component_System* component_system;
    static Window* main_window;
    static Mesh_Manager* mesh_manager;
    static Shader_Manager* shader_manager;
    static Entity_Manager* entity_manager;

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
    static Shader_Manager& get_shader_manager();
    static Entity_Manager& get_entity_manager();
};

#endif // !ENGINE_HPP_INCLUDE