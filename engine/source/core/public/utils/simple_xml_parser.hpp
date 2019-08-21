#include <anton_stl/vector.hpp>
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
            anton_stl::Vector<Tag_Property> properties;
            Tag_Type type;
        };

        anton_stl::Vector<Tag> parse(std::string const&);
    } // namespace xml
} // namespace utils
