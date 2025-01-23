#include <outliner.hpp>

#include <anton/algorithm.hpp>
#include <anton/string.hpp>
#include <anton/string_view.hpp>
#include <editor.hpp>
#include <engine/components/entity_name.hpp>
#include <engine/ecs/ecs.hpp>

ANTON_DISABLE_WARNINGS();
#include <QScrollArea>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
  static Entity find_entity_with_name(anton::String_View name)
  {
    ECS& ecs = Editor::get_ecs();
    auto entities_with_names = ecs.view<Entity_Name>();
    for(Entity const entity: entities_with_names) {
      Entity_Name& entity_name = entities_with_names.get(entity);
      if(name == entity_name.name) {
        return entity;
      }
    }

    return null_entity;
  }

  static anton::String create_unique_entity_name(anton::String base_name) {}

  Outliner::Outliner()
  {
    setMinimumHeight(100);
    layout = new QVBoxLayout(this);
    scroll_area = new QScrollArea;
    scroll_area->setWidgetResizable(true);
    list_widget = new List_Widget<Outliner_Item>();
    list_widget->set_spacing(2);
    scroll_area->setWidget(list_widget);
    layout->addWidget(scroll_area);
  }

  Outliner::~Outliner()
  {
    delete layout;
  }

  void Outliner::add_entity(Entity const entity)
  {
    ECS& ecs = Editor::get_ecs();
    anton::String_View name;
    Entity_Name* entity_name = ecs.try_get_component<Entity_Name>(entity);
    if(entity_name) {
      name = entity_name->name;
    } else {
      name = u8"<unnamed entity>";
    }

    Outliner_Item& item = list_widget->emplace_back(entity, name);
    item.show();
  }

  void Outliner::remove_entities(anton::Array<Entity> const& entities_to_remove)
  {
    for(Entity const entity: entities_to_remove) {
      if(auto iter = anton::find_if(list_widget->begin(), list_widget->end(),
                                    [entity](Outliner_Item const& item) {
                                      return item.get_associated_entity() ==
                                             entity;
                                    });
         iter != list_widget->end()) {
        iter->setParent(nullptr);
        list_widget->erase(iter, iter + 1);
      }
    }
  }

  void Outliner::select_entities(anton::Array<Entity> const& selected_entities)
  {
    for(Outliner_Item& item: *list_widget) {
      if(anton::any_of(selected_entities.begin(), selected_entities.end(),
                       [&item](Entity const entity) {
                         return item.get_associated_entity() == entity;
                       })) {
        item.select();
      } else {
        item.deselect();
      }
    }
  }

  void Outliner::select_entity(Entity const entity)
  {
    for(Outliner_Item& item: *list_widget) {
      if(item.get_associated_entity() == entity) {
        item.select();
        break;
      }
    }
  }

  void Outliner::deselect_entity(Entity const entity)
  {
    for(Outliner_Item& item: *list_widget) {
      if(item.get_associated_entity() == entity) {
        item.deselect();
        break;
      }
    }
  }

  void Outliner::update()
  {
    ECS& ecs = Editor::get_ecs();
    for(Outliner_Item& item: *list_widget) {
      Entity_Name* entity_name =
        ecs.try_get_component<Entity_Name>(item.get_associated_entity());
      if(entity_name) {
        item.set_name(entity_name->name);
      } else {
        item.set_name(u8"<unnamed entity>");
      }
    }
  }
} // namespace anton_engine
