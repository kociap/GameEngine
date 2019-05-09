#ifndef SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
#define SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE

class Debug_Hotkeys {
public:
    bool cursor_captured = false;

    static void update(Debug_Hotkeys&);
};

#endif // !SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
