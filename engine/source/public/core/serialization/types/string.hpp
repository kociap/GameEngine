#pragma once

#include <anton/string.hpp>

#include <core/memory/stack_allocate.hpp>
#include <core/serialization/archives/binary.hpp>

namespace anton_engine {
  void serialize(serialization::Binary_Output_Archive& out,
                 anton::String const& str)
  {
    out.write(str.capacity());
    out.write(str.size_bytes());
    out.write_binary(str.data(), str.size_bytes());
  }

  void deserialize(serialization::Binary_Input_Archive& in, anton::String& str)
  {
    anton::String::size_type capacity;
    anton::String::size_type size;
    in.read(capacity);
    in.read(size);
    str.clear();
    str.ensure_capacity_exact(capacity);
    str.force_size(size);
    in.read_binary(str.data(), size);
  }
} // namespace anton_engine
