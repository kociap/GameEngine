#ifndef EDITOR_HPP_INCLUDE
#define EDITOR_HPP_INCLUDE

namespace anton_engine {
    class Editor_Window;

    class ECS;
    namespace Input {
        class Manager;
    }

    class Shader;
    class Mesh;
    class Material;
    template <typename T>
    class Resource_Manager;
} // namespace anton_engine

namespace anton_engine {
    void quit();

    class Editor {
    public:
        static bool should_close();
        static void quit();

        static Resource_Manager<Mesh>& get_mesh_manager();
        static Resource_Manager<Shader>& get_shader_manager();
        static Resource_Manager<Material>& get_material_manager();
        static Input::Manager& get_input_manager();
        static ECS& get_ecs();
        static Editor_Window& get_window();
    };
} // namespace anton_engine

#endif // !EDITOR_HPP_INCLUDE
