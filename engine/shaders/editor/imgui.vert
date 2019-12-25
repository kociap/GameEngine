#version 450 core

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;

uniform mat4 proj_mat;

out vec4 color;

void main() {
    color = in_color;
    gl_Position = proj_mat * vec4(in_pos, 0, 1);
}
