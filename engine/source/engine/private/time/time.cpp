#include "time.hpp"
#include "engine.hpp"
#include "time_core.hpp"

float Time::get_delta_time() {
    return Engine::get_time_manager().delta_time;
}

float Time::get_time() {
    return Engine::get_time_manager().frame_time;
}

float Time::get_unscaled_delta_time() {
    return Engine::get_time_manager().unscaled_delta_time;
}

float Time::get_unscaled_time() {
    return Engine::get_time_manager().unscaled_frame_time;
}

uint64_t Time::get_frame_count() {
    return Engine::get_time_manager().frame_count;
}