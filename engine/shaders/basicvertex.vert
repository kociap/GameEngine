#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 tex_coordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragment_position;
out vec2 texture_coordinates;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    normal = normalize(in_normal);
    fragment_position = vec3(model * vec4(pos, 1.0));
    texture_coordinates = tex_coordinates;
}
