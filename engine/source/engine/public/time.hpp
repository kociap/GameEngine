#ifndef ENGINE_TIME_HPP_INCLUDE
#define ENGINE_TIME_HPP_INCLUDE

#include <cstdint>

namespace time {
    // The time it took to complete the previous frame
    static float get_delta_time();
    // Time scale independent time it took to complete the previous frame
    static float get_unscaled_delta_time();
    // Time since the start of the game
    static float get_time();
    // Time scale independent time since the start of the game
    static float get_unscaled_time();
    // The number of frames since the start of the game
    static uint64_t get_frame_count();
}; // namespace time

#endif // !ENGINE_TIME_HPP_INCLUDE