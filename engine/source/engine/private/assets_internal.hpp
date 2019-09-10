#ifndef ENGINE_ASSETS_INTERNAL_HPP_INCLUDE
#define ENGINE_ASSETS_INTERNAL_HPP_INCLUDE

#include <cstdint>

namespace anton_engine::assets {
    struct Texture_Header {
        uint64_t identifier;
        uint32_t width;
        uint32_t height;
        uint32_t format;
        uint32_t internal_format;
        uint32_t type;
        uint32_t filter;
        uint64_t swizzle_mask;
    };

    constexpr uint64_t no_swizzle = 0xFFFFFFFFFFFFFFFF;
} // namespace anton_engine::assets

#endif // !ENGINE_ASSETS_INTERNAL_HPP_INCLUDE
