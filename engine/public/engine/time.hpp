#pragma once

#include <core/types.hpp>

namespace anton_engine {
  struct System_Time {
    u16 year;
    // 1 through 12
    u16 month;
    // 1 through 31
    u16 day;
    // 0 (Sunday) through 6 (Saturday)
    u16 day_of_week;
    // 0 through 23
    u16 hour;
    // 0 through 59
    u16 minutes;
    // 0 through 59
    u16 seconds;
    // 0 through 999
    u16 milliseconds;
  };

  System_Time get_utc_system_time();
  System_Time get_local_system_time();

  // Returns time in seconds
  double get_time();
  double get_frame_start_time();

  // The time it took to complete the previous frame
  double get_delta_time();

  // Time scale independent time it took to complete the previous frame
  double get_unscaled_delta_time();

  // Time since the start of the game at the beginning of the frame
  double get_frame_time();

  // Time scale independent time since the start of the game at the beginning of the frame
  double get_unscaled_frame_time();

  // The number of frames since the start of the game
  u64 get_frame_count();
} // namespace anton_engine
