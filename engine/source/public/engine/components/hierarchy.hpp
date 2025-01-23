#pragma once

#include <anton/fixed_array.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/serialization.hpp>
#include <engine/ecs/entity.hpp>

namespace anton_engine {
  class COMPONENT Hierarchy {
  public:
    Entity parent;
    // TODO: Right now supports only 16 children.
    anton::Fixed_Array<Entity, 16> children;
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Hierarchy)
