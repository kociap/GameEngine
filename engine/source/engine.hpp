#ifndef ENGINE_HPP_INCLUDE
#define ENGINE_HPP_INCLUDE

#include <subsystem_forward_declarations.hpp>

class Engine {
private:
    static Input::Manager* input_manager;
    static rendering::Renderer* renderer;
    static Time_Core* time_core;
    static ECS* ecs;
    static Window* main_window;
    static Resource_Manager<Mesh>* mesh_manager;
    static Resource_Manager<Shader>* shader_manager;
    static Resource_Manager<Material>* material_manager;

public:
    static void init();
    static void terminate();

    static void loop();
    static bool should_close();

    // TODO Temporary
    static void load_world();

    static Input::Manager& get_input_manager();
    static Window& get_window();
    static rendering::Renderer& get_renderer();
    static ECS& get_ecs();
    static Resource_Manager<Mesh>& get_mesh_manager();
    static Time_Core& get_time_manager();
    static Resource_Manager<Shader>& get_shader_manager();
    static Resource_Manager<Material>& get_material_manager();
};

#endif // !ENGINE_HPP_INCLUDE
