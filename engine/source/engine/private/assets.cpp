#include "assets.hpp"

#include "glad/glad.h"
#include "stb/stb_image.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "debug_macros.hpp"
#include "mesh/mesh.hpp"
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
    return Shader_File(shader_type_from_filename(path), shader_source);
}

void Assets::load_shader_file_and_attach(Shader& shader, std::filesystem::path const& path) {
    std::filesystem::path full_path = utils::concat_paths(_shaders_path, path);
    std::string shader_source;
    read_file_raw(full_path, shader_source);
    Shader_File s(shader_type_from_filename(path), shader_source);
    shader.attach(s);
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

uint32_t Assets::load_texture(std::filesystem::path filename) {
    int width, height, channels;
    int32_t desired_channel_count = 4;
    //stbi_set_flip_vertically_on_load(true);
    std::string path = utils::concat_paths(_assets_path, filename).string();
    unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image_data) {
        throw std::runtime_error("Image not loaded");
    }
    GLuint texture;
    glGenTextures(1, &texture);
    CHECK_GL_ERRORS();
    glBindTexture(GL_TEXTURE_2D, texture);
    CHECK_GL_ERRORS();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    CHECK_GL_ERRORS();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    CHECK_GL_ERRORS();
    stbi_image_free(image_data);
    return texture;
}

static void load_material_textures(aiMaterial* mat, aiTextureType type, std::filesystem::path const& current_path, std::vector<Texture>& textures) {
    for (std::size_t i = 0, texture_count = mat->GetTextureCount(type); i < texture_count; ++i) {
        aiString str;
        mat->GetTexture(type, static_cast<unsigned int>(i), &str);
        auto texture_path = utils::concat_paths(current_path, str.C_Str());
        Texture texture;
        texture.id = Assets::load_texture(texture_path);
        texture.type = (type == aiTextureType_DIFFUSE ? Texture_Type::diffuse : Texture_Type::specular);
        textures.push_back(std::move(texture));
    }
}

static Mesh process_mesh(aiMesh* mesh, aiScene const* scene, std::filesystem::path const& current_path) {
    std::vector<Vertex> vertices(mesh->mNumVertices);
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
        vertices[i].position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertices[i].normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
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
