#version 450 core

layout(location = 0) in vec3 pos;

uniform mat4 vp_mat;

out vec3 texture_coordinates;

void main() {
    gl_Position = (vp_mat * vec4(pos, 1.0)).xyww;
    texture_coordinates = pos;
}
