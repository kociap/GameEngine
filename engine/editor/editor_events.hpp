#ifndef EDITOR_EDITOR_EVENTS_HPP_INCLUDE
#define EDITOR_EDITOR_EVENTS_HPP_INCLUDE

#include <ecs/entity.hpp>

namespace anton_engine::editor_events {
    void entity_added(Entity);
    void entity_selected(Entity, bool clear_previous_selection);
    void entity_deselected(Entity);
} // namespace anton_engine::editor_events

#endif // !EDITOR_EDITOR_EVENTS_HPP_INCLUDE
