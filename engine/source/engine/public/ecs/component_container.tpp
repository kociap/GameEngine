template <typename C>
void Component_Container<C>::serialize(std::ostream& archive, Component_Container_Base* container) {
    if constexpr (!std::is_empty_v<C>) {
        serialization::serialize(archive, static_cast<Component_Container<C>*>(container)->components);
    }
    Component_Container_Base::serialize(archive, container);
}

template <typename C>
void Component_Container<C>::deserialize(std::istream& archive, Component_Container_Base*& container) {
    container = new Component_Container<C>();
    if constexpr (!std::is_empty_v<C>) {
        serialization::deserialize(archive, static_cast<Component_Container<C>*>(container)->components);
    }
    Component_Container_Base::deserialize(archive, container);
}