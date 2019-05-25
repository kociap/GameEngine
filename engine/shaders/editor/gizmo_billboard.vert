#version 450 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in mat4 mv_matrix;
layout(location = 5) in vec4 in_color;

uniform mat4 projection;

out vec4 color;

void main() {
    color = in_color;
    gl_Position = projection * mv_matrix * vec4(in_position, 1.0);
}