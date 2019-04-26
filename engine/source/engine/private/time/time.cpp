#include "time.hpp"
#include "engine.hpp"
#include "time_core.hpp"

namespace Time {
    double get_delta_time() {
        return Engine::get_time_manager().delta_time;
    }

    double get_time() {
        return Engine::get_time_manager().frame_time;
    }

    double get_unscaled_delta_time() {
        return Engine::get_time_manager().unscaled_delta_time;
    }

    double get_unscaled_time() {
        return Engine::get_time_manager().unscaled_frame_time;
    }

    uint64_t get_frame_count() {
        return Engine::get_time_manager().frame_count;
    }
} // namespace Time

namespace Timef {
    float get_delta_time() {
        return static_cast<float>(Engine::get_time_manager().delta_time);
    }

    float get_time() {
        return static_cast<float>(Engine::get_time_manager().frame_time);
    }

    float get_unscaled_delta_time() {
        return static_cast<float>(Engine::get_time_manager().unscaled_delta_time);
    }

    float get_unscaled_time() {
        return static_cast<float>(Engine::get_time_manager().unscaled_frame_time);
    }
} // namespace Time