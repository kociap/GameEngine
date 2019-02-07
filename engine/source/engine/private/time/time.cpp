#include "time.hpp"
#include "time_core.hpp"

extern Time_Core g_time;

float time::get_delta_time() {
    return g_time.delta_time;
}

float time::get_time() {
    return g_time.frame_time;
}

float time::get_unscaled_delta_time() {
    return g_time.unscaled_delta_time;
}

float time::get_unscaled_time() {
    return g_time.unscaled_frame_time;
}

uint64_t time::get_frame_count() {
    return g_time.frame_count;
}