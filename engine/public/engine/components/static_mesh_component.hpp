#pragma once

#include <core/class_macros.hpp>
#include <core/handle.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
  class Mesh;
  class Shader;
  class Material;

  class COMPONENT Static_Mesh_Component {
  public:
    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
    Handle<Material> material_handle;
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(::anton_engine::Static_Mesh_Component)
