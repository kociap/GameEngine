#pragma once

#include <anton/algorithm.hpp>
#include <anton/array.hpp>
#include <anton/type_traits.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/types/array.hpp>
#include <engine/ecs/component_container_iterator.hpp>
#include <engine/ecs/entity.hpp>

namespace anton_engine {
  class Component_Container_Base {
  public:
    using size_type = anton::Array<Entity>::size_type;
    using iterator = anton::Array<Entity>::iterator;
    using const_iterator = anton::Array<Entity>::const_iterator;

    virtual ~Component_Container_Base() = default;

    [[nodiscard]] Entity* entities();
    [[nodiscard]] Entity const* entities() const;
    [[nodiscard]] iterator begin();
    [[nodiscard]] iterator end();
    [[nodiscard]] size_type size() const;
    [[nodiscard]] bool has(Entity) const;

    friend void serialize(serialization::Binary_Output_Archive&,
                          Component_Container_Base const&);
    friend void deserialize(serialization::Binary_Input_Archive&,
                            Component_Container_Base&);

  protected:
    constexpr static size_type npos = static_cast<size_type>(-1);

    void add_entity(Entity entity);
    [[nodiscard]] size_type get_component_index(Entity entity);
    void remove_entity(Entity entity);

    // Sort entities and the provided component array.
    template<typename Component, typename Sort, typename Predicate>
    void sort_components(anton::Array<Component>&, Sort sort,
                         Predicate predicate);

    // Sort only entities.
    void sort_entities();

  private:
    // Indices into entities array
    anton::Array<size_type> _indirect;
    anton::Array<Entity> _entities;

    [[nodiscard]] size_type indirect_index(Entity entity) const;
    void ensure(size_type index);
  };

  template<typename Component>
  class Component_Container: public Component_Container_Base {
  public:
    using iterator = Component_Container_Iterator<Component>;

    static void serialize(serialization::Binary_Output_Archive& archive,
                          Component_Container_Base const*);
    static void deserialize(serialization::Binary_Input_Archive& archive,
                            Component_Container_Base*&);

    virtual ~Component_Container() = default;

    [[nodiscard]] Component* components();
    [[nodiscard]] Component const* components() const;

    [[nodiscard]] iterator begin()
    {
      return {components(), 0};
    }

    [[nodiscard]] iterator end()
    {
      return {components(), size()};
    }

    template<typename... Args>
    Component& add(Entity const entity, Args&&... args)
    {
      ANTON_ASSERT(!has(entity), "Attempting to add duplicate entity");
      if constexpr(anton::is_empty<Component>) {
        add_entity(entity);
        return _components;
      } else {
        _components.emplace_back(ANTON_FWD(args)...);
        add_entity(entity);
        return _components[_components.size() - 1];
      }
    }

    void remove(Entity const entity)
    {
      if constexpr(!anton::is_empty<Component>) {
        _components.erase_unsorted(get_component_index(entity));
      }

      remove_entity(entity);
    }

    [[nodiscard]] Component& get(Entity const entity)
    {
      ANTON_ASSERT(has(entity),
                   "Attempting to get component of an entity that has not been "
                   "registered");
      if constexpr(anton::is_empty<Component>) {
        return _components;
      } else {
        return _components[get_component_index(entity)];
      }
    }

    [[nodiscard]] Component* try_get(Entity const entity)
    {
      if constexpr(anton::is_empty<Component>) {
        return has(entity) ? &_components : nullptr;
      } else {
        return has(entity) ? _components.data() + get_component_index(entity)
                           : nullptr;
      }
    }

    template<typename Sort, typename Predicate>
    void sort(Sort sort, Predicate predicate);

  private:
    using base_t = Component_Container_Base;

    anton::conditional<anton::is_empty<Component>, Component,
                       anton::Array<Component>>
      _components;
  };
} // namespace anton_engine

namespace anton_engine {
  inline Entity* Component_Container_Base::entities()
  {
    return _entities.data();
  }

  inline Entity const* Component_Container_Base::entities() const
  {
    return _entities.data();
  }

  template<typename Component>
  inline Component* Component_Container<Component>::components()
  {
    if constexpr(anton::is_empty<Component>) {
      return &_components;
    } else {
      return _components.data();
    }
  }

  template<typename Component>
  inline Component const* Component_Container<Component>::components() const
  {
    if constexpr(anton::is_empty<Component>) {
      return &_components;
    } else {
      return _components.data();
    }
  }

  inline Component_Container_Base::iterator Component_Container_Base::begin()
  {
    return _entities.begin();
  }

  inline Component_Container_Base::iterator Component_Container_Base::end()
  {
    return _entities.end();
  }

  inline bool Component_Container_Base::has(Entity const entity) const
  {
    auto index = indirect_index(entity);
    return index < _indirect.size() && _indirect[index] != npos;
  }

  inline Component_Container_Base::size_type
  Component_Container_Base::size() const
  {
    return _entities.size();
  }

  inline void Component_Container_Base::add_entity(Entity const entity)
  {
    ANTON_ASSERT(!has(entity), "Entity has already been registered");
    _entities.emplace_back(entity);
    auto index = indirect_index(entity);
    ensure(index);
    _indirect[index] = _entities.size() - 1;
  }

  inline Component_Container_Base::size_type
  Component_Container_Base::get_component_index(Entity const entity)
  {
    ANTON_ASSERT(
      has(entity),
      "Attempting to get index of an entity that has not been registered");
    return _indirect[indirect_index(entity)];
  }

  inline void Component_Container_Base::remove_entity(Entity const entity)
  {
    ANTON_ASSERT(has(entity),
                 "Attempting to remove entity that has not been registered");
    auto index = indirect_index(entity);
    auto back_index = indirect_index(_entities[_entities.size() - 1]);
    _entities.erase_unsorted(index);
    _indirect[back_index] = _indirect[index];
    _indirect[index] = npos;
  }

  inline Component_Container_Base::size_type
  Component_Container_Base::indirect_index(Entity const entity) const
  {
    return entity.id;
  }

  inline void Component_Container_Base::ensure(size_type index)
  {
    if(_indirect.size() <= index) {
      _indirect.resize(index + 1, npos);
    }
  }

  template<typename Component, typename Sort, typename Predicate>
  void
  Component_Container_Base::sort_components(anton::Array<Component>& components,
                                            Sort sort, Predicate predicate)
  {
    static_assert(
      anton::is_invocable_r<bool, Predicate, Entity const, Entity const> ||
        anton::is_invocable_r<bool, Predicate, Component const,
                              Component const>,
      "Predicate is not invocable with either Entity or Component as the "
      "parameter");
    anton::Array<i64> indices(_entities.size());
    anton::fill_with_consecutive(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(),
         [&, cmp = predicate](i64 const lhs, i64 const rhs) -> bool {
           if constexpr(anton::is_invocable_r<bool, Predicate, Entity const,
                                              Entity const>) {
             return cmp(ANTON_AS_CONST(_entities[lhs]),
                        ANTON_AS_CONST(_entities[rhs]));
           } else {
             return cmp(ANTON_AS_CONST(components[lhs]),
                        ANTON_AS_CONST(components[rhs]));
           }
         });

    using anton::swap;
    for(i64 i = 0; i < indices.size(); ++i) {
      i64 const sorted_index = indices[i];
      if(i != sorted_index) {
        swap(components[i], components[sorted_index]);
        swap(_indirect[indirect_index(_entities[i])],
             _indirect[indirect_index(_entities[sorted_index])]);
        swap(_entities[i], _entities[sorted_index]);
      }
    }
  }

  template<typename Component>
  template<typename Sort, typename Predicate>
  inline void Component_Container<Component>::sort(Sort sort,
                                                   Predicate predicate)
  {
    sort_components(_components, sort, predicate);
  }

  inline void serialize(serialization::Binary_Output_Archive& archive,
                        Component_Container_Base const& container)
  {
    serialize(archive, container._entities);
  }

  inline void deserialize(serialization::Binary_Input_Archive& archive,
                          Component_Container_Base& container)
  {
    deserialize(archive, container._entities);
    for(i64 i = 0; i < container._entities.size(); i += 1) {
      auto index = container.indirect_index(container._entities[i]);
      container.ensure(index);
      container._indirect[index] = i;
    }
  }

  template<typename C>
  inline void Component_Container<C>::serialize(
    serialization::Binary_Output_Archive& archive,
    Component_Container_Base const* container)
  {
    if constexpr(!anton::is_empty<C>) {
      anton_engine::serialize(
        archive,
        static_cast<Component_Container<C> const*>(container)->_components);
    }
    anton_engine::serialize(archive, *container);
  }

  template<typename C>
  inline void Component_Container<C>::deserialize(
    serialization::Binary_Input_Archive& archive,
    Component_Container_Base*& container)
  {
    container = new Component_Container<C>();
    if constexpr(!anton::is_empty<C>) {
      anton_engine::deserialize(
        archive, static_cast<Component_Container<C>*>(container)->_components);
    }
    anton_engine::deserialize(archive, *container);
  }
} // namespace anton_engine
