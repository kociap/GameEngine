#pragma once

#include <anton/string.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>
#include <core/serialization/types/string.hpp>

namespace anton_engine {
  class COMPONENT Entity_Name {
  public:
    anton::String name;
  };

  inline void serialize(serialization::Binary_Output_Archive& out,
                        Entity_Name const& name)
  {
    serialize(out, name.name);
  }

  inline void deserialize(serialization::Binary_Input_Archive& in,
                          Entity_Name& name)
  {
    deserialize(in, name.name);
  }
} // namespace anton_engine
