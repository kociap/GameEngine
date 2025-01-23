#pragma once

#include <core/types.hpp>

namespace anton_engine::threads {
  struct Timespec {
    i64 seconds;
    i64 nanoseconds;
  };

  void sleep(Timespec const& duration);
} // namespace anton_engine::threads
