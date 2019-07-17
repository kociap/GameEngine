#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_IMAGE_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_IMAGE_HPP_INCLUDE

#include <containers/vector.hpp>
#include <cstdint>
#include <stdexcept>

namespace importers {
    constexpr uint64_t jpeg_header = 0xFFD8;

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

    enum Image_Color_Space {
        gamma_encoded,
        srgb,
        linear,
    };

    class Image_Data {
    public:
        uint32_t width;
        uint32_t height;
        float gamma;
        Image_Pixel_Format pixel_format;
        Image_Color_Space color_space;
        containers::Vector<uint8_t> data;

        Image_Data() {}
        Image_Data(uint32_t width, uint32_t height, Image_Pixel_Format pixel_format, Image_Color_Space color_space, float gamma,
                   containers::Vector<uint8_t>&& data)
            : width(width), height(height), gamma(gamma), pixel_format(pixel_format), color_space(color_space), data(std::move(data)) {}
    };
} // namespace importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_IMAGE_HPP_INCLUDE
