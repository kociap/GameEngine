#ifndef EDITOR_HPP_INCLUDE
#define EDITOR_HPP_INCLUDE

#include <cstdint>
#include <editor_subsystem_fwd.hpp>

class Editor {
private:
    static Editor_Window* editor_window;
    static Editor_QApplication* qapplication;
    static Resource_Manager<Mesh>* mesh_manager;
    static Resource_Manager<Shader>* shader_manager;
    static Resource_Manager<Material>* material_manager;
    static Input::Manager* input_manager;
    static ECS* ecs;

    static bool close;

public:
    static void init();
    static void terminate();
    static void loop();
    static bool should_close();
    static void quit();

    static Resource_Manager<Mesh>& get_mesh_manager();
    static Resource_Manager<Shader>& get_shader_manager();
    static Resource_Manager<Material>& get_material_manager();
    static Input::Manager& get_input_manager();
    static ECS& get_ecs();

    // Because why not
    static void load_world();
};

#endif // !EDITOR_HPP_INCLUDE
