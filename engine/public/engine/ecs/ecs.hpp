#pragma once

#include <anton/algorithm.hpp>
#include <anton/array.hpp>
#include <anton/tuple.hpp>
#include <anton/type_traits.hpp>
#include <anton/typeid.hpp>
#include <core/integer_sequence_generator.hpp>
#include <core/serialization/archives/binary.hpp>
#include <engine.hpp>
#include <engine/ecs/component_container.hpp>
#include <engine/ecs/component_view.hpp>
#include <engine/ecs/entity.hpp>

namespace anton_engine {
  class ECS {
  public:
    ECS() = default;
    ECS(ECS const&);
    ECS(ECS&&);
    ~ECS();

    // Returns: Array of entities that have Component associated with them
    template<typename Component>
    [[nodiscard]] Entity const* entities() const;

    // Returns: Array of components of type Component
    template<typename Component>
    [[nodiscard]] Component const* components() const;

    // Create entity without any attached components
    Entity create();

    // Create entity with Components... components attached.
    // Components... must be default constructible.
    template<typename... Components>
    auto create();
    void destroy(Entity);
    template<typename T, typename... Ctor_Args>
    T& add_component(Entity, Ctor_Args&&... args);
    template<typename T>
    void remove_component(Entity);
    template<typename... Ts>
    decltype(auto) get_component(Entity);
    template<typename... Ts>
    decltype(auto) try_get_component(Entity);
    template<typename... Ts>
    [[nodiscard]] bool has_component(Entity);

    template<typename Component, typename Sort, typename Predicate>
    void sort(Sort, Predicate);

    template<typename... Ts>
    Component_View<Ts...> view();

    // Ts... are the components to copy
    template<typename... Ts>
    ECS snapshot() const;

    anton::Array<Entity> const& get_entities() const;
    anton::Array<Entity> const& get_entities_to_remove() const;

    void remove_requested_entities();

    friend void serialize(serialization::Binary_Output_Archive&, ECS const&);
    friend void deserialize(serialization::Binary_Input_Archive&, ECS&);

  private:
    struct Components_Container_Data {
      u64 family;
      Component_Container_Base* container = nullptr;
      void (*remove)(Component_Container_Base&, Entity);
      Component_Container_Base* (*make_snapshot)(
        Component_Container_Base const&);
    };

    anton::Array<Entity> _entities;
    anton::Array<Entity> entities_to_remove;
    anton::Array<Components_Container_Data> containers;
    Integer_Sequence_Generator id_generator;

    template<typename... Container_Data>
    ECS(anton::Array<Entity> const&, anton::Array<Entity> const&,
        Integer_Sequence_Generator, Container_Data...);

    template<typename T>
    Component_Container<T>* ensure_container();
    template<typename T>
    Component_Container<T>* find_container();
    template<typename T>
    Component_Container<T> const* find_container() const;
    template<typename T>
    Components_Container_Data* find_container_data();
    template<typename T>
    Components_Container_Data const* find_container_data() const;
  };

  ECS& get_ecs();
} // namespace anton_engine

namespace anton_engine {
  inline ECS::ECS(ECS const& other)
    : _entities(other._entities), entities_to_remove(other.entities_to_remove),
      containers(other.containers), id_generator(other.id_generator)
  {
    for(Components_Container_Data& data: containers) {
      data.container = data.make_snapshot(*data.container);
    }
  }

  inline ECS::ECS(ECS&& other)
    : _entities(ANTON_MOV(other._entities)),
      entities_to_remove(ANTON_MOV(other.entities_to_remove)),
      containers(ANTON_MOV(other.containers)), id_generator(other.id_generator)
  {
  }

  template<typename Component>
  [[nodiscard]] inline Entity const* ECS::entities() const
  {
    auto const* c = find_container<Component>();
    return c ? c->entities() : nullptr;
  }

  template<typename Component>
  [[nodiscard]] inline Component const* ECS::components() const
  {
    auto const* c = find_container<Component>();
    return c ? c->components() : nullptr;
  }

  template<typename... Components>
  inline auto ECS::create()
  {
    if constexpr(sizeof...(Components) == 0) {
      return create();
    } else {
      Entity entity = create();
      return anton::Tuple<Entity, Components&...>{
        entity, add_component<Components>(entity)...};
    }
  }

  inline void ECS::destroy(Entity const entity)
  {
    entities_to_remove.push_back(entity);
  }

  template<typename T, typename... Ctor_Args>
  inline T& ECS::add_component(Entity const entity, Ctor_Args&&... args)
  {
    Component_Container<T>& components = *ensure_container<T>();
    return components.add(entity, ANTON_FWD(args)...);
  }

  template<typename T>
  inline void ECS::remove_component(Entity const entity)
  {
    Component_Container<T>& components = *ensure_container<T>();
    components.remove(entity);
  }

  template<typename... Ts>
  inline decltype(auto) ECS::get_component(Entity const entity)
  {
    if constexpr(sizeof...(Ts) == 1) {
      Component_Container<Ts...>* components = ensure_container<Ts...>();
      return components->get(entity);
    } else {
      return anton::make_tuple(get_component<Ts>(entity)...);
    }
  }

  template<typename... Ts>
  inline decltype(auto) ECS::try_get_component(Entity const entity)
  {
    if constexpr(sizeof...(Ts) == 1) {
      Component_Container<Ts...>* components = ensure_container<Ts...>();
      return components->try_get(entity);
    } else {
      return anton::make_tuple(try_get_component<Ts>(entity)...);
    }
  }

  template<typename... Ts>
  inline bool ECS::has_component(Entity const entity)
  {
    static_assert(sizeof...(Ts) > 0, "Empty parameter pack");
    auto const containers = anton::make_tuple(find_container<Ts>()...);
    return (... &&
            (anton::get<Component_Container<Ts>*>(containers)
               ? anton::get<Component_Container<Ts>*>(containers)->has(entity)
               : false));
  }

  template<typename Component, typename Sort, typename Predicate>
  inline void ECS::sort(Sort sort, Predicate predicate)
  {
    Component_Container<Component>* const container =
      find_container<Component>();
    if(container) {
      container->sort(sort, predicate);
    }
  }

  template<typename... Ts>
  inline Component_View<Ts...> ECS::view()
  {
    return Component_View<Ts...>(ensure_container<Ts>()...);
  }

  template<typename... Ts>
  inline ECS ECS::snapshot() const
  {
    ANTON_VERIFY(
      (... && (find_container_data<Ts>() != nullptr)),
      "Cannot create a snapshot of component that has not been added.");
    return ECS(_entities, entities_to_remove, id_generator,
               *find_container_data<Ts>()...);
  }

  inline anton::Array<Entity> const& ECS::get_entities() const
  {
    return _entities;
  }

  inline anton::Array<Entity> const& ECS::get_entities_to_remove() const
  {
    return entities_to_remove;
  }

  inline void ECS::remove_requested_entities()
  {
    for(auto& container_data: containers) {
      for(Entity const entity: entities_to_remove) {
        if(container_data.container->has(entity)) {
          container_data.remove(*container_data.container, entity);
        }
      }
    }

    for(Entity const entity: entities_to_remove) {
      auto iter = anton::find(_entities.begin(), _entities.end(), entity);
      _entities.erase_unsorted(iter);
    }

    entities_to_remove.clear();
  }

  template<typename... Container_Data>
  inline ECS::ECS(anton::Array<Entity> const& e, anton::Array<Entity> const& er,
                  Integer_Sequence_Generator g, Container_Data... data)
    : _entities(e), entities_to_remove(er),
      containers(anton::reserve, sizeof...(Container_Data)), id_generator(g)
  {
    static_assert(
      (... && anton::is_same<Container_Data, Components_Container_Data>),
      "Template argument Container_Data is not Components_Container_Data.");
    (...,
     containers.emplace_back(data.family, data.make_snapshot(*data.container),
                             data.remove, data.make_snapshot));
  }

  template<typename T>
  inline Component_Container<T>* ECS::ensure_container()
  {
    u64 const type = anton::type_identifier<T>();
    for(auto& data: containers) {
      if(data.family == type) {
        return static_cast<Component_Container<T>*>(data.container);
      }
    }

    auto& data = containers.emplace_back();
    try {
      data.container = new Component_Container<T>();
    } catch(...) {
      containers.pop_back();
      throw;
    }
    data.family = type;
    data.remove = [](Component_Container_Base& container, Entity const entity) {
      static_cast<Component_Container<T>&>(container).remove(entity);
    };
    data.make_snapshot = [](Component_Container_Base const& container)
      -> Component_Container_Base* {
      Component_Container<T> const& c =
        static_cast<Component_Container<T> const&>(container);
      return new Component_Container<T>(c);
    };
    return static_cast<Component_Container<T>*>(data.container);
  }

  template<typename T>
  inline Component_Container<T> const* ECS::find_container() const
  {
    u64 const type = anton::type_identifier<T>();
    for(auto& data: containers) {
      if(data.family == type) {
        return static_cast<Component_Container<T> const*>(data.container);
      }
    }
    return nullptr;
  }

  template<typename T>
  inline Component_Container<T>* ECS::find_container()
  {
    u64 const type = anton::type_identifier<T>();
    for(auto& data: containers) {
      if(data.family == type) {
        return static_cast<Component_Container<T>*>(data.container);
      }
    }
    return nullptr;
  }

  template<typename T>
  inline ECS::Components_Container_Data* ECS::find_container_data()
  {
    u64 const type = anton::type_identifier<T>();
    for(auto& data: containers) {
      if(data.family == type) {
        return &data;
      }
    }
    return nullptr;
  }

  template<typename T>
  inline ECS::Components_Container_Data const* ECS::find_container_data() const
  {
    u64 const type = anton::type_identifier<T>();
    for(auto& data: containers) {
      if(data.family == type) {
        return &data;
      }
    }
    return nullptr;
  }
} // namespace anton_engine
