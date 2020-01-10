#include <editor_events.hpp>

namespace anton_engine::editor_events {
    void entity_added(Entity const entity) {}

    void entity_selected(Entity const entity, bool const clear_previous_selection) {}

    void entity_deselected(Entity const entity) {}
} // namespace anton_engine::editor_events
