#ifndef ENGINE_DECODERS_PNG_HPP_INCLUDE
#define ENGINE_DECODERS_PNG_HPP_INCLUDE

#include <containers/vector.hpp>
#include <cstdint>
#include <stdexcept>

constexpr uint64_t jpeg_header = 0xFFD8;
constexpr uint64_t png_header = 0x89504E470D0A1A0A;

class Unknown_Critical_Chunk: public std::runtime_error {
public:
    Unknown_Critical_Chunk(): std::runtime_error("") {}
};

class Invalid_Image_File: public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Decompression_Failure: public std::runtime_error {
    using std::runtime_error::runtime_error;
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

class Image_Data {
public:
    uint32_t width;
    uint32_t height;
    Image_Pixel_Format pixel_format;
    containers::Vector<uint8_t> data;

    Image_Data(uint32_t width, uint32_t height, Image_Pixel_Format pixel_format, containers::Vector<uint8_t>&& data)
        : width(width), height(height), pixel_format(pixel_format), data(std::move(data)) {}
};

Image_Data decode_png(containers::Vector<uint8_t> const&);

#endif // !ENGINE_DECODERS_PNG_HPP_INCLUDE
