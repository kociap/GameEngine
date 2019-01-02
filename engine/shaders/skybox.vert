#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 tex_coordinates;

uniform mat4 view;
uniform mat4 projection;

out vec3 texture_coordinates;

void main() {
    gl_Position = (projection * view * vec4(pos, 1.0)).xyww;
    texture_coordinates = pos;
}
