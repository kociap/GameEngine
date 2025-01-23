#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/memory.hpp>

namespace anton_engine {
  template<typename T>
  class Stack_Allocate {
  public:
    T& reference()
    {
      return *pointer();
    }

    T* pointer()
    {
      return reinterpret_cast<T*>(anton::launder(&buffer));
    }

    anton::Aligned_Buffer<sizeof(T), alignof(T)> buffer;
  };
} // namespace anton_engine
