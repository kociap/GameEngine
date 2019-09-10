#include <ecs/component_container.hpp>

namespace anton_engine {
    void Component_Container_Base::serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const* container) {
        serialization::serialize(archive, container->entities);
    }

    void Component_Container_Base::deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base*& container) {
        serialization::deserialize(archive, container->entities);
        for (int64_t i = 0; i < container->entities.size(); i += 1) {
            auto index = container->indirect_index(container->entities[i]);
            container->ensure(index);
            container->indirect[index] = i;
        }
    }

    Component_Container_Base::~Component_Container_Base() {}

    Component_Container_Base::pointer Component_Container_Base::data() {
        return entities.data();
    }

    Component_Container_Base::const_pointer Component_Container_Base::data() const {
        return entities.data();
    }

    Component_Container_Base::iterator Component_Container_Base::begin() {
        return entities.begin();
    }

    Component_Container_Base::iterator Component_Container_Base::end() {
        return entities.end();
    }

    bool Component_Container_Base::has(Entity const entity) {
        auto index = indirect_index(entity);
        return index < indirect.size() && indirect[index] != npos;
    }

    Component_Container_Base::size_type Component_Container_Base::size() const {
        return entities.size();
    }

    void Component_Container_Base::add_entity(Entity const entity) {
        ANTON_ASSERT(!has(entity), "Entity has already been registered");
        entities.emplace_back(entity);
        auto index = indirect_index(entity);
        ensure(index);
        indirect[index] = entities.size() - 1;
    }

    Component_Container_Base::size_type Component_Container_Base::get_component_index(Entity const entity) {
        ANTON_ASSERT(has(entity), "Attempting to get index of an entity that has not been registered");
        return indirect[indirect_index(entity)];
    }

    void Component_Container_Base::remove_entity(Entity const entity) {
        ANTON_ASSERT(has(entity), "Attempting to remove entity that has not been registered");
        auto index = indirect_index(entity);
        auto back_index = indirect_index(entities.back());
        entities.erase_unsorted(index);
        indirect[back_index] = indirect[index];
        indirect[index] = npos;
    }

    Component_Container_Base::size_type Component_Container_Base::indirect_index(Entity const entity) {
        return entity.id;
    }

    void Component_Container_Base::ensure(size_type index) {
        if (indirect.size() <= index) {
            indirect.resize(index + 1, npos);
        }
    }
} // namespace anton_engine
