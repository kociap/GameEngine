#ifndef LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE
#define LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE

class Framebuffer;

class Level_Editor {
public:
    Level_Editor();
    ~Level_Editor();

    void prepare_editor_ui();

    Framebuffer* framebuffer = nullptr;
};

#endif // !LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE
