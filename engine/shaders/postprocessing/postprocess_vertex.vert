#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coordinates;

out vec2 texture_coordinates;

void main() {
    gl_Position = vec4(position, 1.0);
    texture_coordinates = tex_coordinates;
}