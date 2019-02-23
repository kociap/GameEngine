#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include "key.hpp"

namespace Input {
    float get_axis(std::string const& axis);
    float get_axis_raw(std::string const& axis);
    float get_action(std::string const& action);
}; // namespace Input

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE