#include <asset_importing.hpp>

#include <anton_stl/vector.hpp>
#include <assets_internal.hpp>
#include <debug_macros.hpp>
#include <importers/image.hpp>
#include <importers/mesh.hpp>
#include <importers/obj.hpp>
#include <importers/png.hpp>
#include <importers/tga.hpp>
#include <opengl.hpp>
#include <opengl_enums_defs.hpp>
#include <paths.hpp>
#include <utils/filesystem.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

// _INLINE_VAR constexpr size_t _FNV_offset_basis = 14695981039346656037ULL;
// _INLINE_VAR constexpr size_t _FNV_prime = 1099511628211ULL;

namespace anton_engine::asset_importing {
    struct Texture_Format {
        opengl::Format format;
        opengl::Sized_Internal_Format internal_format;
        uint64_t swizzle_mask;
    };

    static Texture_Format get_matching_texture_format(importers::Image const& image) {
        using Internal_Format = opengl::Sized_Internal_Format;
        opengl::Format format;
        Internal_Format internal_format;
        uint64_t swizzle_mask = assets::no_swizzle;
        switch (image.pixel_format) {
            case importers::Image_Pixel_Format::grey8:
            case importers::Image_Pixel_Format::grey16: {
                format = opengl::Format::red;
                internal_format = image.pixel_format == importers::Image_Pixel_Format::grey16 ? Internal_Format::r16 : Internal_Format::r8;
                swizzle_mask = static_cast<uint64_t>(GL_RED) | (static_cast<uint64_t>(GL_RED) << 16) | (static_cast<uint64_t>(GL_RED) << 32) |
                               (static_cast<uint64_t>(GL_ONE) << 48);
                break;
            }
            case importers::Image_Pixel_Format::grey8_alpha8:
            case importers::Image_Pixel_Format::grey16_alpha16: {
                format = opengl::Format::rg;
                internal_format = image.pixel_format == importers::Image_Pixel_Format::grey16_alpha16 ? Internal_Format::rg16 : Internal_Format::rg8;
                swizzle_mask = static_cast<uint64_t>(GL_RED) | (static_cast<uint64_t>(GL_RED) << 16) | (static_cast<uint64_t>(GL_RED) << 32) |
                               (static_cast<uint64_t>(GL_GREEN) << 48);
                break;
            }
            case importers::Image_Pixel_Format::rgb8:
                format = opengl::Format::rgb;
                internal_format = Internal_Format::srgb8;
                break;
            case importers::Image_Pixel_Format::rgb16:
                format = opengl::Format::rgb;
                internal_format = Internal_Format::rgb16;
                break;
            case importers::Image_Pixel_Format::rgba8:
                format = opengl::Format::rgba;
                internal_format = Internal_Format::srgb8_alpha8; // TODO srgb might be used for non-color data
                break;
            case importers::Image_Pixel_Format::rgba16:
                format = opengl::Format::rgba;
                internal_format = Internal_Format::rgba16;
                break;
                //default:
                //    throw std::runtime_error("Unsupported png pixel format");
        }

        return {format, internal_format, swizzle_mask};
    }

    static void write_texture(std::filesystem::path const& output_directory, std::filesystem::path const& file_original_path, importers::Image const& image) {
        assets::Texture_Header texture_header;
        texture_header.identifier = 0; // TODO generate identifier
        texture_header.width = image.width;
        texture_header.height = image.height;
        Texture_Format const texture_format = get_matching_texture_format(image);
        texture_header.format = utils::enum_to_value(texture_format.format);
        texture_header.internal_format = utils::enum_to_value(texture_format.internal_format);
        texture_header.swizzle_mask = texture_format.swizzle_mask;
        texture_header.type = utils::enum_to_value(opengl::Type::unsigned_byte);
        texture_header.filter = utils::enum_to_value(opengl::Texture_Filter::nearest_mipmap_nearest);
        int64_t const image_bytes = image.data.size();
        int64_t const texture_chunk_size = static_cast<int64_t>(sizeof(assets::Texture_Header)) + 8 + image_bytes;

        std::filesystem::path const out_file_path = utils::concat_paths(output_directory, file_original_path.stem().generic_string() + ".getex");
        std::ofstream file(out_file_path, std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<char const*>(&texture_chunk_size), 8); // Allows to skip the entire texture chunk without parsing it
        file.write(reinterpret_cast<char const*>(&texture_header), sizeof(assets::Texture_Header));
        file.write(reinterpret_cast<char const*>(&image_bytes), 8); // Prefix the image with its size in bytes because that will simplify mipmap access
        file.write(reinterpret_cast<char const*>(image.data.data()), image_bytes);
    }

    void import(std::filesystem::path const& path) {
        // TODO convert from gamma encoded/srgb space to linear
        // TODO include necessary info in the output file
        // TODO meta files
        // TODO support files with multiple images
        // TODO generate mipmaps and save them to the file (if asked to do so)

        anton_stl::Vector<uint8_t> const file = utils::read_file_binary(path);
        if (importers::test_png(file)) {
            importers::Image decoded_image = importers::import_png(file);
            write_texture(paths::assets_directory(), path, decoded_image);
            return;
        }

        if (importers::test_tga(file)) {
            importers::Image decoded_image = importers::import_tga(file);
            write_texture(paths::assets_directory(), path, decoded_image);
            return;
        }

        // if (importers::test_obj(file)) {
        //     anton_stl::Vector<importers::Mesh> meshes = importers::import_obj(file);
        //     write_meshes();
        // }

        throw std::runtime_error("Unsupported file format");
    }
} // namespace anton_engine::asset_importing
