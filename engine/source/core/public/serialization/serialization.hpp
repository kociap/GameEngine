#ifndef CORE_SERIALIZATION_HPP_INCLUDE
#define CORE_SERIALIZATION_HPP_INCLUDE

#include <anton_stl/type_traits.hpp>
#include <serialization/archives/binary.hpp>

namespace anton_engine {
    namespace serialization {
        template <typename T>
        struct use_default_serialize: anton_stl::False_Type {};

        template <typename T>
        struct use_default_deserialize: anton_stl::False_Type {};

        template <typename T>
        anton_stl::enable_if<serialization::use_default_serialize<T>::value, void> serialize(Binary_Output_Archive& out, T const& obj) {
            out.write(obj);
        }

        template <typename T>
        anton_stl::enable_if<serialization::use_default_deserialize<T>::value, void> deserialize(Binary_Input_Archive& in, T& obj) {
            in.read(obj);
        }
    } // namespace serialization
} // namespace anton_engine

#define ANTON_DEFAULT_SERIALIZABLE(type)                                 \
    namespace anton_engine {                                             \
        namespace serialization {                                        \
            template <>                                                  \
            struct use_default_serialize<type>: anton_stl::True_Type {}; \
        }                                                                \
    }

#define ANTON_DEFAULT_DESERIALIZABLE(type) \
    template <>                            \
    struct anton_engine::serialization::use_default_deserialize<type>: anton_engine::anton_stl::True_Type {}

#endif // !CORE_SERIALIZATION_HPP_INCLUDE
