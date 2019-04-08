#include "assets.hpp"

#include "glad/glad.h"
#include "stb/stb_image.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "debug_macros.hpp"
#include "math/vector3.hpp"
#include "mesh/mesh.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "utils/path.hpp"

#include <fstream>
#include <stdexcept>

std::filesystem::path Assets::_current_path("");
std::filesystem::path Assets::_assets_path("");
std::filesystem::path Assets::_shaders_path("");

void Assets::init(std::filesystem::path path, std::filesystem::path assets, std::filesystem::path shaders) {
    _current_path = std::move(path);
    _assets_path = std::move(assets);
    _shaders_path = std::move(shaders);
}

std::filesystem::path Assets::current_path() {
    return _current_path;
}

void Assets::read_file_raw(std::filesystem::path const& filename, std::string& out) {
    std::ifstream file(filename);
    if (file) {
        std::getline(file, out, '\0');
        file.close();
    } else {
        throw std::invalid_argument("Could not open file " + filename.string());
    }
}

Shader_Type Assets::shader_type_from_filename(std::filesystem::path const& filename) {
    std::string extension(filename.extension().string());
    if (extension == ".vert") {
        return Shader_Type::vertex;
    } else if (extension == ".frag") {
        return Shader_Type::fragment;
    } else if (extension == ".geom") {
        return Shader_Type::geometry;
    } else if (extension == ".comp") {
        return Shader_Type::compute;
    } else if (extension == ".tese") {
        return Shader_Type::tessellation_evaluation;
    } else if (extension == ".tesc") {
        return Shader_Type::tessellation_control;
    } else {
        throw std::invalid_argument("\"" + extension + "\" is not a known shader file extension");
    }
}

Shader_File Assets::load_shader_file(std::filesystem::path const& path) {
    std::filesystem::path full_path = utils::concat_paths(_shaders_path, path);
    std::string shader_source;
    read_file_raw(full_path, shader_source);
    return Shader_File(path.string(), shader_type_from_filename(path), shader_source);
}

void Assets::load_shader_file_and_attach(Shader& shader, std::filesystem::path const& path) {
    std::filesystem::path full_path = utils::concat_paths(_shaders_path, path);
    std::string shader_source;
    read_file_raw(full_path, shader_source);
    Shader_File s(path.string(), shader_type_from_filename(path), shader_source);
    shader.attach(s);
}

static opengl::texture::Format get_matching_pixel_format(int channels) {
    // clang-format off
    if(channels == 1) { return opengl::texture::Format::red; } 
    else if(channels == 2) { return opengl::texture::Format::rg; } 
    else if(channels == 3) { return opengl::texture::Format::rgb; } 
    else { return opengl::texture::Format::rgba; }
    // clang-format on
}

uint32_t Assets::load_cubemap(std::vector<std::filesystem::path> const& paths) {
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

uint32_t Assets::load_texture(std::filesystem::path filename, bool flip) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    std::string path = utils::concat_paths(_assets_path, filename).string();
    unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image_data) {
        throw std::runtime_error("Image not loaded");
    }
    uint32_t texture;
    opengl::texture::Sized_Internal_Format internal_format = opengl::texture::Sized_Internal_Format::rgba8;
    opengl::texture::Format format = get_matching_pixel_format(channels);
    opengl::gen_textures(1, &texture);
    opengl::bind_texture(GL_TEXTURE_2D, texture);
    opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, width, height, format, opengl::texture::Type::unsigned_byte, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERRORS();
    opengl::generate_mipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);
    return texture;
}

uint32_t Assets::load_srgb_texture(std::filesystem::path filename, bool flip) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    std::string path = utils::concat_paths(_assets_path, filename).string();
    unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image_data) {
        throw std::runtime_error("Image not loaded");
    }
    uint32_t texture;
    opengl::gen_textures(1, &texture);
    opengl::bind_texture(GL_TEXTURE_2D, texture);
    // TODO
    opengl::texture::Sized_Internal_Format internal_format = opengl::texture::Sized_Internal_Format::srgb8_alpha8;
    opengl::texture::Format format = get_matching_pixel_format(channels);
    opengl::tex_image_2D(GL_TEXTURE_2D, 0, internal_format, width, height, format, opengl::texture::Type::unsigned_byte, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERRORS();
    opengl::generate_mipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);
    return texture;
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

static void load_material_textures(aiMaterial* mat, aiTextureType type, std::filesystem::path const& current_path, std::vector<Texture>& textures) {
    for (std::size_t i = 0, texture_count = mat->GetTextureCount(type); i < texture_count; ++i) {
        aiString str;
        mat->GetTexture(type, static_cast<unsigned int>(i), &str);
        auto texture_path = utils::concat_paths(current_path, str.C_Str());
        Texture texture;
        if (type == aiTextureType_DIFFUSE) {
            texture.type = Texture_Type::diffuse;
            texture.id = Assets::load_srgb_texture(texture_path, false);
        } else if (type == aiTextureType_SPECULAR) {
            texture.type = Texture_Type::specular;
            texture.id = Assets::load_texture(texture_path, false);
        } else if (type == aiTextureType_NORMALS) {
            texture.type = Texture_Type::normal;
            texture.id = Assets::load_texture(texture_path, false);
        } else {
            throw std::runtime_error("Unknown texture type");
        }
        textures.push_back(std::move(texture));
    }
}

static Mesh process_mesh(aiMesh* mesh, aiScene const* scene, std::filesystem::path const& current_path) {
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
        auto& vert = mesh->mVertices[i];
        vertices[i].position = Vector3(vert.x, vert.y, vert.z);
        auto& normal = mesh->mNormals[i];
        vertices[i].normal = Vector3(normal.x, normal.y, normal.z);
        if (mesh->mTextureCoords[0]) {
            vertices[i].uv_coordinates = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
    }

    for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
        aiFace& face = mesh->mFaces[i];
        for (std::size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        compute_tangents(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        load_material_textures(material, aiTextureType_DIFFUSE, current_path, textures);
        load_material_textures(material, aiTextureType_SPECULAR, current_path, textures);
    }

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

static void process_node(std::vector<Mesh>& meshes, aiNode* node, aiScene const* scene, std::filesystem::path const& current_path) {
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene, current_path));
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i) {
        process_node(meshes, node->mChildren[i], scene, current_path);
    }
}

std::vector<Mesh> Assets::load_model(std::filesystem::path const& path) {
    auto asset_path = utils::concat_paths(_assets_path, path);
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(asset_path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

    asset_path.remove_filename();

    std::vector<Mesh> meshes;
    process_node(meshes, scene->mRootNode, scene, asset_path);
    return meshes;
}
