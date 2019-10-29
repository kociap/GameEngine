#include <editor_preferences.hpp>

namespace anton_engine {
    static Editor_Preferences editor_preferences;

    Editor_Preferences& get_editor_preferences() {
        return editor_preferences;
    }

    void load_default_editor_preferences() {
        editor_preferences.grid_settings.axis_x_color = {179.f / 255.f, 20.f / 255.f, 5.f / 255.f};
        editor_preferences.grid_settings.axis_y_color = {0.545f, 0.863f, 0.0f};
        // Color const axis_green = {0.1416f, 0.26953f, 0.02417f};
        // Color const axis_green = {0, 220.0f / 255.0f, 0};
        editor_preferences.grid_settings.axis_z_color = {15.f / 255.f, 77.f / 255.f, 186.f / 255.f};
        editor_preferences.grid_settings.grid_flags = grid_enabled | grid_axis_x | grid_axis_z;
    }
} // namespace anton_engine
