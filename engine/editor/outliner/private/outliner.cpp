#include <outliner.hpp>

#include <anton_stl/algorithm.hpp>
#include <anton_stl/string.hpp>
#include <anton_stl/string_view.hpp>
#include <components/entity_name.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>

ANTON_DISABLE_WARNINGS();
#include <QScrollArea>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
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
        list_widget = new List_Widget<Outliner_Item>();
        list_widget->set_spacing(2);
        scroll_area->setWidget(list_widget);
        layout->addWidget(scroll_area);
    }

    Outliner::~Outliner() {
        delete layout;
    }

    void Outliner::add_entity(Entity const entity) {
        ECS& ecs = Editor::get_ecs();
        anton_stl::String_View name;
        Entity_Name* entity_name = ecs.try_get_component<Entity_Name>(entity);
        if (entity_name) {
            name = entity_name->name;
        } else {
            name = u8"<unnamed entity>";
        }

        Outliner_Item& item = list_widget->emplace_back(entity, name);
        item.show();
    }

    void Outliner::remove_entities(anton_stl::Vector<Entity> const& entities_to_remove) {
        for (Entity const entity: entities_to_remove) {
            if (auto iter = anton_stl::find_if(list_widget->begin(), list_widget->end(),
                                               [entity](Outliner_Item const& item) { return item.get_associated_entity() == entity; });
                iter != list_widget->end()) {
                iter->setParent(nullptr);
                list_widget->erase(iter, iter + 1);
            }
        }
    }

    void Outliner::select_entities(anton_stl::Vector<Entity> const& selected_entities) {
        for (Outliner_Item& item: *list_widget) {
            if (anton_stl::any_of(selected_entities.begin(), selected_entities.end(),
                                  [&item](Entity const entity) { return item.get_associated_entity() == entity; })) {
                item.select();
            } else {
                item.deselect();
            }
        }
    }

    void Outliner::select_entity(Entity const entity) {
        for (Outliner_Item& item: *list_widget) {
            if (item.get_associated_entity() == entity) {
                item.select();
                break;
            }
        }
    }

    void Outliner::deselect_entity(Entity const entity) {
        for (Outliner_Item& item: *list_widget) {
            if (item.get_associated_entity() == entity) {
                item.deselect();
                break;
            }
        }
    }

    void Outliner::update() {
        ECS& ecs = Editor::get_ecs();
        for (Outliner_Item& item: *list_widget) {
            Entity_Name* entity_name = ecs.try_get_component<Entity_Name>(item.get_associated_entity());
            if (entity_name) {
                item.set_name(entity_name->name);
            } else {
                item.set_name(u8"<unnamed entity>");
            }
        }
    }
} // namespace anton_engine
