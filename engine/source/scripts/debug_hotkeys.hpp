#ifndef SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
#define SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE

#include <class_macros.hpp>
#include <serialization/serialization.hpp>

class COMPONENT Debug_Hotkeys {
public:
    bool cursor_captured = false;

    static void update(Debug_Hotkeys&);
};

DEFAULT_SERIALIZABLE(Debug_Hotkeys);
DEFAULT_DESERIALIZABLE(Debug_Hotkeys);

#endif // !SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
