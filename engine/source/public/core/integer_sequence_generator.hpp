#pragma once

#include <core/types.hpp>

namespace anton_engine {
  class Integer_Sequence_Generator {
  public:
    Integer_Sequence_Generator(u64 start_number = 0): _next(start_number) {}

    u64 next()
    {
      return _next++;
    }

  private:
    u64 _next;
  };
} // namespace anton_engine
