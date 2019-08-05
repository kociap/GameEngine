#include <time/time_core.hpp>

#include <time.hpp>
#include <time/time_platform.hpp>

namespace time_core {
    // The number of frames since the start of the game
    static uint64_t frame_count = 0;
    // The time it took to complete the previous frame
    static double delta_time = 0.016667f;
    // Scale independent time it took to complete the previous frame
    static double unscaled_delta_time = 0.016667f;
    static double time_scale = 1.0f;
    // The time at the beginning of the current frame
    static double frame_time = 0;
    // The time at the beginning of the previous frame
    static double previous_frame_time = 0;
    // Scale independent time at the beginning of the current frame
    static double unscaled_frame_time = 0;
    // Scale independent time at the beginning of the previous frame
    static double unscaled_previous_frame_time = 0;

    // (private) Time since init was called.
    static double time_offset = 0;

    void init() {
        time_offset = get_time();
    }

    void update_time() {
        ++frame_count;
        double current_time = get_time() - time_offset;
        unscaled_delta_time = current_time - unscaled_frame_time;
        unscaled_previous_frame_time = unscaled_frame_time;
        unscaled_frame_time = current_time;
        delta_time = unscaled_delta_time * time_scale;
        previous_frame_time = frame_time;
        frame_time += delta_time;
    }
} // namespace time_core

namespace timing {
    double get_delta_time() {
        return time_core::delta_time;
    }

    double get_time() {
        return time_core::frame_time;
    }

    double get_unscaled_delta_time() {
        return time_core::unscaled_delta_time;
    }

    double get_unscaled_time() {
        return time_core::unscaled_frame_time;
    }

    uint64_t get_frame_count() {
        return time_core::frame_count;
    }
} // namespace timing

namespace timingf {
    float get_delta_time() {
        return static_cast<float>(time_core::delta_time);
    }

    float get_time() {
        return static_cast<float>(time_core::frame_time);
    }

    float get_unscaled_delta_time() {
        return static_cast<float>(time_core::unscaled_delta_time);
    }

    float get_unscaled_time() {
        return static_cast<float>(time_core::unscaled_frame_time);
    }
} // namespace timingf
