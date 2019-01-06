#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "camera.hpp"
#include "color.hpp"
#include "debug_macros.hpp"
#include "math/math.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "mesh/cube.hpp"
#include "mesh/plane.hpp"
#include "model.hpp"
#include "renderer/framebuffer.hpp"
#include "shader.hpp"
#include "spotlight.hpp"
#include "stb/stb_image.hpp"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <vector>

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_button_callback(GLFWwindow*, int button, int action, int mods);
void mouse_position_callback(GLFWwindow*, double param_x, double param_y);
void scroll_callback(GLFWwindow*, double offset_x, double offset_y);
void process_input(GLFWwindow* window);

enum class Input_event { scroll };

struct Input_axis_binding {
    std::string axis_name;
    Input_event input_event;
    float axis_value = 0;

    Input_axis_binding(std::string const& name, Input_event evt) : axis_name(name), input_event(evt) {}
};

struct Input {
public:
    float get_axis(std::string const& axis_name) {
        auto map_iter = axis_bindings.find(axis_name);
        if (map_iter == axis_bindings.end()) {
            throw std::runtime_error("Axis " + axis_name + " not bound");
        }
        return map_iter->second.axis_value;
    }

    void add_axis_binding(std::string const& axis_name, Input_event evt) {
        axis_bindings.emplace(axis_name, Input_axis_binding(axis_name, evt));
    }

    void update_axis_value(Input_event evt, float value) {
        for (auto& [key, binding] : axis_bindings) {
            if (binding.input_event == evt) {
                binding.axis_value = value;
            }
        }
    }

private:
    std::map<std::string, Input_axis_binding> axis_bindings;
};

GLuint window_width = 800;
GLuint window_height = 800;

uint32_t shadow_width = 1024;
uint32_t shadow_height = 1024;

Camera camera;
Matrix4 projection;

Input input;

int main(int argc, char** argv) {
    auto path = argv[0];

    input = Input();
    camera = Camera();
    projection = transform::perspective(math::radians(camera.fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // I really need to do something with those hardcoded paths
    Shader shader;
    shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert");
    shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag");
    shader.link();

    Shader light_shader;
    light_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/light.vert");
    light_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/light.frag");
    light_shader.link();

    Shader normals_shader;
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.vert");
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.geom");
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.frag");
    normals_shader.link();

    Shader gamma_correction_shader;
    gamma_correction_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/postprocessing/postprocess_vertex.vert");
    gamma_correction_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/postprocessing/gamma_correction.frag");
    gamma_correction_shader.link();

    Shader quad_shader;
    quad_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.vert");
    quad_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.frag");
    quad_shader.link();

    Shader skybox_shader;
    skybox_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/skybox.vert");
    skybox_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/skybox.frag");
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

    auto load_texture = [](std::filesystem::path filename) -> GLuint {
        int width, height, channels;
        int32_t desired_channel_count = 4;
        //stbi_set_flip_vertically_on_load(true);
        unsigned char* image_data = stbi_load(filename.string().c_str(), &width, &height, &channels, 0);
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
    };

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::mt19937 rng1;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(0, 1.0f);
    std::uniform_real_distribution<float> ring_variation(0.9f, 1.1f);
    std::uniform_real_distribution<float> variance(0.9f, 1.3f);
    std::uniform_real_distribution<float> scale(0.6f, 1.2f);

    uint32_t asteroid_count = 1000;
    float ring_radius = 9;
    std::vector<Matrix4> model_transforms;
    model_transforms.reserve(asteroid_count);
    for (int i = 0; i < asteroid_count; ++i) {
        float random_number = math::radians(dist(rng) * 360.0f);
        float ring = ring_radius * ring_variation(rng) * variance(rng1);
        model_transforms.push_back(
            (transform::scale({0.1f, 0.1f, 0.1f}) *= scale(rng)) * transform::rotate_z(math::radians(dist(rng) * 360.0f)) *
            transform::rotate_x(math::radians(dist(rng) * 360.0f)) *
            transform::translate({ring * std::cosf(random_number), (dist(rng) - 0.5f) * variance(rng1) + 0.8f, ring * std::sinf(random_number)}));
    }

    Matrix4 view = Matrix4::identity;

    Model cube = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/cube.obj");
    Plane scene_quad;
    Cube skybox;
    Plane floor;

    GLuint skybox_cubemap =
        load_cubemap({"C:/Users/An0num0us/Documents/GameEngine/assets/skybox/right.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/left.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/top.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/bottom.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/back.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/front.jpg"});

    GLuint wood_texture = load_texture("C:/Users/An0num0us/Documents/GameEngine/assets/wood_floor.png");

    renderer::framebuffer::Framebuffer_construct_info framebuffer_construct_info;
    framebuffer_construct_info.width = window_width;
    framebuffer_construct_info.height = window_height;
    framebuffer_construct_info.depth_buffer = true;
    framebuffer_construct_info.multisampled = true;
    framebuffer_construct_info.samples = 4;

    renderer::framebuffer::Framebuffer framebuffer_multisampled(framebuffer_construct_info);

    framebuffer_construct_info.multisampled = false;
    //framebuffer_construct_info.depth_buffer_type = renderer::framebuffer::Buffer_type::texture;
    renderer::framebuffer::Framebuffer framebuffer(framebuffer_construct_info);

    //glEnable(GL_CULL_FACE);

    Vector3 light_pos(-1.0f, 1.0f, 0.0f);

    shader.use();
    shader.set_float("ambient_strength", 0.02f);
    shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
    shader.set_float("light.attentuation_constant", 1.0f);
    shader.set_float("light.attentuation_linear", 0.09f);
    shader.set_float("light.attentuation_quadratic", 0.032f);
    shader.set_float("light.intensity", 1.0f);
    shader.set_float("light.diffuse_strength", 0.8f);
    shader.set_float("light.specular_strength", 1.0f);
    shader.set_float("material.shininess", 32.0f);
    shader.set_vec3("light.position", light_pos);
    shader.set_vec3("light.color", Color(1.0f, 1.0f, 1.0f));

    light_shader.use();
    light_shader.set_matrix4("light_transform", transform::scale({0.1f, 0.1f, 0.1f}) * transform::translate(light_pos));

    input.add_axis_binding("scroll", Input_event::scroll);

    float gamma_correction_value = 2.2;
    glDisable(GL_FRAMEBUFFER_SRGB);
    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        float scroll_value = input.get_axis("scroll");
        camera.position += camera.front * scroll_value * 2;

        framebuffer_multisampled.bind();
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        view = transform::look_at(camera.position, camera.position + camera.front, Vector3::up);

        shader.use();
        shader.set_matrix4("model", transform::scale({10, 10, 10}) * transform::rotate_x(math::radians(90.0f)));
        shader.set_matrix4("view", view);
        shader.set_matrix4("projection", projection);
        shader.set_vec3("camera.position", camera.position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wood_texture);
        shader.set_int("material.texture_diffuse0", 0);
        floor.draw(shader);

        light_shader.use();
        light_shader.set_matrix4("view", view);
        light_shader.set_matrix4("projection", projection);
        cube.draw(light_shader);

        //normals_shader.use();
        //normals_shader.set_matrix4("model", Matrix4::identity);
        //normals_shader.set_matrix4("view", view);
        //normals_shader.set_matrix4("projection", projection);
        //normals_shader.set_float("normal_magnitude", 0.2f);
        //cube1.draw(normals_shader);

        //glDisable(GL_CULL_FACE);
        //glDepthFunc(GL_LEQUAL);
        //skybox_shader.use();
        //skybox_shader.set_matrix4("view", view * transform::translate(-transform::get_translation(view)));
        //skybox_shader.set_matrix4("projection", projection);
        //skybox_shader.set_int("skybox", 0);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);
        //skybox.draw(skybox_shader);
        //glDepthFunc(GL_LESS);
        //glEnable(GL_CULL_FACE);

        framebuffer_multisampled.blit(framebuffer);
        glDisable(GL_DEPTH_TEST);
        gamma_correction_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.get_texture());
        gamma_correction_shader.set_int("scene_texture", 0);
        gamma_correction_shader.set_float("gamma", 1 / gamma_correction_value);
        scene_quad.draw(gamma_correction_shader);

        glfwSwapBuffers(window);
        input.update_axis_value(Input_event::scroll, static_cast<float>(0));
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
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
    input.update_axis_value(Input_event::scroll, static_cast<float>(offset_y));
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
