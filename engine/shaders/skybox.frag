#version 450 core

in vec3 texture_coordinates;

uniform samplerCube skybox;

out vec4 frag_color;

void main() {
    frag_color = texture(skybox, texture_coordinates);
}