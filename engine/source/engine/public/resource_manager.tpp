template <typename T>
Handle<T> Resource_Manager<T>::add(T&& resource) {
    Handle<T> handle{resource.id.value()};
    resources.emplace_back(std::forward<T>(resource));
    return handle;
}

template <typename T>
T& Resource_Manager<T>::get(Handle<T> handle) {
    for (T& resource: resources) {
        if (resource.id.value() == handle.value) {
            return resource;
        }
    }

    throw std::invalid_argument("");
}

template <typename T>
void Resource_Manager<T>::remove(Handle<T> handle) {
    // TODO size_t ?
    containers::Vector<T>::size_type i = 0;
    for (T& resource: resources) {
        if (handle.value == resource.id.value()) {
            resources.erase_unsorted_unchecked(i);
            break;
        }
        ++i;
    }
}