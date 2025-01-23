#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinates;

out Vert_data {
    vec3 normal;
}
vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.normal = normalize(vec3(projection * transpose(inverse(view * model)) * vec4(normal, 0.0)));
}