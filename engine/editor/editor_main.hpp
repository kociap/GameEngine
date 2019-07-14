#ifndef EDITOR_EDITOR_MAIN_HPP_INCLUDE
#define EDITOR_EDITOR_MAIN_HPP_INCLUDE

#include <editor.hpp>
#include <engine.hpp>

inline int editor_main() {
    Engine::init();
    Editor::init();
    while (!Editor::should_close()) {
        Editor::loop();
    }
    Editor::terminate();
    Engine::terminate();
    return 0;
}

#endif // !EDITOR_EDITOR_MAIN_HPP_INCLUDE