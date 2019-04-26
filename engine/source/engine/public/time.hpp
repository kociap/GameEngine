#ifndef ENGINE_TIME_HPP_INCLUDE
#define ENGINE_TIME_HPP_INCLUDE

#include <cstdint>

namespace Time {
    // The time it took to complete the previous frame
    double get_delta_time();

    // Time scale independent time it took to complete the previous frame
    double get_unscaled_delta_time();

    // Time since the start of the game
    double get_time();

    // Time scale independent time since the start of the game
    double get_unscaled_time();

    // The number of frames since the start of the game
    uint64_t get_frame_count();
}; // namespace Time

// Time, but with floats instead of doubles
namespace Timef {
    // The time it took to complete the previous frame
    float get_delta_time();

    // Time scale independent time it took to complete the previous frame
    float get_unscaled_delta_time();

    // Time since the start of the game
    float get_time();

    // Time scale independent time since the start of the game
    float get_unscaled_time();

    // The number of frames since the start of the game
    using Time::get_frame_count;
} // namespace Timef

#endif // !ENGINE_TIME_HPP_INCLUDE