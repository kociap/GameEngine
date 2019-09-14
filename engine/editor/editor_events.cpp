#include <editor_events.hpp>

#include <anton_stl/algorithm.hpp>
#include <editor.hpp>
#include <editor_window.hpp>
#include <outliner.hpp>

namespace anton_engine::editor_events {
    void entity_added(Entity const entity) {
        Editor_Window& editor = Editor::get_window();
        editor.outliner->add_entity(entity);
    }

    void entity_selected(Entity const entity, bool const clear_previous_selection) {
        Editor_Window& editor = Editor::get_window();
        anton_stl::Vector<Entity>& selected_entities = editor.shared_state.selected_entities;

        if (clear_previous_selection) {
            selected_entities.clear();
        }

        selected_entities.insert_unsorted(selected_entities.cbegin(), entity);
        editor.outliner->select_entities(selected_entities);
    }

    void entity_deselected(Entity const entity) {
        Editor_Window& editor = Editor::get_window();
        anton_stl::Vector<Entity>& selected_entities = editor.shared_state.selected_entities;

        auto iter = anton_stl::find(selected_entities.begin(), selected_entities.end(), entity);
        selected_entities.erase(iter, iter + 1);
        editor.outliner->deselect_entity(entity);
    }
} // namespace anton_engine::editor_events
