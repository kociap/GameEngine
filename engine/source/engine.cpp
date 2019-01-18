#include "glad/glad.h" // GLAD must be before GLFW
                       // Or define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "color.hpp"
#include "components/camera.hpp"
#include "debug_macros.hpp"
#include "game_object.hpp"
#include "math/math.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "mesh/cube.hpp"
#include "mesh/plane.hpp"
#include "model.hpp"
#include "object_manager.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/renderer.hpp"
#include "shader.hpp"

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

uint32_t window_width = 800;
uint32_t window_height = 800;

uint32_t shadow_width = 1024;
uint32_t shadow_height = 1024;

Matrix4 projection;

Input input;
//renderer::Renderer g_renderer;
//Object_Manager<Mesh> g_mesh_manager;

int main(int argc, char** argv) {
    auto path = argv[0];
    std::filesystem::path current_path = std::filesystem::current_path();
	
	Game_Object camera_object;
    camera_object.add_component<Camera>();
    input = Input();
    //g_renderer = renderer::Renderer();
    //g_mesh_manager = Object_Manager<Mesh>();

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

    Shader gamma_correction_shader;
    gamma_correction_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/postprocessing/postprocess_vertex.vert");
    gamma_correction_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/postprocessing/gamma_correction.frag");
    gamma_correction_shader.link();

    Shader quad_shader;
    quad_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.vert");
    quad_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/quad.frag");
    quad_shader.link();

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
    std::uniform_real_distribution<float> dist(0, 1.0f);

    std::vector<Matrix4> model_transforms;
    uint32_t cubes_count = 40;
    for (int i = 0; i < cubes_count; ++i) {
        model_transforms.push_back(transform::translate({dist(rng) * 1, dist(rng) * 5, -dist(rng) * 20}));
    }

    Matrix4 view = Matrix4::identity;

    Model cube = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/cube.obj");
    Plane scene_quad;
    Cube box;
    Plane floor;

    GLuint wood_texture = load_texture("C:/Users/An0num0us/Documents/GameEngine/assets/wood_floor.png");
    GLuint container_texture = load_texture("C:/Users/An0num0us/Documents/GameEngine/assets/container.jpg");

    renderer::framebuffer::Framebuffer_Construct_Info framebuffer_construct_info;
    framebuffer_construct_info.width = window_width;
    framebuffer_construct_info.height = window_height;
    framebuffer_construct_info.depth_buffer = true;
    framebuffer_construct_info.multisampled = true;
    framebuffer_construct_info.samples = 4;

    renderer::framebuffer::Framebuffer framebuffer_multisampled(framebuffer_construct_info);

    framebuffer_construct_info.multisampled = false;
    renderer::framebuffer::Framebuffer framebuffer(framebuffer_construct_info);

    framebuffer_construct_info.depth_buffer_type = renderer::framebuffer::Buffer_Type::texture;
    framebuffer_construct_info.color_buffer = false;
    renderer::framebuffer::Framebuffer light_depth_buffer(framebuffer_construct_info);

    Vector3 light_pos(-1.0f, 4.0f, 0.0f);

    shader.use();
    shader.set_float("ambient_strength", 0.02f);
    shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
    shader.set_float("light.attentuation_constant", 1.0f);
    shader.set_float("light.attentuation_linear", 0.09f);
    shader.set_float("light.attentuation_quadratic", 0.032f);
    shader.set_float("light.intensity", 6.0f);
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
    glEnable(GL_CULL_FACE);

    // A hack
    auto render_scene = [&shader, &view, &wood_texture, &floor, &light_shader, &cube, &cubes_count, &model_transforms, &container_texture, &box]() -> void {
        shader.use();
        shader.set_matrix4("view", view);
        shader.set_matrix4("projection", projection);
        shader.set_vec3("camera.position", Camera::main->transform.position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wood_texture);
        shader.set_int("material.texture_diffuse0", 0);
        for (int i = -4; i < 20; ++i) {
            for (int j = -4; j < 20; ++j) {
                shader.set_matrix4("model",
                                   transform::rotate_x(math::radians(90.0f)) * transform::translate({static_cast<float>(i * 2), 0, static_cast<float>(j * 2)}));
                floor.draw(shader);
            }
        }

        light_shader.use();
        light_shader.set_matrix4("view", view);
        light_shader.set_matrix4("projection", projection);
        cube.draw(light_shader);

        for (int i = 0; i < cubes_count; ++i) {
            shader.use();
            shader.set_matrix4("model", model_transforms[i]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, container_texture);
            box.draw(shader);
        }
    };

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        float scroll_value = input.get_axis("scroll");
        Camera::main->transform.position += Camera::main->get_front() * scroll_value * 2;
        glEnable(GL_DEPTH_TEST);

        //view = Camera::main->get_view_transform();
		view = Camera::main->get_view_transform();
        projection = Camera::main->get_projection_transform();

        glViewport(0, 0, shadow_width, shadow_height);
        light_depth_buffer.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        render_scene();

        glViewport(0, 0, window_width, window_height);
        framebuffer_multisampled.bind();
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_scene();

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
    Camera::main->transform.rotate(Vector3::up, math::radians(-offset_x));
}

void scroll_callback(GLFWwindow*, double offset_x, double offset_y) {
    input.update_axis_value(Input_event::scroll, static_cast<float>(offset_y));
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	Vector3 camera_front = Camera::main->get_front();
    Vector3& camera_position = Camera::main->transform.position;
    float camera_speed = 0.15f;
    if (glfwGetKey(window, GLFW_KEY_W)) {
        camera_position += camera_front * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        camera_position -= camera_front * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera_position += Vector3::cross(Vector3::up, camera_front).normalize() * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        camera_position -= Vector3::cross(Vector3::up, camera_front).normalize() * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E)) {
        camera_position += Vector3::up * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        camera_position -= Vector3::up * camera_speed;
    }
}
