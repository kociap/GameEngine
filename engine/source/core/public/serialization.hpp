#ifndef CORE_SERIALIZATION_HPP_INCLUDE
#define CORE_SERIALIZATION_HPP_INCLUDE

#include <cstring>
#include <fstream>
#include <new>
#include <type_traits>
#include <utility>

namespace serialization {
    // clang-format off
    template <typename T, typename = void>
    struct is_member_serializable : std::false_type {};

    template <typename T>
    struct is_member_serializable<T, std::enable_if_t<std::is_same_v<void, decltype(std::declval<T>().serialize(std::declval<std::ofstream>()))>>>
        : std::true_type {};

    template <typename T, typename = void>
    struct is_member_deserializable : std::false_type {};

    template <typename T>
    struct is_member_deserializable<T, std::enable_if_t<std::is_same_v<void, decltype(std::declval<T>().deserialize(std::declval<std::ifstream>()))>>>
        : std::true_type {};

    template <typename T, typename = void>
    struct is_in_place_deserializable : std::false_type {};

    template <typename T>
    struct is_in_place_deserializable<T, std::enable_if_t<std::is_same_v<void, decltype(serialization::deserialize(std::declval<std::ifstream>(), std::declval<T>()))>>> 
        : std::true_type {};

    template<typename T>
    struct use_default_deserialize : std::false_type {};
    // clang-format on

    template <typename T>
    constexpr bool is_in_place_deserializable_v = is_in_place_deserializable<T>::value;

    template <typename T>
    void serialize(std::ostream& out, T const& obj) {
        constexpr uint32_t data_size = sizeof(T);
        char const* data = reinterpret_cast<char const*>(std::launder(&obj));
        out.write(data, data_size);
    }

    template <typename T>
    std::enable_if_t<serialization::use_default_deserialize<T>::value, void> deserialize(std::istream& in, T& obj) {
        constexpr uint32_t data_size = sizeof(T);
        char* data = reinterpret_cast<char*>(std::launder(&obj));
        in.read(data, data_size);
    }

    template <typename T>
    void deserialize(std::istream& in, T*);

    namespace detail {
        template <typename T>
        void read(std::istream& in, T& val) {
            in.read(reinterpret_cast<char*>(std::launder(&val)), sizeof(T));
        }

        template <typename T>
        void write(std::ostream& out, T const& val) {
            out.write(reinterpret_cast<char const*>(std::launder(&val)), sizeof(T));
        }
    } // namespace detail
} // namespace serialization

#endif // !CORE_SERIALIZATION_HPP_INCLUDE
