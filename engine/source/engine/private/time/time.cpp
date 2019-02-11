#include "time.hpp"
#include "time_core.hpp"
#include "engine.hpp"

float time::get_delta_time() {
    return Engine::get_time_manager().delta_time;
}

float time::get_time() {
    return Engine::get_time_manager().frame_time;
}

float time::get_unscaled_delta_time() {
    return Engine::get_time_manager().unscaled_delta_time;
}

float time::get_unscaled_time() {
    return Engine::get_time_manager().unscaled_frame_time;
}

uint64_t time::get_frame_count() {
    return Engine::get_time_manager().frame_count;
}