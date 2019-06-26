#ifndef SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
#define SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE

#include <class_macros.hpp>
#include <serialization.hpp>

class COMPONENT Debug_Hotkeys {
public:
    bool cursor_captured = false;

    static void update(Debug_Hotkeys&);
};

template <>
struct serialization::use_default_deserialize<Debug_Hotkeys>: std::true_type {};

#endif // !SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
