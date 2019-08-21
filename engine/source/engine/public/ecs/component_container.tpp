template <typename C>
void Component_Container<C>::serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const* container) {
    if constexpr (!std::is_empty_v<C>) {
        serialization::serialize(archive, static_cast<Component_Container<C> const*>(container)->components);
    }
    Component_Container_Base::serialize(archive, container);
}

template <typename C>
void Component_Container<C>::deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base*& container) {
    container = new Component_Container<C>();
    if constexpr (!std::is_empty_v<C>) {
        serialization::deserialize(archive, static_cast<Component_Container<C>*>(container)->components);
    }
    Component_Container_Base::deserialize(archive, container);
}
