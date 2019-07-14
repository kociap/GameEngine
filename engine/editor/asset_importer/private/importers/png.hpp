#ifndef ENGINE_IMPORTERS_PNG_HPP_INCLUDE
#define ENGINE_IMPORTERS_PNG_HPP_INCLUDE

#include <importers/image.hpp>

namespace importers {
    bool test_png(containers::Vector<uint8_t> const&);
    Image_Data import_png(containers::Vector<uint8_t> const&);
} // namespace importers

#endif // !ENGINE_IMPORTERS_PNG_HPP_INCLUDE
