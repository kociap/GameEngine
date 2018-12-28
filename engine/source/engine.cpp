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
#include "spotlight.hpp"
#include "model.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <vector>

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

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    std::vector<Matrix4> model_transforms;
    for (int i = 0; i < 20; ++i) {
        model_transforms.push_back(transform::scale({3, 3, 3}) * transform::translate({dist(rng) * 10.0f, dist(rng) * 2.0f, dist(rng) * 10.0f}));
    }

    Matrix4 view = Matrix4::identity;

    Spot_light light;
    light.position = Vector3(-2.0f, 1.0f, -3.0f);
    light.direction = Vector3(0.3f, 1.0f, 0.3f);
    light.color = Color(1.0f, 1.0f, 1.0f);
    light.cutoff_angle = math::radians(22.5f);
    light.blend_angle = math::radians(35.0f);
    light.direction.normalize();
    Color object_color(1.0f, 0.5f, 0.31f);

    Vector3 point_light_positions[] = {Vector3(0.7f, 0.2f, 2.0f), Vector3(2.3f, -3.3f, -4.0f), Vector3(-4.0f, 2.0f, -12.0f), Vector3(0.0f, 0.0f, -3.0f)};

    shader.use();
    shader.set_float("material.shininess", 32.0f);
    shader.set_float("material.ambient_strength", 0.2f);
    shader.set_float("material.diffuse_strength", 1.0f);
    shader.set_float("material.specular_strength", 1.0f);
    shader.set_float("light.attentuation_constant", 1.0f);
    shader.set_float("light.attentuation_linear", 0.09f);
    shader.set_float("light.attentuation_quadratic", 0.032f);

    Color point_light_color = Color(0.75f, 0.5f, 0.25f);

    shader.set_vec3("point_lights[0].position", point_light_positions[0]);
    shader.set_vec3("point_lights[0].color", point_light_color);
    shader.set_float("point_lights[0].diffuse_strength", 0.8f);
    shader.set_float("point_lights[0].specular_strength", 1.0f);
    shader.set_float("point_lights[0].attentuation_constant", 1.0f);
    shader.set_float("point_lights[0].attentuation_linear", 0.009f);
    shader.set_float("point_lights[0].attentuation_quadratic", 0.0032f);

	Model model = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/nanosuit/nanosuit.obj");

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        model.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
