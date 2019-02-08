#include "assets.hpp"

#include "glad/glad.h"
#include "stb/stb_image.hpp"

#include "debug_macros.hpp"
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

Shader_type Assets::shader_type_from_filename(std::filesystem::path const& filename) {
    std::string extension(filename.extension().string());
    if (extension == ".vert") {
        return Shader_type::vertex;
    } else if (extension == ".frag") {
        return Shader_type::fragment;
    } else if (extension == ".geom") {
        return Shader_type::geometry;
    } else if (extension == ".comp") {
        return Shader_type::compute;
    } else if (extension == ".tese") {
        return Shader_type::tessellation_evaluation;
    } else if (extension == ".tesc") {
        return Shader_type::tessellation_control;
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
    CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, texture);
    CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    CHECK_GL_ERRORS
    stbi_image_free(image_data);
    return texture;
}