#version 450 core

in vec2 tex_coords;
in vec2 screen_size_rcp;
out vec4 frag_color;

uniform sampler2D scene_texture;

#define FXAA_EDGE_THRESHOLD 0.125
#define FXAA_EDGE_THRESHOLD_MIN (1.0 / 24.0)
#define FXAA_SUBPIX_TRIM 0.25
#define FXAA_SUBPIX_CAP 0.75
#define FXAA_SEARCH_STEPS 16
#define FXAA_SEARCH_THRESHOLD 0.25

#define FXAA_SUBPIX_TRIM_SCALE (1.0 / (1.0 - FXAA_SUBPIX_TRIM))

#define FXAA_EARLY_EXIT_OUTPUT_LUMA 1
#define FXAA_DEBUG_EDGE_DETECTION 0
#define FXAA_DEBUG_EDGE_ORIENTATION 0
#define FXAA_DEBUG_GRADIENT 0
#define FXAA_DEBUG_NEGPOS 0

vec3 fxaa_shader(sampler2D tex, vec2 pos, vec2 frame_rcp);

void main() {
    frag_color.rgb = fxaa_shader(scene_texture, tex_coords, screen_size_rcp);
}

vec3 fxaa_lerp(vec3 a, vec3 b, float factor) {
    return a + (b - a) * factor;
}

vec3 fxaa_texture_offset(sampler2D tex, vec2 pos, vec2 offset) {
    return texture(tex, pos + offset).rgb;
}

vec3 fxaa_texture_offset_pixels(sampler2D tex, vec2 pos, ivec2 offset) {
    return textureOffset(tex, pos, offset).rgb;
}

float fxaa_luma(vec3 color) {
    return color.g * (0.587 / 0.299) + color.r;
}

vec3 fxaa_shader(sampler2D tex, vec2 tex_pos, vec2 frame_rcp) {
    vec3 rgb_c = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(0, 0));
    vec3 rgb_n = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(0, 1));
    vec3 rgb_w = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(1, 0));
    vec3 rgb_s = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(0, -1));
    vec3 rgb_e = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(-1, 0));
    float luma_c = fxaa_luma(rgb_c);
    float luma_n = fxaa_luma(rgb_n);
    float luma_w = fxaa_luma(rgb_w);
    float luma_s = fxaa_luma(rgb_s);
    float luma_e = fxaa_luma(rgb_e);
    float luma_min = min(luma_c, min(min(luma_n, luma_w), min(luma_e, luma_s)));
    float luma_max = max(luma_c, max(max(luma_n, luma_w), max(luma_e, luma_s)));
    float luma_range = luma_max - luma_min;

    #if FXAA_DEGUB_EDGE_DETECTION
        return vec3(luma_range);
    #endif // FXAA_SHOW_EDGE_DETECTION

    // Early exit
    if(luma_range < max(FXAA_EDGE_THRESHOLD_MIN, luma_max * FXAA_EDGE_THRESHOLD)) {
        #if FXAA_EARLY_EXIT_OUTPUT_LUMA
            return vec3(luma_c / (1.0 + (0.587/0.299)));
        #else
            return rgb_c;
        #endif // FXAA_EARLY_EXIT_OUTPUT_LUMA
    }

    float luma_lowpass = (luma_n + luma_s + luma_w + luma_e) * 0.25;
    float range_lowpass = abs(luma_lowpass - luma_c);
    float blend_lowpass = max(0.0, (range_lowpass / luma_range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
    blend_lowpass = min(FXAA_SUBPIX_CAP, blend_lowpass);

    vec3 rgb_nw = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(1, 1));
    vec3 rgb_sw = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(1, -1));
    vec3 rgb_se = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(-1, -1));
    vec3 rgb_ne = fxaa_texture_offset_pixels(tex, tex_pos, ivec2(-1, 1));
    float luma_nw = fxaa_luma(rgb_nw);
    float luma_sw = fxaa_luma(rgb_sw);
    float luma_se = fxaa_luma(rgb_se);
    float luma_ne = fxaa_luma(rgb_ne);

    float edge_vertical = 
        abs((0.25 * luma_nw) + (-0.5 * luma_n) + (0.25 * luma_ne)) + 
        abs((0.50 * luma_w ) + (-1.0 * luma_c) + (0.50 * luma_e )) + 
        abs((0.25 * luma_sw) + (-0.5 * luma_s) + (0.25 * luma_se));
    float edge_horizontal = 
        abs((0.25 * luma_nw) + (-0.5 * luma_w) + (0.25 * luma_sw)) + 
        abs((0.50 * luma_n ) + (-1.0 * luma_c) + (0.50 * luma_s )) + 
        abs((0.25 * luma_ne) + (-0.5 * luma_e) + (0.25 * luma_se));
    bool horizontal_span = edge_horizontal >= edge_vertical;

    #if FXAA_DEBUG_EDGE_ORIENTATION
        return vec3(horizontal_span * 1.0, (1.0 - horizontal_span) * 1.0, 0);
    #endif // FXAA_DEBUG_EDGE_ORIENTATION

    float luma1 = luma_n;
    float luma2 = luma_s;
    if(!horizontal_span) {
        luma1 = luma_w;
        luma2 = luma_e;
    }
    float gradient1 = abs(luma1 - luma_c);
    float gradient2 = abs(luma2 - luma_c);
    float luma_local_average;
    float gradient;
    float length_sign = 1;
    if(gradient1 >= gradient2) {
        luma_local_average = (luma1 + luma_c) * 0.5;
        gradient = gradient1 * FXAA_SEARCH_THRESHOLD;
        length_sign = -1;
    } else {
        luma_local_average = (luma2 + luma_c) * 0.5;
        gradient = gradient2 * FXAA_SEARCH_THRESHOLD;
    }

    #if FXAA_DEBUG_GRADIENT
        bool pair = gradient1 >= gradient2;
        return vec3(0.0, (1.0 - pair) * 1.0, pair * 1.0);
    #endif // FXAA_DEBUG_GRADIENT

    vec2 tex_pos_offset = tex_pos + (horizontal_span ? vec2(0, 0.5 * length_sign * frame_rcp.y) : vec2(0.5 * length_sign * frame_rcp.x, 0));

    bool done1 = false;
    bool done2 = false;
    float luma_end1 = luma_local_average;
    float luma_end2 = luma_local_average;
    vec2 step_offset = horizontal_span ? vec2(frame_rcp.x, 0) : vec2(0, frame_rcp.y);
    vec2 pos1 = step_offset;
    vec2 pos2 = -step_offset;
    for(int i = 0; i < FXAA_SEARCH_STEPS; ++i) {
        if(!done1) { luma_end1 = fxaa_luma(fxaa_texture_offset(tex, tex_pos_offset, pos1)); }
        if(!done2) { luma_end2 = fxaa_luma(fxaa_texture_offset(tex, tex_pos_offset, pos2)); }

        done1 = done1 || (abs(luma_end1 - luma_local_average) >= gradient);
        done2 = done2 || (abs(luma_end2 - luma_local_average) >= gradient);

        if(done1 && done2) { break; }
        if(!done1) { pos1 += step_offset; }
        if(!done2) { pos2 -= step_offset; }
    }

    float distance1 = pos1.x + pos1.y;
    float distance2 = -pos2.x - pos2.y;
    float shortest_distance = min(distance1, distance2);
    float edge_length = distance1 + distance2;
    bool edge_direction = distance1 < distance2;
    float luma_end = edge_direction ? luma_end1 : luma_end2;

    #if FXAA_DEBUG_NEGPOS
        return vec3(direction * 1.0, 0, (1.0 - direction) * 1.0);
    #endif // FXAA_DEBUG_NEGPOS

    if(luma_c < luma_local_average == luma_end < luma_local_average) {
        length_sign = 0;
    }

    float subpixel_offset = (0.5 - shortest_distance / edge_length) * length_sign;
    vec2 subpixel_offset_vec = horizontal_span ? vec2(0.0, subpixel_offset * frame_rcp.y) : vec2(subpixel_offset * frame_rcp.x, 0.0);
    vec3 rgb_final = fxaa_texture_offset(tex, tex_pos, subpixel_offset_vec);
    vec3 rgb_average = (rgb_c + rgb_n + rgb_nw + rgb_w + rgb_sw + rgb_s + rgb_se + rgb_e + rgb_ne) * (1.0 / 9.0);
    return fxaa_lerp(rgb_average, rgb_final, blend_lowpass);
}