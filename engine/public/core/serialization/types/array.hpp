#pragma once

#include <anton/array.hpp>

#include <core/memory/stack_allocate.hpp>
#include <core/serialization/archives/binary.hpp>

namespace anton_engine {
  template<typename T>
  void serialize(serialization::Binary_Output_Archive& out,
                 anton::Array<T> const& vec)
  {
    out.write(vec.capacity());
    out.write(vec.size());
    for(T const& elem: vec) {
      serialize(out, elem);
    }
  }

  template<typename T>
  void deserialize(serialization::Binary_Input_Archive& in,
                   anton::Array<T>& vec)
  {
    using size_type = typename anton::Array<T>::size_type;
    size_type capacity = 0;
    size_type size = 0;
    in.read(capacity);
    in.read(size);
    vec.clear();
    vec.set_capacity(capacity);
    if constexpr(anton::is_default_constructible<T>) {
      vec.resize(size);
      try {
        for(T& elem: vec) {
          deserialize(in, elem);
        }
      } catch(...) {
        // TODO: Move stream backwards to maintain weak guarantee
        anton::destruct_n(vec.data(), size);
        throw;
      }
    } else {
      size_type n = size;
      try {
        for(; n > 0; --n) {
          Stack_Allocate<T> elem;
          vec.push_back(ANTON_MOV(elem.reference()));
          deserialize(in, vec.back());
        }
        vec._size = size;
      } catch(...) {
        // TODO: Move stream backwards to maintain weak guarantee
        anton::destruct_n(vec.data(), size - n);
        throw;
      }
    }
  }
} // namespace anton_engine
