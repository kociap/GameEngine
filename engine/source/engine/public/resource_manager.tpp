template <typename T>
typename Resource_Manager<T>::iterator Resource_Manager<T>::begin() {
    return resources.begin();
}

template <typename T>
typename Resource_Manager<T>::iterator Resource_Manager<T>::end() {
    return resources.end();
}

template <typename T>
Handle<T> Resource_Manager<T>::add(T&& resource) {
    resources.emplace_back(std::forward<T>(resource));
    id_type resource_id = ID_Generator<T>::next();
    identifiers.push_back(resource_id);
    return {resource_id};
}

template <typename T>
T& Resource_Manager<T>::get(Handle<T> handle) {
    for (containers::Vector<T>::size_type i = 0; i < identifiers.size(); ++i) {
        if (identifiers[i] == handle.value) {
            return resources[i];
        }
    }

    throw std::invalid_argument("");
}

template <typename T>
void Resource_Manager<T>::remove(Handle<T> handle) {
    // TODO size_t ?
    for (containers::Vector<T>::size_type i = 0; i < identifiers.size(); ++i) {
        if (identifiers[i] == handle.value) {
            identifiers.erase_unsorted_unchecked(i);
            resources.erase_unsorted_unchecked(i);
            return;
        }
    }
}