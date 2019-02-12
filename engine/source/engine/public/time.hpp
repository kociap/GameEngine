#ifndef ENGINE_TIME_HPP_INCLUDE
#define ENGINE_TIME_HPP_INCLUDE

#include <cstdint>

namespace Time {
    // The time it took to complete the previous frame
    float get_delta_time();

    // Time scale independent time it took to complete the previous frame
    float get_unscaled_delta_time();

    // Time since the start of the game
    float get_time();

    // Time scale independent time since the start of the game
    float get_unscaled_time();

    // The number of frames since the start of the game
    uint64_t get_frame_count();
}; // namespace Time

#endif // !ENGINE_TIME_HPP_INCLUDE