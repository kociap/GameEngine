#include <assets.hpp>

#include <glad/glad.h>
#include <stb/stb_image.hpp>

#include <containers/vector.hpp>
#include <debug_macros.hpp>
#include <importers/obj.hpp>
#include <importers/png.hpp>
#include <math/vector3.hpp>
#include <mesh/mesh.hpp>
#include <opengl.hpp>
#include <postprocess.hpp>
#include <shader.hpp>
#include <utils/filesystem.hpp>

#include <fstream>
#include <stdexcept>

namespace assets {
    static std::filesystem::path _current_path("");
    static std::filesystem::path _assets_path("");
    static std::filesystem::path _shaders_path("");

    void init(std::filesystem::path path, std::filesystem::path assets, std::filesystem::path shaders) {
        _current_path = std::move(path);
        _assets_path = std::move(assets);
        _shaders_path = std::move(shaders);
    }

    std::filesystem::path current_path() {
        return _current_path;
    }

    std::string read_file_raw_string(std::filesystem::path const& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::invalid_argument("Could not open file " + filename.string());
        }
        std::string out;
        std::getline(file, out, '\0');
        return out;
    }

    containers::Vector<uint8_t> read_file_raw(std::filesystem::path const& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::invalid_argument("Could not open file " + filename.string());
        }
        file.seekg(0, std::ios::end);
        containers::Vector<uint8_t> file_contents(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(file_contents.data()), file_contents.size());
        return file_contents;
    }

    opengl::Shader_Type shader_type_from_filename(std::filesystem::path const& filename) {
        std::string extension(filename.extension().string());
        if (extension == ".vert") {
            return opengl::Shader_Type::vertex_shader;
        } else if (extension == ".frag") {
            return opengl::Shader_Type::fragment_shader;
        } else if (extension == ".geom") {
            return opengl::Shader_Type::geometry_shader;
        } else if (extension == ".comp") {
            return opengl::Shader_Type::compute_shader;
        } else if (extension == ".tese") {
            return opengl::Shader_Type::tessellation_evaluation_shader;
        } else if (extension == ".tesc") {
            return opengl::Shader_Type::tessellation_control_shader;
        } else {
            throw std::invalid_argument("\"" + extension + "\" is not a known shader file extension");
        }
    }

    Shader_File load_shader_file(std::filesystem::path const& path) {
        std::filesystem::path full_path = utils::concat_paths(_shaders_path, path);
        std::string shader_source = read_file_raw_string(full_path);
        return Shader_File(path.string(), shader_type_from_filename(path), shader_source);
    }

    static opengl::Format get_matching_pixel_format(int channels) {
        switch (channels) {
            case 1:
                return opengl::Format::red;
            case 2:
                return opengl::Format::rg;
            case 3:
                return opengl::Format::rgb;
            default:
                return opengl::Format::rgba;
        }
    }

    uint32_t load_cubemap(containers::Vector<std::filesystem::path> const& paths) {
        if (paths.size() != 6) {
            throw std::invalid_argument("The number of paths provided is not 6");
        }

        GLuint cubemap;
        glGenTextures(1, &cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        int32_t width;
        int32_t height;
        int32_t channels;
        for (std::size_t i = 0; i < 6; ++i) {
            std::string path = utils::concat_paths(_assets_path, paths[i]).string();
            uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            if (!data) {
                glDeleteTextures(1, &cubemap);
                throw std::runtime_error("Could not load image " + paths[i].string());
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return cubemap;
    };

    uint32_t load_texture(std::filesystem::path filename, bool flip) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(flip);
        std::string path = utils::concat_paths(_assets_path, filename).string();
        unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!image_data) {
            throw std::runtime_error("Image not loaded");
        }
        uint32_t texture;
        opengl::Sized_Internal_Format internal_format = opengl::Sized_Internal_Format::rgba8;
        opengl::Format format = get_matching_pixel_format(channels);
        opengl::gen_textures(1, &texture);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, texture);
        opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, width, height, format, opengl::Type::unsigned_byte, image_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_GL_ERRORS();
        opengl::generate_mipmap(GL_TEXTURE_2D);
        stbi_image_free(image_data);
        return texture;
    }

    uint32_t load_srgb_texture(std::filesystem::path filename, bool flip) {
        if (filename.extension() == ".png") {
            //if (false) {
            std::string path = utils::concat_paths(_assets_path, filename).string();
            auto png_data = read_file_raw(path);
            importers::Image_Data image_data = importers::import_png(png_data);
            uint32_t texture;
            opengl::gen_textures(1, &texture);
            opengl::bind_texture(opengl::Texture_Type::texture_2D, texture);
            opengl::Format format;
            opengl::Sized_Internal_Format internal_format;
            switch (image_data.pixel_format) {
                case importers::Image_Pixel_Format::grey8:
                case importers::Image_Pixel_Format::grey16: {
                    format = opengl::Format::red;
                    internal_format = image_data.pixel_format == importers::Image_Pixel_Format::grey16 ? opengl::Sized_Internal_Format::r16 :
                                                                                                         opengl::Sized_Internal_Format::r8;
                    int32_t swizzle_mask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
                    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                    break;
                }
                case importers::Image_Pixel_Format::grey8_alpha8:
                case importers::Image_Pixel_Format::grey16_alpha16: {
                    format = opengl::Format::rg;
                    internal_format = image_data.pixel_format == importers::Image_Pixel_Format::grey16_alpha16 ? opengl::Sized_Internal_Format::rg16 :
                                                                                                                 opengl::Sized_Internal_Format::rg8;
                    int32_t swizzle_mask[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
                    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                    break;
                }
                case importers::Image_Pixel_Format::rgb8:
                    format = opengl::Format::rgb;
                    internal_format = opengl::Sized_Internal_Format::rgb8;
                    break;
                case importers::Image_Pixel_Format::rgb16:
                    format = opengl::Format::rgb;
                    internal_format = opengl::Sized_Internal_Format::rgb16;
                    break;
                case importers::Image_Pixel_Format::rgba8:
                    format = opengl::Format::rgba;
                    internal_format = opengl::Sized_Internal_Format::rgba8;
                    break;
                case importers::Image_Pixel_Format::rgba16:
                    format = opengl::Format::rgba;
                    internal_format = opengl::Sized_Internal_Format::rgba16;
                    break;
                default:
                    GE_log("Unsupported png pixel format");
            }
            opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, image_data.width, image_data.height, format, opengl::Type::unsigned_byte,
                                 image_data.data.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            CHECK_GL_ERRORS();
            opengl::generate_mipmap(GL_TEXTURE_2D);
            return texture;
        } else {
            int width, height, channels;
            stbi_set_flip_vertically_on_load(flip);
            std::string path = utils::concat_paths(_assets_path, filename).string();
            unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            if (!image_data) {
                throw std::runtime_error("Image not loaded");
            }
            containers::Vector<uint8_t> pixel_preview(width * height * channels);
            memory::copy(image_data, image_data + (width * height * channels), pixel_preview.data());
            uint32_t texture;
            opengl::gen_textures(1, &texture);
            opengl::bind_texture(opengl::Texture_Type::texture_2D, texture);
            // TODO
            // What was that todo supposed to mean?????
            opengl::Sized_Internal_Format internal_format = opengl::Sized_Internal_Format::srgb8_alpha8;
            opengl::Format format = get_matching_pixel_format(channels);
            opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, width, height, format, opengl::Type::unsigned_byte, image_data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            CHECK_GL_ERRORS();
            opengl::generate_mipmap(GL_TEXTURE_2D);
            stbi_image_free(image_data);
            return texture;
        }
    }

    static void compute_tangents(Vertex& vert1, Vertex& vert2, Vertex& vert3) {
        Vector3 delta_pos1 = vert2.position - vert1.position;
        Vector3 delta_pos2 = vert3.position - vert1.position;
        Vector2 delta_uv1 = vert2.uv_coordinates - vert1.uv_coordinates;
        Vector2 delta_uv2 = vert3.uv_coordinates - vert1.uv_coordinates;
        float determinant = delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x;
        Vector3 tangent = math::normalize((delta_uv2.y * delta_pos1 - delta_uv1.y * delta_pos2) / determinant);
        Vector3 bitangent = math::normalize((delta_uv1.x * delta_pos2 - delta_uv2.x * delta_pos1) / determinant);

        vert1.tangent = vert2.tangent = vert3.tangent = tangent;
        vert1.bitangent = vert2.bitangent = vert3.bitangent = bitangent;
        //vert1.bitangent = vert2.bitangent = vert3.bitangent = Vector3::cross(vert1.normal, vert1.tangent);
    }

    static Mesh process_mesh(importers::Mesh const& imported_mesh) {
        containers::Vector<Vertex> vertices(imported_mesh.vertices.size());
        containers::Vector<uint32_t> indices;
        if (imported_mesh.texture_coordinates.size() != 0) {
            for (uint64_t i = 0; i < imported_mesh.vertices.size(); ++i) {
                vertices[i].position = imported_mesh.vertices[i];
                vertices[i].normal = imported_mesh.normals[i];
                vertices[i].uv_coordinates = Vector2(imported_mesh.texture_coordinates[i]);
            }
        } else {
            for (uint64_t i = 0; i < imported_mesh.vertices.size(); ++i) {
                vertices[i].position = imported_mesh.vertices[i];
                vertices[i].normal = imported_mesh.normals[i];
            }
        }

        for (importers::Face const& imported_face: imported_mesh.faces) {
            for (uint32_t index: imported_face.indices) { // TODO use vector's assign once I implement it correctly
                indices.push_back(index);
            }
        }

        for (uint64_t i = 0; i < indices.size(); i += 3) {
            compute_tangents(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
        }

        return {std::move(vertices), std::move(indices)};
    }

    containers::Vector<Mesh> load_model(std::filesystem::path const& path) {
        auto asset_path = utils::concat_paths(_assets_path, path);
        auto extension = asset_path.extension();
        containers::Vector<Mesh> meshes;
        if (extension == ".obj") {
            //if (false) {
            auto file = utils::read_file_binary(asset_path);
            auto imported_meshes = importers::import_obj(file);
            for (auto& imported_mesh: imported_meshes) {
                flip_texture_coordinates(imported_mesh.texture_coordinates);
                meshes.push_back(process_mesh(imported_mesh));
            }
        } else {
            throw std::runtime_error("Unsupported file format");
        }
        return meshes;
    }
} // namespace assets