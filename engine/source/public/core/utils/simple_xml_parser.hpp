#include <core/atl/vector.hpp>
#include <core/atl/string.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine::utils {
    namespace xml {
        enum class Tag_Type { self_closing, opening, closing };

        struct Tag_Property {
            atl::String name;
            atl::String value;
        };

        struct Tag {
            atl::String name;
            atl::Vector<Tag_Property> properties;
            Tag_Type type;
        };

        atl::Vector<Tag> parse(atl::String_View file);
    } // namespace xml
} // namespace anton_engine::utils
