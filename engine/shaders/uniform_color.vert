#version 450 core

layout(location = 0) in vec3 pos;

uniform mat4 mvp_mat;

void main() {
    gl_Position = mvp_mat * vec4(pos, 1.0);
}
