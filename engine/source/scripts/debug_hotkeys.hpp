#ifndef SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
#define SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE

class Debug_Hotkeys {
public:
    static void update(Debug_Hotkeys&);

    float reload_old = 0.0f;
    float show_shadow_map_old = 0.0f;
    float swap_fxaa_old = 0.0f;
};

#endif // !SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
