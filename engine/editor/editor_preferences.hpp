#ifndef EDITOR_EDITOR_PREFERENCES_HPP_INCLUDE
#define EDITOR_EDITOR_PREFERENCES_HPP_INCLUDE

#include <color.hpp>

namespace anton_engine {
    u32 constexpr static grid_axis_x = 1 << 0;
    u32 constexpr static grid_axis_y = 1 << 1;
    u32 constexpr static grid_axis_z = 1 << 2;
    u32 constexpr static grid_enabled = 1 << 5;

    class Grid_Settings {
    public:
        Color axis_x_color;
        Color axis_y_color;
        Color axis_z_color;
        u32 grid_flags;
    };

    class Gizmo_Settings {
    public:
        Color axis_x_color;
        Color axis_y_color;
        Color axis_z_color;
        // Size in pixels
        u32 size;
    };

    class Editor_Preferences {
    public:
        Grid_Settings grid_settings;
        Gizmo_Settings gizmo_settings;
    };

    [[nodiscard]] Editor_Preferences& get_editor_preferences();
    void load_default_editor_preferences();
} // namespace anton_engine

#endif // !EDITOR_EDITOR_PREFERENCES_HPP_INCLUDE
