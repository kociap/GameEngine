namespace anton_engine {
    template <typename C>
    void Component_Container<C>::serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const* container) {
        if constexpr (!anton_stl::is_empty<C>) {
            anton_engine::serialize(archive, static_cast<Component_Container<C> const*>(container)->_components);
        }
        anton_engine::serialize(archive, *container);
    }

    template <typename C>
    void Component_Container<C>::deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base*& container) {
        container = new Component_Container<C>();
        if constexpr (!anton_stl::is_empty<C>) {
            anton_engine::deserialize(archive, static_cast<Component_Container<C>*>(container)->_components);
        }
        anton_engine::deserialize(archive, *container);
    }
} // namespace anton_engine
