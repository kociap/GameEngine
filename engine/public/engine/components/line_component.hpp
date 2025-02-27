#pragma once

#include <core/class_macros.hpp>
#include <core/color.hpp>
#include <core/handle.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
  class Shader;
  class Mesh;
  class Material;

  class COMPONENT Line_Component {
  public:
    Color color = Color::white;
    float thickness = 1;

    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
    Handle<Material> material_handle;
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Line_Component)
