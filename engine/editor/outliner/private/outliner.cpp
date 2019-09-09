#include <outliner.hpp>

#include <anton_stl/algorithm.hpp>
#include <anton_stl/string.hpp>
#include <anton_stl/string_view.hpp>
#include <components/entity_name.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>

ANTON_DISABLE_WARNINGS()
#include <QScrollArea>
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
    setMinimumHeight(100);
    layout = new QVBoxLayout(this);
    scroll_area = new QScrollArea;
    scroll_area->setWidgetResizable(true);
    scroll_area_contents = new QWidget;
    scroll_area_contents_layout = new QVBoxLayout(scroll_area_contents);
    scroll_area_contents_layout->setSpacing(2);
    scroll_area->setWidget(scroll_area_contents);
    layout->addWidget(scroll_area);
}

Outliner::~Outliner() {
    delete layout;
}

void Outliner::add_entity(Entity const entity) {
    // TODO: How do I update the name?
    ECS& ecs = Editor::get_ecs();
    anton_stl::String_View name;
    Entity_Name* entity_name = ecs.try_get_component<Entity_Name>(entity);
    if (entity_name) {
        name = entity_name->name;
    } else {
        name = u8"<unnamed entity>";
    }

    Outliner_Item& item = items.emplace_back(entity, name);
    scroll_area_contents_layout->addWidget(&item);
    connect(&item, &Outliner_Item::selected, this, &Outliner::entity_selected);
    connect(&item, &Outliner_Item::deselected, this, &Outliner::entity_deselected);
}

void Outliner::remove_entities(anton_stl::Vector<Entity> const& entities_to_remove) {
    for (Entity const entity: entities_to_remove) {
        if (auto iter = anton_stl::find_if(items.begin(), items.end(), [entity](Outliner_Item const& item) { return item.get_associated_entity() == entity; });
            iter != items.end()) {
            scroll_area_contents_layout->removeWidget(&(*iter));
            disconnect(&(*iter), &Outliner_Item::selected, this, &Outliner::entity_selected);
            disconnect(&(*iter), &Outliner_Item::deselected, this, &Outliner::entity_deselected);
            items.erase(iter, iter + 1);
        }
    }
}

void Outliner::select_entities(anton_stl::Vector<Entity> const& selected_entities) {
    for (Outliner_Item& item: items) {
        if (anton_stl::any_of(selected_entities.begin(), selected_entities.end(),
                              [&item](Entity const entity) { return item.get_associated_entity() == entity; })) {
            item.select();
        } else {
            item.deselect();
        }
    }
}

void Outliner::select_entity(Entity const entity) {
    for (Outliner_Item& item: items) {
        if (item.get_associated_entity() == entity) {
            item.select();
            break;
        }
    }
}

void Outliner::deselect_entity(Entity const entity) {
    for (Outliner_Item& item: items) {
        if (item.get_associated_entity() == entity) {
            item.deselect();
            break;
        }
    }
}

void Outliner::update() {}
