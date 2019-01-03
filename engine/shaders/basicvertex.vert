#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 tex_coordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out Frag_data {
    vec3 normal;
    vec3 fragment_position;
    vec2 texture_coordinates;
}
fragment_data;

void main() {
    mat4 model_inverse_transpose = transpose(inverse(model));
    gl_Position = projection * view * model * vec4(pos, 1.0);
    fragment_data.normal = vec3(normalize(model_inverse_transpose * vec4(in_normal, 0.0)));
    fragment_data.fragment_position = vec3(model * vec4(pos, 1.0));
    fragment_data.texture_coordinates = tex_coordinates;
}
