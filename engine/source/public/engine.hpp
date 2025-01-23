#pragma once

namespace anton_engine {
  class Framebuffer;
  class ECS;
  namespace rendering {
    class Renderer;
  }

  class Shader;
  class Mesh;
  class Material;
  template<typename T>
  class Resource_Manager;

  class Engine {
  public:
    static rendering::Renderer& get_renderer();
    static ECS& get_ecs();
    static Resource_Manager<Mesh>& get_mesh_manager();
    static Resource_Manager<Shader>& get_shader_manager();
    static Resource_Manager<Material>& get_material_manager();
  };
} // namespace anton_engine
