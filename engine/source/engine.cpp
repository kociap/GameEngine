#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <cmath>
#include <iostream>

#include "camera.hpp"
#include "color.hpp"
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

#ifdef _DEBUG
void check_gl_errors() {
    GLenum error = glGetError();
    if (error == GL_INVALID_ENUM) {
        throw std::runtime_error("GL_INVALID_ENUM");
    } else if (error == GL_INVALID_VALUE) {
        throw std::runtime_error("GL_INVALID_VALUE");
    } else if (error == GL_INVALID_OPERATION) {
        throw std::runtime_error("GL_INVALID_OPERATION");
    } else if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
    } else if (error == GL_OUT_OF_MEMORY) {
        throw std::runtime_error("GL_OUT_OF_MEMORY");
    } else if (error == GL_STACK_UNDERFLOW) {
        throw std::runtime_error("GL_STACK_UNDERFLOW");
    } else if (error == GL_STACK_OVERFLOW) {
        throw std::runtime_error("GL_STACK_OVERFLOW");
    }
}
#define CHECK_GL_ERRORS check_gl_errors();
#else
#define CHECK_GL_ERRORS
#endif // _DEBUG

GLuint window_width = 800;
GLuint window_height = 800;

Camera camera;
Matrix4 projection;

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
    projection = transform::perspective(math::radians(camera.fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods) {}

void mouse_position_callback(GLFWwindow*, double param_x, double param_y) {
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

void scroll_callback(GLFWwindow*, double offset_x, double offset_y) {
    camera.fov -= offset_y * 2;
    camera.fov = std::min(camera.fov, 120.0f);
    camera.fov = std::max(camera.fov, 10.0f);
    projection = transform::perspective(math::radians(camera.fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
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
    projection = transform::perspective(math::radians(camera.fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

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
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, //
        0.5f,  -0.5f, -0.5f, //
        0.5f,  0.5f,  -0.5f, //
        0.5f,  0.5f,  -0.5f, //
        -0.5f, 0.5f,  -0.5f, //
        -0.5f, -0.5f, -0.5f, //

        -0.5f, -0.5f, 0.5f, //
        0.5f,  -0.5f, 0.5f, //
        0.5f,  0.5f,  0.5f, //
        0.5f,  0.5f,  0.5f, //
        -0.5f, 0.5f,  0.5f, //
        -0.5f, -0.5f, 0.5f, //

        -0.5f, 0.5f,  0.5f,  //
        -0.5f, 0.5f,  -0.5f, //
        -0.5f, -0.5f, -0.5f, //
        -0.5f, -0.5f, -0.5f, //
        -0.5f, -0.5f, 0.5f,  //
        -0.5f, 0.5f,  0.5f,  //

        0.5f,  0.5f,  0.5f,  //
        0.5f,  0.5f,  -0.5f, //
        0.5f,  -0.5f, -0.5f, //
        0.5f,  -0.5f, -0.5f, //
        0.5f,  -0.5f, 0.5f,  //
        0.5f,  0.5f,  0.5f,  //

        -0.5f, -0.5f, -0.5f, //
        0.5f,  -0.5f, -0.5f, //
        0.5f,  -0.5f, 0.5f,  //
        0.5f,  -0.5f, 0.5f,  //
        -0.5f, -0.5f, 0.5f,  //
        -0.5f, -0.5f, -0.5f, //

        -0.5f, 0.5f,  -0.5f, //
        0.5f,  0.5f,  -0.5f, //
        0.5f,  0.5f,  0.5f,  //
        0.5f,  0.5f,  0.5f,  //
        -0.5f, 0.5f,  0.5f,  //
        -0.5f, 0.5f,  -0.5f  //
    };

    // Buffers
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint light_vao;
    glGenVertexArrays(1, &light_vao);
    glBindVertexArray(vao);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Load, compile and link shaders
    // I really need to do something with those hardcoded paths
    std::vector<std::filesystem::path> shader_paths(
        {"C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert", "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag",
         "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/lightvertex.vert", "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/lightfrag.frag"});

    auto load_shader_file = [](Shader& shader, std::filesystem::path const& path) -> void {
        std::string shader_source;
        read_file(path, shader_source);
        Shader_file s(shader_type_from_filename(path), shader_source);
        shader.attach(s);
    };

    Shader shader;
    load_shader_file(shader, shader_paths[0]);
    load_shader_file(shader, shader_paths[1]);
    shader.link();

    Shader light_shader;
    load_shader_file(light_shader, shader_paths[2]);
    load_shader_file(light_shader, shader_paths[3]);
    light_shader.link();

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

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    std::vector<Matrix4> model_transforms;
    for (int i = 0; i < 20; ++i) {
        model_transforms.push_back(transform::translate({dist(rng) * 5.0f, dist(rng) * 1.25f, dist(rng) * 5.0f}));
    }

    Matrix4 view = Matrix4::identity;

    Color light_color(1.0f, 1.0f, 1.0f);
    Color object_color(1.0f, 0.5f, 0.31f);

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = transform::look_at(camera.position, camera.position + camera.front, Vector3::up);

        shader.use();
        shader.set_vec3("light_color", light_color);
        shader.set_vec3("object_color", object_color);
        for (int i = 0; i < 20; ++i) {
            Matrix4 transform_matrix = model_transforms[i] * view * projection;
            shader.set_matrix4("transform", transform_matrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }

        light_shader.use();
        Matrix4 transform_matrix = transform::scale({0.2f, 0.2f, 0.2f}) * transform::translate({-5.0f, 2.0f, -3.0f}) * view * projection;
        light_shader.set_vec3("light_color", light_color);
        light_shader.set_matrix4("transform", transform_matrix);

        glBindVertexArray(light_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
