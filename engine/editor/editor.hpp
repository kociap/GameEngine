#ifndef EDITOR_HPP_INCLUDE
#define EDITOR_HPP_INCLUDE

#include "editor_subsystem_forward_declarations.hpp"
#include <cstdint>

class Editor {
private:
    static Level_Editor* level_editor;
    static Imgui_Renderer* imgui_renderer;

    static void draw_ui();

	static bool mouse_captured;

public:
    static void init();
    static void terminate();
    static void loop();
    static bool should_close();
    static void resize(uint32_t width, uint32_t height);
    static bool is_mouse_captured();
    static void set_mouse_captured(bool);
};

#endif // !EDITOR_HPP_INCLUDE
