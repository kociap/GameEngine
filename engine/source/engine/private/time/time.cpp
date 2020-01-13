#include <time/time_internal.hpp>

#include <time.hpp>

namespace anton_engine {
    // The number of frames since the start of the game
    static u64 frame_count = 0;
    // The time it took to complete the previous frame
    static double delta_time = 0.01666666;
    // Scale independent time it took to complete the previous frame
    static double unscaled_delta_time = 0.01666666;
    static double time_scale = 1.0;
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

    void init_time() {
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

    double get_delta_time() {
        return delta_time;
    }

    double get_frame_time() {
        return frame_time;
    }

    double get_unscaled_delta_time() {
        return unscaled_delta_time;
    }

    double get_unscaled_time() {
        return unscaled_frame_time;
    }

    u64 get_frame_count() {
        return frame_count;
    }
} // namespace anton_engine
