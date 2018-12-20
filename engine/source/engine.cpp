#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <cmath>
#include <iostream>

#include "camera.hpp"
#include "math/math.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "shader.hpp"
#include "stb/stb_image.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <vector>

GLuint window_width = 800;
GLuint window_height = 800;

Matrix4 projection = transform::perspective(math::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
Camera camera;

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
    window_width = width;
    window_height = height;
    projection = transform::perspective(math::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

void mouse_position_callback(GLFWwindow* window, double param_x, double param_y) {
    static float last_x = window_width / 2.0f;
    static float last_y = window_height / 2.0f;
    float x = static_cast<float>(param_x);
    float y = static_cast<float>(param_y);
    float offset_x = x - last_x;
    float offset_y = last_y - y;
    last_x = x;
    last_y = y;

    float sensitivity = 0.05f;
    offset_x *= sensitivity;
    offset_y *= sensitivity;

    static float pitch = 0;
    static float yaw = 0;
    yaw += offset_x;
    pitch += offset_y;
    pitch = std::max(pitch, -89.0f);
    pitch = std::min(pitch, 89.0f);

    camera.front.x = std::cosf(math::radians(yaw)) * std::cosf(math::radians(pitch));
    camera.front.y = std::sinf(math::radians(pitch));
    camera.front.z = std::sinf(math::radians(yaw)) * std::cosf(math::radians(pitch));
    camera.front.normalize();
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    float camera_speed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W)) {
        camera.position += camera.front * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        camera.position -= camera.front * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera.position += Vector3::cross(Vector3::up, camera.front).normalize() * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        camera.position -= Vector3::cross(Vector3::up, camera.front).normalize() * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E)) {
        camera.position += Vector3::up * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        camera.position -= Vector3::up * camera_speed;
    }
}

int main() {
    camera = Camera();

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glEnable(GL_DEPTH_TEST);

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //

        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //

        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //

        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f  //
    };

    // GLuint indices[] = {
    //     0, 1, 3, // 1st triangle
    //     1, 2, 3  // 2nd triangle
    // };

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

    // GLuint ebo;
    // glGenBuffers(1, &ebo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Load, compile and link shaders
    std::vector<std::filesystem::path> shader_paths(
        {"C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert", "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag"});

    Shader shader;
    for (std::filesystem::path const& path : shader_paths) {
        std::string shader_source;
        read_file(path, shader_source);
        Shader_file s(shader_type_from_filename(path), shader_source);
        shader.attach(s);
    }
    shader.link();

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

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    std::vector<Matrix4> model_transforms;
    for (int i = 0; i < 20; ++i) {
        model_transforms.push_back(transform::translate({dist(rng) * 5.0f, dist(rng) * 1.25f, dist(rng) * 5.0f}));
        //model_transforms.push_back(transform::translate({1, 1, -4}));
        // transform::rotate_x(math::radians(dist(rng) * 90.0f)) * transform::rotate_z(math::radians(dist(rng) * 90.0f)) *
    }
    Matrix4 view = Matrix4::identity;
    //Matrix4 projection = transform::orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.5f, 0.7f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glUniform1i(shader.get_uniform("frag_texture0"), 0);
        glUniform1i(shader.get_uniform("frag_texture1"), 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_tex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, face_tex);
        // projection = transform::orthographic(-2, 2, -2, 2, 0.1, 100);
        view = transform::look_at(camera.position, camera.position + camera.front, Vector3::up);

        for (int i = 0; i < 20; ++i) {
            Matrix4 transform_matrix = model_transforms[i] * view * projection;
            shader.setMatrix4("transform", transform_matrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
