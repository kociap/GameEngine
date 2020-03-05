#include <content_browser/asset_importing.hpp>

#include <core/stl/string.hpp>
#include <core/stl/vector.hpp>
#include <content_browser/importers/image.hpp>
#include <content_browser/importers/mesh.hpp>
#include <content_browser/importers/obj.hpp>
#include <content_browser/importers/png.hpp>
#include <content_browser/importers/tga.hpp>
#include <core/math/math.hpp>
#include <rendering/opengl.hpp>
#include <rendering/opengl_enums_defs.hpp>
#include <core/paths.hpp>
#include <rendering/texture_format.hpp>
#include <core/utils/filesystem.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace anton_engine::asset_importing {
    struct Matching_Format {
        opengl::Format format;
        opengl::Sized_Internal_Format internal_format;
        int32_t swizzle_mask[4];
    };

    [[nodiscard]] static Matching_Format get_matching_texture_format(importers::Image const& image) {
        using Internal_Format = opengl::Sized_Internal_Format;
        switch (image.pixel_format) {
            case importers::Image_Pixel_Format::grey8:
            case importers::Image_Pixel_Format::grey16: {
                Internal_Format const internal_format =
                    image.pixel_format == importers::Image_Pixel_Format::grey16 ? Internal_Format::r16 : Internal_Format::r8;
                return {opengl::Format::red, internal_format, {GL_RED, GL_RED, GL_RED, GL_ONE}};
            }
            case importers::Image_Pixel_Format::grey8_alpha8:
            case importers::Image_Pixel_Format::grey16_alpha16: {
                Internal_Format const internal_format =
                    image.pixel_format == importers::Image_Pixel_Format::grey16_alpha16 ? Internal_Format::rg16 : Internal_Format::rg8;
                return {opengl::Format::rg, internal_format, {GL_RED, GL_RED, GL_RED, GL_GREEN}};
            }
            // TODO: srgb might be used for non-color data, e.g. normal maps
            case importers::Image_Pixel_Format::rgb8:
                return {opengl::Format::rgb, Internal_Format::srgb8, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
            case importers::Image_Pixel_Format::rgb16:
                return {opengl::Format::rgb, Internal_Format::rgb16, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
            case importers::Image_Pixel_Format::rgba8:
                return {opengl::Format::rgba, Internal_Format::srgb8_alpha8, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
            case importers::Image_Pixel_Format::rgba16:
                return {opengl::Format::rgba, Internal_Format::rgba16, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
                //default:
                //    throw std::runtime_error("Unsupported png pixel format");
        }
    }

    [[nodiscard]] static int32_t compute_number_of_mipmaps(uint32_t const width, uint32_t const height) {
        int32_t mipmap_levels = 0;
        uint32_t max_dim = math::max(width, height);
        while (max_dim > 1) {
            max_dim /= 2;
            mipmap_levels += 1;
        }
        return mipmap_levels;
    }

    // TODO: Preprocess textures to limit the number of possible texture formats
    static void write_texture(std::filesystem::path const& output_directory, std::filesystem::path const& file_original_path, importers::Image const& image) {
        uint64_t identifier = 0; // TODO generate identifier
        Texture_Format format;
        format.width = image.width;
        format.height = image.height;
        Matching_Format const texture_format = get_matching_texture_format(image);
        format.pixel_format = utils::enum_to_value(texture_format.format);
        format.sized_internal_format = utils::enum_to_value(texture_format.internal_format);
        format.mip_levels = compute_number_of_mipmaps(format.width, format.height);
        memcpy(format.swizzle_mask, texture_format.swizzle_mask, sizeof(texture_format.swizzle_mask));
        // TODO: Hardcoded values. Should be customizable.
        format.pixel_type = GL_UNSIGNED_BYTE;
        format.filter = GL_NEAREST_MIPMAP_NEAREST;
        int64_t const image_bytes = image.data.size();
        int64_t const texture_chunk_size = static_cast<int64_t>(sizeof(Texture_Format)) + 8 + image_bytes;

        std::filesystem::path const out_file_path = utils::concat_paths(output_directory, file_original_path.stem().generic_string() + ".getex");
        std::ofstream file(out_file_path, std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<char const*>(&texture_chunk_size), 8); // Allows to skip the entire texture chunk without parsing it
        file.write(reinterpret_cast<char const*>(&identifier), 8);
        file.write(reinterpret_cast<char const*>(&format), sizeof(Texture_Format));
        file.write(reinterpret_cast<char const*>(&image_bytes), 8); // Prefix the image with its size in bytes because that will simplify mipmap access
        file.write(reinterpret_cast<char const*>(image.data.data()), image_bytes);
    }

    void import_image(std::filesystem::path const& path) {
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

        throw std::runtime_error("Unsupported file format");
    }

    static void compute_tangents(Vertex& vert1, Vertex& vert2, Vertex& vert3) {
        Vector3 const delta_pos1 = vert2.position - vert1.position;
        Vector3 const delta_pos2 = vert3.position - vert1.position;
        Vector2 const delta_uv1 = vert2.uv_coordinates - vert1.uv_coordinates;
        Vector2 const delta_uv2 = vert3.uv_coordinates - vert1.uv_coordinates;
        float const determinant = delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x;
        Vector3 const tangent = math::normalize((delta_uv2.y * delta_pos1 - delta_uv1.y * delta_pos2) / determinant);
        Vector3 const bitangent = math::normalize((delta_uv1.x * delta_pos2 - delta_uv2.x * delta_pos1) / determinant);

        vert1.tangent = vert2.tangent = vert3.tangent = tangent;
        vert1.bitangent = vert2.bitangent = vert3.bitangent = bitangent;
        //vert1.bitangent = vert2.bitangent = vert3.bitangent = Vector3::cross(vert1.normal, vert1.tangent);
    }

    static Mesh process_mesh(importers::Mesh const& imported_mesh) {
        anton_stl::Vector<Vertex> vertices(imported_mesh.vertices.size());
        anton_stl::Vector<u32> indices;
        if (imported_mesh.texture_coordinates.size() != 0) {
            for (isize i = 0; i < imported_mesh.vertices.size(); ++i) {
                vertices[i].position = imported_mesh.vertices[i];
                vertices[i].normal = imported_mesh.normals[i];
                vertices[i].uv_coordinates = Vector2(imported_mesh.texture_coordinates[i]);
            }
        } else {
            for (isize i = 0; i < imported_mesh.vertices.size(); ++i) {
                vertices[i].position = imported_mesh.vertices[i];
                vertices[i].normal = imported_mesh.normals[i];
            }
        }

        for (importers::Face const& imported_face: imported_mesh.faces) {
            for (u32 const index: imported_face.indices) {
                indices.push_back(index);
            }
        }

        for (isize i = 0; i < indices.size(); i += 3) {
            compute_tangents(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
        }

        return {std::move(vertices), std::move(indices)};
    }

    Imported_Meshes import_mesh(std::filesystem::path const& path) {
        anton_stl::Vector<u8> const file = utils::read_file_binary(path);
        std::string const extension = path.extension().generic_string();
        if (importers::test_obj(extension.data(), file)) {
            anton_stl::Vector<importers::Mesh> meshes = importers::import_obj(file);

            Imported_Meshes imported_meshes;
            imported_meshes.hierarchy.resize(meshes.size(), -1);
            for (importers::Mesh const& mesh: meshes) {
                imported_meshes.meshes.push_back(process_mesh(mesh));
            }

            return imported_meshes;
        }

        throw std::runtime_error("Unsupported file format");
    }

    void save_meshes(anton_stl::String_View filename, anton_stl::Slice<u64 const> const guids, anton_stl::Slice<Mesh const> meshes) {
        anton_stl::String file_name_with_ext(filename);
        file_name_with_ext.append(u8".mesh");
        std::filesystem::path path = utils::concat_paths(paths::assets_directory(), std::filesystem::path(file_name_with_ext.data()));
        std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc);
        for (isize i = 0; i < guids.size(); ++i) {
            out.write(reinterpret_cast<char const*>(&guids[i]), sizeof(u64));
            i64 const vertex_count = meshes[i].vertices.size();
            out.write(reinterpret_cast<char const*>(&vertex_count), sizeof(i64));
            out.write(reinterpret_cast<char const*>(meshes[i].vertices.data()), vertex_count * sizeof(Vertex));
            i64 const index_count = meshes[i].indices.size();
            out.write(reinterpret_cast<char const*>(&index_count), sizeof(i64));
            out.write(reinterpret_cast<char const*>(meshes[i].indices.data()), index_count * sizeof(u32));
        }
    }
} // namespace anton_engine::asset_importing
