#ifndef ENGINE_TIME_HPP_INCLUDE
#define ENGINE_TIME_HPP_INCLUDE

#include <cstdint>
#include <macro_undefs.hpp>

// "time" collides with time.h's time function. I have no idea what a better name for this namespace could be
namespace timing {
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
}; // namespace timing

// Time, but with floats instead of doubles
namespace timingf {
    // The time it took to complete the previous frame
    float get_delta_time();

    // Time scale independent time it took to complete the previous frame
    float get_unscaled_delta_time();

    // Time since the start of the game
    float get_time();

    // Time scale independent time since the start of the game
    float get_unscaled_time();

    // The number of frames since the start of the game
    using timing::get_frame_count;
} // namespace timingf

#endif // !ENGINE_TIME_HPP_INCLUDE
