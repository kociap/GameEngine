#ifndef CORE_SERIALIZATION_HPP_INCLUDE
#define CORE_SERIALIZATION_HPP_INCLUDE

#include <serialization/archives/binary.hpp>
#include <type_traits>

namespace serialization {
#define DEFAULT_SERIALIZABLE(type) \
    template <>                    \
    struct serialization::use_default_serialize<type>: std::true_type {}

#define DEFAULT_DESERIALIZABLE(type) \
    template <>                      \
    struct serialization::use_default_deserialize<type>: std::true_type {}

    template <typename T>
    struct use_default_serialize: std::false_type {};

    template <typename T>
    struct use_default_deserialize: std::false_type {};

    template <typename T>
    std::enable_if_t<serialization::use_default_serialize<T>::value, void> serialize(Binary_Output_Archive& out, T const& obj) {
        out.write(obj);
    }

    template <typename T>
    std::enable_if_t<serialization::use_default_deserialize<T>::value, void> deserialize(Binary_Input_Archive& in, T& obj) {
        in.read(obj);
    }
} // namespace serialization

#endif // !CORE_SERIALIZATION_HPP_INCLUDE
