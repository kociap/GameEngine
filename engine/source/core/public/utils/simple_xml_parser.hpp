#include <istream>
#include <string>
#include <vector>

namespace utils {
    namespace xml {
        enum class Tag_Type { self_closing, opening, closing };

        struct Tag_Property {
            std::string name;
            std::string value;
        };

        struct Tag {
            std::string name;
            std::vector<Tag_Property> properties;
            Tag_Type type;
        };

        std::vector<Tag> parse(std::string const&);
    } // namespace xml
} // namespace utils