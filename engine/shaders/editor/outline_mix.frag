#version 450 core

in vec2 tex_coords;
in vec2 screen_size_rcp;

uniform sampler2D scene_texture;
uniform sampler2D outline_texture;

out vec4 frag_color;

// #define INSET_OUTLINE

vec4 texture_offset(sampler2D tex, vec2 coords, vec2 offset) {
    return texture(tex, coords + offset * screen_size_rcp);
}

void main() {
    vec4 current_frag_color = texture(outline_texture, tex_coords);
    vec4 around_color_average = texture_offset(outline_texture, tex_coords, vec2(1,   0)) + 
                                texture_offset(outline_texture, tex_coords, vec2(1,   1)) +
                                texture_offset(outline_texture, tex_coords, vec2(0,   1)) +
                                texture_offset(outline_texture, tex_coords, vec2(-1,  1)) +
                                texture_offset(outline_texture, tex_coords, vec2(-1,  0)) +
                                texture_offset(outline_texture, tex_coords, vec2(-1, -1)) +
                                texture_offset(outline_texture, tex_coords, vec2(0,  -1)) +
                                texture_offset(outline_texture, tex_coords, vec2(1,  -1));
    around_color_average /= 8.0;
#ifdef INSET_OUTLINE
    bool edge_detected = current_frag_color.a > 0 && around_color_average.a < 1;
#else
    bool edge_detected = current_frag_color.a == 0 && around_color_average.a != 0; 
#endif
    if(edge_detected) {
        around_color_average /= around_color_average.a;
        frag_color = vec4(around_color_average.rgb, 1.0);
    } else {
        frag_color = texture(scene_texture, tex_coords);
    }
}