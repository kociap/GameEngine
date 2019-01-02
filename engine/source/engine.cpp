#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "camera.hpp"
#include "color.hpp"
#include "debug_macros.hpp"
#include "math/math.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "model.hpp"
#include "renderer/framebuffer.hpp"
#include "shader.hpp"
#include "spotlight.hpp"
#include "stb/stb_image.hpp"
#include "mesh/plane.hpp"
#include "mesh/cube.hpp"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <vector>

GLuint window_width = 800;
GLuint window_height = 800;

Camera camera;
Matrix4 projection;

void read_file(std::filesystem::path const& filename, std::string& out);
Shader_type shader_type_from_filename(std::filesystem::path const& filename);
void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
void mouse_position_callback(GLFWwindow*, double param_x, double param_y);
void scroll_callback(GLFWwindow*, double offset_x, double offset_y);
void process_input(GLFWwindow* window);

int main(int argc, char** argv) {
    auto path = argv[0];

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
        throw std::runtime_error("GLFW failed to create a window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwTerminate();
        throw std::runtime_error("GLAD not initialized");
    }

    glViewport(0, 0, window_width, window_height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Load, compile and link shaders
    auto load_shader_file = [](Shader& shader, std::filesystem::path const& path) -> void {
        std::string shader_source;
        read_file(path, shader_source);
        Shader_file s(shader_type_from_filename(path), shader_source);
        shader.attach(s);
    };

    // I really need to do something with those hardcoded paths
    Shader shader;
    load_shader_file(shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert");
    load_shader_file(shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag");
    shader.link();

    Shader quad_shader;
    load_shader_file(quad_shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.vert");
    load_shader_file(quad_shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.frag");
    quad_shader.link();

	Shader skybox_shader;
    load_shader_file(skybox_shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/skybox.vert");
    load_shader_file(skybox_shader, "C:/Users/An0num0us/Documents/GameEngine/engine/shaders/skybox.frag");
	skybox_shader.link();

    auto load_cubemap = [](std::vector<std::filesystem::path> const& paths) -> GLuint {
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
            uint8_t* data = stbi_load(paths[i].string().c_str(), &width, &height, &channels, 0);
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

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    std::vector<Matrix4> model_transforms;
    for (int i = 0; i < 20; ++i) {
        model_transforms.push_back(transform::translate({dist(rng) * 10.0f, dist(rng) * 2.0f, dist(rng) * 10.0f}));
    }

    Matrix4 view = Matrix4::identity;

    Spot_light light;
    light.position = Vector3(-2.0f, 1.0f, -3.0f);
    light.direction = Vector3(0.3f, 1.0f, 0.3f);
    light.color = Color(1.0f, 1.0f, 1.0f);
    light.cutoff_angle = math::radians(22.5f);
    light.blend_angle = math::radians(35.0f);
    light.intensity = 1.5f;
    light.direction.normalize();

    shader.use();
    shader.set_float("material.shininess", 32.0f);
    shader.set_float("material.ambient_strength", 0.2f);
    shader.set_float("material.diffuse_strength", 1.0f);
    shader.set_float("material.specular_strength", 1.0f);
    shader.set_float("light.attentuation_constant", 1.0f);
    shader.set_float("light.attentuation_linear", 0.09f);
    shader.set_float("light.attentuation_quadratic", 0.032f);
    shader.set_float("light.intensity", light.intensity);

	auto load_texture = [](std::filesystem::path filename) -> GLuint {
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

    Model cube1 = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/cube.obj");
    Model cube2 = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/cube.obj");
    Plane scene_quad;
    Cube skybox;
	Cube cube;
    GLuint wall_texture = load_texture("C:/Users/An0num0us/Documents/GameEngine/assets/wall.jpg");

    GLuint skybox_cubemap =
        load_cubemap({"C:/Users/An0num0us/Documents/GameEngine/assets/skybox/right.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/left.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/top.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/bottom.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/back.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/front.jpg"});

    Framebuffer framebuffer(window_width, window_height);
    glEnable(GL_CULL_FACE);

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        framebuffer.bind();
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        view = transform::look_at(camera.position, camera.position + camera.front, Vector3::up);

        shader.use();
        shader.set_vec3("light.position", camera.position);
        shader.set_vec3("light.direction", normalize(camera.front));
        shader.set_vec3("light.color", light.color);
        shader.set_float("light.cutoff_angle", std::cos(light.cutoff_angle));
        shader.set_float("light.blend_angle", std::cos(light.blend_angle));
        shader.set_vec3("view_position", camera.position);
        shader.set_vec3("camera.position", camera.position);
        shader.set_vec3("camera.direction", normalize(camera.front));

        shader.set_matrix4("model", Matrix4::identity);
        shader.set_matrix4("view", view);
        shader.set_matrix4("projection", projection);
        cube1.draw(shader);
        shader.set_matrix4("model", transform::translate({4.0f, 0.0f, -2.0f}));
        //cube2.draw(shader);
        cube2.draw(shader);

		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
        skybox_shader.use();
        skybox_shader.set_matrix4("view", view * transform::translate(-transform::get_translation(view)));
        skybox_shader.set_matrix4("projection", projection);
        skybox_shader.set_int("skybox", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);
        skybox.draw(skybox_shader);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        framebuffer.unbind();
        glDisable(GL_DEPTH_TEST);
        quad_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.get_texture());
        quad_shader.set_int("scene_texture", 0);
        scene_quad.draw(quad_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

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
    camera.fov -= static_cast<float>(offset_y * 2.0);
    camera.fov = std::min(camera.fov, 120.0f);
    camera.fov = std::max(camera.fov, 10.0f);
    projection = transform::perspective(math::radians(camera.fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    float camera_speed = 0.15f;
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
