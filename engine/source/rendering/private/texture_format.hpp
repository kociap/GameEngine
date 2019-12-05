#ifndef RENDERING_TEXTURE_FORMAT_HPP_INCLUDE
#define RENDERING_TEXTURE_FORMAT_HPP_INCLUDE

#include <anton_int.hpp>

namespace anton_engine {
    struct Texture_Format {
        u32 width;
        u32 height;
        u32 sized_internal_format;
        u32 pixel_format;
        u32 pixel_type;
        u32 filter;
        i32 mip_levels;
        i32 swizzle_mask[4];
    };
} // namespace anton_engine

#endif // !RENDERING_TEXTURE_FORMAT_HPP_INCLUDE
