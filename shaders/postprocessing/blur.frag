#version 450 core

in vec2 texture_coordinates;

uniform sampler2D scene_texture;

out vec4 frag_color;

void main() {
    const float offset = 1.0 / 300.0;

    // clang-format off
    float kernel[9] = float[](
        2.0 / 28, 4.0 / 28, 2.0 / 28, 
        4.0 / 28, 4.0 / 28, 4.0 / 28, 
        2.0 / 28, 4.0 / 28, 2.0 / 28
    );

    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), vec2(0.0, offset), vec2(offset, offset),
        vec2(-offset, 0.0), vec2(0.0, 0.0), vec2(offset, 0.0),
        vec2(-offset, -offset), vec2(0.0, -offset), vec2(offset, -offset)
    );
    // clang-format on

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        color += vec3(texture(scene_texture, texture_coordinates + offsets[i])) * kernel[i];
    }

    frag_color = vec4(color, 1.0);
}