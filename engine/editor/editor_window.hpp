#ifndef EDITOR_EDITOR_WINDOW_HPP_INCLUDE
#define EDITOR_EDITOR_WINDOW_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <ecs/entity.hpp>

namespace anton_engine {
    class Editor_Shared_State {
    public:
        anton_stl::Vector<Entity> selected_entities;
        int32_t active_editor = 0;
    };

    class Editor_Window {
    public:
        Editor_Window();
        ~Editor_Window();

        void update();
        void render();

        Editor_Shared_State shared_state;
    };
} // namespace anton_engine

#endif // !EDITOR_EDITOR_WINDOW_HPP_INCLUDE
