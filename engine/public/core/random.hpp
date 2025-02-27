#pragma once

#include <core/types.hpp>

namespace anton_engine {
  i64 random_i64(i64 min, i64 max);
  f32 random_f32(f32 min, f32 max);
  f64 random_f64(f64 min, f64 max);

  void seed_default_random_engine(u64);
} // namespace anton_engine
