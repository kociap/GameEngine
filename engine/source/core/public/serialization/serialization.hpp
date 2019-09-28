#ifndef CORE_SERIALIZATION_HPP_INCLUDE
#define CORE_SERIALIZATION_HPP_INCLUDE

#include <anton_stl/type_traits.hpp>
#include <serialization/archives/binary.hpp>

namespace anton_engine {
    template <typename T>
    struct use_default_serialize: anton_stl::False_Type {};

    template <typename T>
    anton_stl::enable_if<use_default_serialize<T>::value, void> serialize(serialization::Binary_Output_Archive& out, T const& obj) {
        out.write(obj);
    }

    template <typename T>
    anton_stl::enable_if<use_default_serialize<T>::value, void> deserialize(serialization::Binary_Input_Archive& in, T& obj) {
        in.read(obj);
    }
} // namespace anton_engine

#define ANTON_DEFAULT_SERIALIZABLE(type)                             \
    namespace anton_engine {                                         \
        template <>                                                  \
        struct use_default_serialize<type>: anton_stl::True_Type {}; \
    }

#endif // !CORE_SERIALIZATION_HPP_INCLUDE
