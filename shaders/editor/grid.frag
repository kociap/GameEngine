#version 450 core

in vec3 world_pos;
out vec4 frag_color;

uniform vec3 camera_pos;
uniform vec2 rcp_res;

uniform vec4 axis_x_color;
uniform vec4 axis_z_color;
uniform vec4 axis_y_color;
uniform uint grid_flags;
#define GRID_AXIS_X 1
#define GRID_AXIS_Y 2
#define GRID_AXIS_Z 4
#define GRID_ENABLED 32

float log10(float v) {
  return log(v) / log(10.0);
}

float grid_factor(vec2 pos, float grid_size, vec2 grid_fwidth) {
    float half_size = grid_size * 0.5;
    vec2 grid = abs(mod(pos + half_size, grid_size) - half_size);
    grid /= grid_fwidth;
    float merged_grid = min(grid.x, grid.y);
    return 1.0 - smoothstep(0.0, 1.0, merged_grid);
}

void main() {
    vec4 grid_color = vec4(0.2, 0.2, 0.2, 0.5);
    float grid_fade_length = 100.0;
    float grid_fade_distance = 100.0;

    float line_width = 4.0;
    float line_width_adjusted = line_width * rcp_res.y / gl_FragCoord.w;
    float line_half_width = line_width_adjusted / 2.0;
    vec2 plane_coord = vec2(world_pos.x, world_pos.z);

    // Fade

    vec3 view_vec = camera_pos - world_pos;
    float view_dist = length(view_vec);
    vec3 view_vec_normalized = view_vec / view_dist;
    float fade = 1 - abs(view_vec_normalized.y);
    fade *= fade;
    fade = 1 - fade;
    fade *= 1.0 - smoothstep(0.0, grid_fade_length, view_dist - grid_fade_distance);

    vec2 grid_fwidth = fwidth(world_pos).xz;

    // Grid

    if((grid_flags & GRID_ENABLED) != 0) {
        float camera_height = max(abs(camera_pos.y), 10.0);
        float distance_linear = log10(camera_height);
        float distance_magnitude = floor(distance_linear);
        float distance_diff = distance_magnitude - (distance_linear - 1.0);

        float scale_a = pow(10.0, distance_magnitude - 1);
        float scale_b = scale_a * 10.0;
        float grid_a = grid_factor(plane_coord, scale_a, grid_fwidth);
        float grid_b = grid_factor(plane_coord, scale_b, grid_fwidth);

        frag_color = grid_color;
        frag_color.a *= grid_a * max(0.0, min(1.0, distance_diff * 2.0 - 1.0));
        frag_color = mix(frag_color, grid_color, grid_b);
    } else {
        frag_color = vec4(0.0);
    }

    // Axes

    if((grid_flags & GRID_AXIS_X) != 0) {
        float alpha = 1.0 - smoothstep(0.0, 1.0, abs(plane_coord.y) / grid_fwidth.y);
        frag_color = axis_x_color * alpha + frag_color * (1.0 - alpha);
    }

    if((grid_flags & GRID_AXIS_Z) != 0) {
        float alpha = 1.0 - smoothstep(0.0, 1.0, abs(plane_coord.x) / grid_fwidth.x);
        frag_color = axis_z_color * alpha + frag_color * (1.0 - alpha);
    }

    frag_color.a *= fade;
}
