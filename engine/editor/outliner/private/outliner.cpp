#include <outliner.hpp>

#include <anton_stl/algorithm.hpp>
#include <anton_stl/string.hpp>
#include <anton_stl/string_view.hpp>
#include <components/entity_name.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>

ANTON_DISABLE_WARNINGS()
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS()

static Entity find_entity_with_name(anton_stl::String_View name) {
    ECS& ecs = Editor::get_ecs();
    auto entities_with_names = ecs.access<Entity_Name>();
    for (Entity const entity: entities_with_names) {
        Entity_Name& entity_name = entities_with_names.get(entity);
        if (name == entity_name.name) {
            return entity;
        }
    }

    return null_entity;
}

static anton_stl::String create_unique_entity_name(anton_stl::String base_name) {}

Outliner::Outliner() {
    layout = new QVBoxLayout;
    setLayout(layout);
    setMinimumHeight(300);
}

Outliner::~Outliner() {
    delete layout;
}

void Outliner::remove_entities(anton_stl::Vector<Entity> const& entities_to_remove) {
    for (Entity const entity: entities_to_remove) {
        if (auto iter = anton_stl::find_if(items.begin(), items.end(), [entity](Outliner_Item const& item) { return item.get_associated_entity() == entity; });
            iter != items.end()) {
            layout->removeWidget(&(*iter));
            items.erase(iter, iter + 1);
        }
    }
}

void Outliner::update() {
    // TODO: A better way to handle adding new entities
    ECS& ecs = Editor::get_ecs();
    for (Entity const entity: ecs.get_entities()) {
        if (auto iter = anton_stl::find_if(items.begin(), items.end(), [entity](Outliner_Item const& item) { return item.get_associated_entity() == entity; });
            iter == items.end()) {
            anton_stl::String_View name;
            if (ecs.has_component<Entity_Name>(entity)) {
                Entity_Name& entity_name = ecs.get_component<Entity_Name>(entity);
                name = entity_name.name;
            } else {
                name = u8"<unnamed entity>";
            }

            Outliner_Item& item = items.emplace_back(entity, name);
            item.show();
            layout->addWidget(&item);
        }
    }
}
