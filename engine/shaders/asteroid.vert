#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 tex_coordinates;
layout(location = 3) in mat4 instance_model;

uniform mat4 projection;
uniform mat4 view;

out Frag_data {
    vec2 texture_coordinates;
}
fragment_data;

void main() {
    gl_Position = projection * view * instance_model * vec4(pos, 1.0);
    fragment_data.texture_coordinates = tex_coordinates;
}
