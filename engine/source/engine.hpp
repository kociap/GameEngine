#ifndef ENGINE_HPP_INCLUDE
#define ENGINE_HPP_INCLUDE

#include <subsystem_forward_declarations.hpp>

namespace anton_engine {
    class Framebuffer;
    class Window;
    class ECS;
    namespace rendering {
        class Renderer;
    }

    class Shader;
    class Mesh;
    class Material;
    template <typename T>
    class Resource_Manager;

    class Engine {
    private:
        static rendering::Renderer* renderer;
        static ECS* ecs;
        static Window* main_window;
        static Resource_Manager<Mesh>* mesh_manager;
        static Resource_Manager<Shader>* shader_manager;
        static Resource_Manager<Material>* material_manager;

        static Framebuffer* deferred_framebuffer;
        static Framebuffer* postprocess_front;
        static Framebuffer* postprocess_back;

    public:
        static void init();
        static void terminate();

        static void loop();
        static bool should_close();

        // TODO Temporary
        static void load_world();

        static Window& get_window();
        static rendering::Renderer& get_renderer();
        static ECS& get_ecs();
        static Resource_Manager<Mesh>& get_mesh_manager();
        static Resource_Manager<Shader>& get_shader_manager();
        static Resource_Manager<Material>& get_material_manager();
    };
} // namespace anton_engine

#endif // !ENGINE_HPP_INCLUDE
