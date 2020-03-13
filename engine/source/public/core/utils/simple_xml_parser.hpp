#include <core/atl/vector.hpp>
#include <istream>
#include <string>

namespace anton_engine::utils {
    namespace xml {
        enum class Tag_Type { self_closing, opening, closing };

        struct Tag_Property {
            std::string name;
            std::string value;
        };

        struct Tag {
            std::string name;
            atl::Vector<Tag_Property> properties;
            Tag_Type type;
        };

        atl::Vector<Tag> parse(std::string const&);
    } // namespace xml
} // namespace anton_engine::utils
