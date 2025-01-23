#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 4) in vec2 tex_coordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out Frag_Data {
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinates;
}
fragment_data;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    fragment_data.fragment_position = vec3(model * vec4(pos, 1.0));
    fragment_data.normal = normalize(vec3(transpose(inverse(model)) * vec4(in_normal, 0.0)));
    fragment_data.texture_coordinates = tex_coordinates;
}
