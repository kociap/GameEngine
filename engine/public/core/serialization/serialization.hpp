#pragma once

#include <anton/type_traits.hpp>
#include <core/serialization/archives/binary.hpp>

namespace anton_engine {
  template<typename T>
  struct use_default_serialize: anton::False_Type {};

  template<typename T>
  anton::enable_if<use_default_serialize<T>::value, void>
  serialize(serialization::Binary_Output_Archive& out, T const& obj)
  {
    out.write(obj);
  }

  template<typename T>
  anton::enable_if<use_default_serialize<T>::value, void>
  deserialize(serialization::Binary_Input_Archive& in, T& obj)
  {
    in.read(obj);
  }
} // namespace anton_engine

#define ANTON_DEFAULT_SERIALIZABLE(type)                     \
  namespace anton_engine {                                   \
    template<>                                               \
    struct use_default_serialize<type>: anton::True_Type {}; \
  }
