#include <engine/ecs/ecs.hpp>

#include <core/assert.hpp>
#include <core/stl/algorithm.hpp>
#include <core/stl/memory.hpp>
#include <engine/ecs/component_serialization.hpp>

#include <build_config.hpp>

#if ANTON_WITH_EDITOR
#    include <editor.hpp>
#else
#    include <engine.hpp>
#endif // ANTON_WITH_EDITOR

namespace anton_engine {
    ECS::~ECS() {
        for (auto& container_data: containers) {
            anton_stl::destruct(container_data.container);
            container_data.container = nullptr;
        }
    }

    Entity ECS::create() {
        // TODO: More clever entity creation (generations and reusing ids)
        return _entities.emplace_back(id_generator.next());
    }

    static void serialize_component_container(u64 identifier, serialization::Binary_Output_Archive& archive, Component_Container_Base const* container) {
        ANTON_ASSERT(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
        auto& serialization_funcs = get_component_serialization_funcs();
        auto iter = anton_stl::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                                       [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
        ANTON_ASSERT(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
        iter->serialize(archive, container);
    }

    static void deserialize_component_container(u64 identifier, serialization::Binary_Input_Archive& archive, Component_Container_Base*& container) {
        ANTON_ASSERT(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
        auto& serialization_funcs = get_component_serialization_funcs();
        auto iter = anton_stl::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                                       [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
        ANTON_ASSERT(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
        iter->deserialize(archive, container);
    }

    void serialize(serialization::Binary_Output_Archive& archive, ECS const& ecs) {
        serialize(archive, ecs._entities);
        archive.write(ecs.containers.size());
        for (auto const& data: ecs.containers) {
            archive.write(data.family);
            serialize_component_container(data.family, archive, data.container);
        }
    }

    void deserialize(serialization::Binary_Input_Archive& archive, ECS& ecs) {
        deserialize(archive, ecs._entities);
        int64_t containers_count;
        archive.read(containers_count);
        ecs.containers.resize(containers_count);
        for (auto& data: ecs.containers) {
            archive.read(data.family);
            deserialize_component_container(data.family, archive, data.container);
        }
    }

    ECS& get_ecs() {
#if ANTON_WITH_EDITOR
        return Editor::get_ecs();
#else
        return Engine::get_ecs();
#endif
    }
} // namespace anton_engine
