#version 450 core

layout(location = 0) in vec3 in_position;

uniform mat4 model_mat;
uniform mat4 vp_mat;

out vec3 world_pos;

void main() {
    vec4 wp = model_mat * vec4(in_position, 1.0);
    world_pos = vec3(wp);
    gl_Position = vp_mat * wp;
}
