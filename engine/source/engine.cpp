#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <cmath>
#include <iostream>

#include "math/math.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "program.hpp"
#include "shader.hpp"
#include "stb/stb_image.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

void read_file(std::filesystem::path const& filename, std::string& out) {
    std::ifstream file(filename);
    if (file) {
        file.seekg(0, std::ios::end);
        out.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&out[0], out.size());
        file.close();
    } else {
        throw std::invalid_argument("Could not open file " + filename.string());
    }
}

Shader_type shader_type_from_filename(std::filesystem::path const& filename) {
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

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main() {
    constexpr GLuint window_width = 800;
    constexpr GLuint window_height = 600;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "GameEngine", NULL, NULL);
    if (!window) {
        std::cout << "GLFW failed to create a window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "GLAD not initialized\n";
        glfwTerminate();
        return -2;
    }

    glViewport(0, 0, window_width, window_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLfloat vertices[] = {
        // position        rgb               texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  //
    };

    GLuint indices[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    GLfloat texture_coordinates[] = {
        0.0f, 0.0f, //
        1.0f, 0.0f, //
        0.5f, 1.0f  //
    };

    // Buffers
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // rgb
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Load, compile and link shaders
    std::vector<std::filesystem::path> shader_paths(
        {"C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert", "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag"});

    Program program;
    for (std::filesystem::path const& path : shader_paths) {
        std::string shader_source;
        read_file(path, shader_source);
        Shader shader(shader_type_from_filename(path), shader_source);
        program.attach(shader);
    }
    program.link();

    auto load_texture = [](char const* filename) -> GLuint {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image_data = stbi_load(filename, &width, &height, &channels, 0);
        if (!image_data) {
            throw std::runtime_error("Image not loaded");
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image_data);
        return texture;
    };

    GLuint wall_tex = load_texture("C:/Users/An0num0us/Documents/GameEngine/wall.jpg");
    GLuint face_tex = load_texture("C:/Users/An0num0us/Documents/GameEngine/awesomeface.jpg");

    Matrix4 model = transform::rotate_x(math::radians(-55.0f));
    Matrix4 view = transform::translate({0, -1.0f, 3.0f});
    Matrix4 projection = transform::perspective(math::radians(45.0f), window_width / window_height, 0.1f, 100.0f);
    //Matrix4 projection = transform::orthographic(-1.0f, 1.0f, -1.0f, 1.0f, -0.0f, -100.0f);

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.5f, 0.7f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        glUniform1i(program.get_uniform("frag_texture0"), 0);
        glUniform1i(program.get_uniform("frag_texture1"), 1);
        Matrix4 transform_matrix = model * view * projection;
        glUniformMatrix4fv(program.get_uniform("transform"), 1, GL_FALSE, transform_matrix.get_raw());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_tex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, face_tex);

        glBindVertexArray(vao);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
