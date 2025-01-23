#pragma once

#include <anton/array.hpp>
#include <core/exception.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
  constexpr u64 jpeg_header = 0xFFD8;

  class Unknown_Critical_Chunk: public Exception {
  public:
    Unknown_Critical_Chunk(): Exception("") {}
  };

  class Invalid_Image_File: public Exception {
    using Exception::Exception;
  };

  class Decompression_Failure: public Exception {
    using Exception::Exception;
  };

  enum class Image_Pixel_Format {
    grey8,
    grey16,
    grey8_alpha8,
    grey16_alpha16,
    rgb8,
    rgb16,
    rgba8,
    rgba16,
  };

  enum Image_Color_Space {
    gamma_encoded,
    srgb,
    linear,
  };

  class Image {
  public:
    u32 width;
    u32 height;
    float gamma;
    Image_Pixel_Format pixel_format;
    Image_Color_Space color_space;
    anton::Array<u8> data;

    Image() {}
    Image(u32 width, u32 height, Image_Pixel_Format pixel_format,
          Image_Color_Space color_space, float gamma, anton::Array<u8>&& data)
      : width(width), height(height), gamma(gamma), pixel_format(pixel_format),
        color_space(color_space), data(ANTON_MOV(data))
    {
    }
  };
} // namespace anton_engine::importers
