#include <ecs/ecs.hpp>

#include <anton_assert.hpp>
#include <anton_stl/algorithm.hpp>
#include <anton_stl/memory.hpp>
#include <ecs/component_serialization.hpp>

#include <build_config.hpp>

#if ANTON_WITH_EDITOR
#    include <editor.hpp>
#    include <editor_window.hpp>
#    include <outliner.hpp>
#endif

namespace anton_engine {
    ECS::~ECS() {
        for (auto& container_data: containers) {
            anton_stl::destruct(container_data.container);
            container_data.container = nullptr;
        }
    }

    Entity ECS::create() {
        // TODO: More clever entity creation (generations and reusing ids)
#if !ANTON_WITH_EDITOR
        return entities.emplace_back(id_generator.next());
#else
        Entity entity = entities.emplace_back(id_generator.next());
        Editor_Window& editor = Editor::get_window();
        editor.outliner->add_entity(entity);
        return entity;
#endif
    }

    void ECS::serialize_component_container(Type_Family::family_t identifier, serialization::Binary_Output_Archive& archive,
                                            Component_Container_Base const* container) const {
        ANTON_ASSERT(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
        auto& serialization_funcs = get_component_serialization_funcs();
        auto iter = anton_stl::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                                       [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
        ANTON_ASSERT(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
        iter->serialize(archive, container);
    }

    void ECS::deserialize_component_container(Type_Family::family_t identifier, serialization::Binary_Input_Archive& archive,
                                              Component_Container_Base*& container) {
        ANTON_ASSERT(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
        auto& serialization_funcs = get_component_serialization_funcs();
        auto iter = anton_stl::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                                       [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
        ANTON_ASSERT(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
        iter->deserialize(archive, container);
    }

    void serialize(serialization::Binary_Output_Archive& archive, ECS const& ecs) {
        serialize(archive, ecs.entities);
        archive.write(ecs.containers.size());
        for (auto const& data: ecs.containers) {
            archive.write(data.family);
            ecs.serialize_component_container(data.family, archive, data.container);
        }
    }

    void deserialize(serialization::Binary_Input_Archive& archive, ECS& ecs) {
        deserialize(archive, ecs.entities);
        int64_t containers_count;
        archive.read(containers_count);
        ecs.containers.resize(containers_count);
        for (auto& data: ecs.containers) {
            archive.read(data.family);
            ecs.deserialize_component_container(data.family, archive, data.container);
        }
    }
} // namespace anton_engine
