#ifndef EDITOR_EDITOR_MAIN_HPP_INCLUDE
#define EDITOR_EDITOR_MAIN_HPP_INCLUDE

#include "editor.hpp"
#include "engine.hpp"

inline int editor_main(int argc, char** argv) {
    Engine::init(argc, argv);
    Editor::init();
    while (!Editor::should_close()) {
        Editor::loop();
    }
    Editor::terminate();
    Engine::terminate();
    return 0;
}

#endif // !EDITOR_EDITOR_MAIN_HPP_INCLUDE