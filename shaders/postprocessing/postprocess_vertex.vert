#version 450 core

layout(location = 0) in vec3 position;
layout(location = 4) in vec2 tex_coordinates;

out vec2 tex_coords;
out vec2 screen_size_rcp;

uniform sampler2D scene_texture;

void main() {
    vec2 tex_size = textureSize(scene_texture, 0).xy;
    screen_size_rcp = 1 / tex_size;
    tex_coords = tex_coordinates;
    gl_Position = vec4(position, 1.0);
}