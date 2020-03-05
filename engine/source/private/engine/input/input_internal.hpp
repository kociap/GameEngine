#ifndef ENGINE_INPUT_INPUT_INTERNAL_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_INTERNAL_HPP_INCLUDE

#include <core/types.hpp>
#include <engine/key.hpp>

namespace anton_engine::input {
    void add_gamepad_event(i32 pad_index, Key, f32 value);
    void add_event(Key, f32 value);

    void process_events();
} // namespace anton_engine::input

#endif // !ENGINE_INPUT_INPUT_INTERNAL_HPP_INCLUDE
