#ifndef ENGINE_TIME_TIME_CORE_HPP_INCLUDE
#define ENGINE_TIME_TIME_CORE_HPP_INCLUDE

#include <cstdint>

class Time_Core {
public:
    // The number of frames since the start of the game
    uint64_t frame_count = 0;
    // The time it took to complete the previous frame
    double delta_time = 0.016667f;
    // Scale independent time it took to complete the previous frame
    double unscaled_delta_time = 0.016667f;
    double time_scale = 1.0f;
    // The time at the beginning of the current frame
    double frame_time = 0;
    // The time at the beginning of the previous frame
    double previous_frame_time = 0;
    // Scale independent time at the beginning of the current frame
    double unscaled_frame_time = 0;
    // Scale independent time at the beginning of the previous frame
    double unscaled_previous_frame_time = 0;

    void update_time();
};

#endif // !ENGINE_TIME_TIME_CORE_HPP_INCLUDE