#pragma once

namespace anton_engine {
  class ECS;
  class Shader;
  class Mesh;
  class Material;
  template<typename T>
  class Resource_Manager;
} // namespace anton_engine

namespace anton_engine {
  void quit();

  class Editor {
  public:
    static Resource_Manager<Mesh>& get_mesh_manager();
    static Resource_Manager<Shader>& get_shader_manager();
    static Resource_Manager<Material>& get_material_manager();
    static ECS& get_ecs();
  };
} // namespace anton_engine
