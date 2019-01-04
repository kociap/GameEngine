#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 tex_coordinates;

uniform mat4 light_transform;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * light_transform * vec4(pos, 1.0);
}
