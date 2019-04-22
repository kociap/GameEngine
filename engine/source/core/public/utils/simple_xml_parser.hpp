#include "containers/vector.hpp"
#include <istream>
#include <string>

namespace utils {
    namespace xml {
        enum class Tag_Type { self_closing, opening, closing };

        struct Tag_Property {
            std::string name;
            std::string value;
        };

        struct Tag {
            std::string name;
            containers::Vector<Tag_Property> properties;
            Tag_Type type;
        };

        containers::Vector<Tag> parse(std::string const&);
    } // namespace xml
} // namespace utils