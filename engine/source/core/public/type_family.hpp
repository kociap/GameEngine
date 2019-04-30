#ifndef CORE_TYPE_FAMILY_HPP_INCLUDE
#define CORE_TYPE_FAMILY_HPP_INCLUDE

#include "config.hpp"
#include <type_traits>

class Type_Family {
    inline static id_type identifier = static_cast<id_type>(-1);

    template <typename...>
    inline static const id_type family_identifier = ++identifier;

public:
    using family_type = id_type;

    template <typename... Ts>
    inline static const family_type family_id = family_identifier<std::decay_t<Ts>...>;
};

#endif // !CORE_TYPE_FAMILY_HPP_INCLUDE
