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

GLuint window_width = 800;
GLuint window_height = 800;

Camera camera;
Matrix4 projection;

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

    // I really need to do something with those hardcoded paths
    Shader shader;
    shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicvertex.vert");
    shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/basicfrag.frag");
    shader.link();

	Shader instanced_shader;
    instanced_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/asteroid.vert");
    instanced_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/asteroid.frag");
	instanced_shader.link();

    Shader normals_shader;
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.vert");
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.geom");
    normals_shader.load_shader_file("C:/Users/An0num0us/Documents/GameEngine/engine/shaders/normals.frag");
    normals_shader.link();

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

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::mt19937 rng1;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(0, 1.0f);
    std::uniform_real_distribution<float> ring_variation(0.9f, 1.1f);
    std::uniform_real_distribution<float> variance(0.9f, 1.3f);

    uint32_t asteroid_count = 2000;
    float ring_radius = 9;
    std::vector<Matrix4> model_transforms;
    for (int i = 0; i < asteroid_count; ++i) {
        float random_number = math::radians(dist(rng) * 360.0f);
        float ring = ring_radius * ring_variation(rng) * variance(rng1);
        model_transforms.push_back(transform::rotate_z(math::radians(dist(rng) * 360.0f)) * transform::rotate_x(math::radians(dist(rng) * 360.0f)) *
                                   transform::translate({ring * std::cosf(random_number), (dist(rng) - 0.5f) * variance(rng1) + 0.8f, ring * std::sinf(random_number)}));
    }

    Matrix4 view = Matrix4::identity;

    Model cube = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/cube.obj");
    Plane scene_quad;
    Cube skybox;

    Model planet = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/planet/planet.obj");
    Model asteroid = Model::load_from_file("C:/Users/An0num0us/Documents/GameEngine/assets/rock/rock.obj");

    GLuint skybox_cubemap =
        load_cubemap({"C:/Users/An0num0us/Documents/GameEngine/assets/skybox/right.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/left.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/top.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/bottom.jpg",
                      "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/back.jpg", "C:/Users/An0num0us/Documents/GameEngine/assets/skybox/front.jpg"});

    Framebuffer framebuffer(window_width, window_height);
    glEnable(GL_CULL_FACE);

	GLuint instance_buffer;
    glGenBuffers(1, &instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, asteroid_count * sizeof(Matrix4), &model_transforms[0], GL_STATIC_DRAW);

	asteroid.for_each_mesh([](Mesh& mesh) -> void { 
		mesh.bind();
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(12 * sizeof(float)));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    });

    // Window and render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        framebuffer.bind();
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        view = transform::look_at(camera.position, camera.position + camera.front, Vector3::up);

        shader.use();
        shader.set_matrix4("model", Matrix4::identity);
        shader.set_matrix4("view", view);
        shader.set_matrix4("projection", projection);
        planet.draw(shader);

		instanced_shader.use();
        instanced_shader.set_matrix4("view", view);
        instanced_shader.set_matrix4("proejction", projection);
        asteroid.for_each_mesh([&instanced_shader, asteroid_count](Mesh& mesh) -> void {
            mesh.draw_instanced(instanced_shader, asteroid_count);
		});

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
