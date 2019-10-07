#ifndef RENDERING_TEXTURE_FORMAT_HPP_INCLUDE
#define RENDERING_TEXTURE_FORMAT_HPP_INCLUDE

#include <cstdint>

namespace anton_engine {
    struct Texture_Format {
        uint32_t width;
        uint32_t height;
        uint32_t sized_internal_format;
        uint32_t pixel_format;
        uint32_t pixel_type;
        uint32_t filter;
        int32_t mip_levels;
        int32_t swizzle_mask[4];
    };
} // namespace anton_engine

#endif // !RENDERING_TEXTURE_FORMAT_HPP_INCLUDE
