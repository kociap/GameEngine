#include "mesh/model.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "debug_macros.hpp"
#include "stb/stb_image.hpp"
#include <stdexcept>

void Model::draw(Shader& shader) {
    for (Mesh& mesh : meshes) {
        mesh.draw(shader);
    }
}

void Model::draw_instanced(Shader& shader, uint32_t count) {
    for (Mesh& mesh : meshes) {
        mesh.draw_instanced(shader, count);
    }
}

static std::filesystem::path current_model_path;

static uint32_t load_texture(std::filesystem::path filename) {
    int width, height, channels;
    int32_t desired_channel_count = 4;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* image_data = stbi_load(filename.string().c_str(), &width, &height, &channels, desired_channel_count);
    if (!image_data) {
        throw std::runtime_error("Image not loaded");
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);
    return texture;
};

static void load_material_textures(aiMaterial* mat, aiTextureType type, std::vector<Texture>& textures) {
    for (std::size_t i = 0, texture_count = mat->GetTextureCount(type); i < texture_count; ++i) {
        aiString str;
        mat->GetTexture(type, static_cast<unsigned int>(i), &str);
        auto texture_path(current_model_path);
        texture_path += str.C_Str();
        Texture texture;
        texture.id = load_texture(texture_path);
        texture.type = (type == aiTextureType_DIFFUSE ? Texture_Type::diffuse : Texture_Type::specular);
        textures.push_back(std::move(texture));
    }
}

static Mesh process_mesh(aiMesh* mesh, aiScene const* scene) {
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
        load_material_textures(material, aiTextureType_DIFFUSE, textures);
        load_material_textures(material, aiTextureType_SPECULAR, textures);
    }

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

static void process_node(std::vector<Mesh>& meshes, aiNode* node, aiScene const* scene) {
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i) {
        process_node(meshes, node->mChildren[i], scene);
    }
}

Model Model::load_from_file(std::filesystem::path const& path) {
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

    current_model_path = path;
    current_model_path.remove_filename();

    Model model;
    process_node(model.meshes, scene->mRootNode, scene);
    return model;
}
