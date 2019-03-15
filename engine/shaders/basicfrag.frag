#version 450 core

in Frag_Data {
    mat3 tbn;
    vec3 fragment_position;
    vec3 normal;
    vec2 tex_coords;
} fs_in;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D normal_map;
    float shininess;
    bool normal_map_attached;
};

uniform Material material;

layout(location = 0) out vec3 gbuf_position;
layout(location = 1) out vec3 gbuf_normal;
layout(location = 2) out vec4 gbuf_albedo_spec;

vec3 get_surface_normal(sampler2D normal_map, vec2 tex_pos);
vec3 color_to_normal(vec3 c);

void main() {
    // vec3 surface_normal = material.normal_map_attached ? get_surface_normal(material.normal_map, fs_in.tex_coords) : fs_in.normal;
    vec3 surface_normal = fs_in.normal;
    vec4 tex_color = texture(material.texture_diffuse, fs_in.tex_coords);
    vec4 specular_tex_color = texture(material.texture_specular, fs_in.tex_coords);

    gbuf_position = fs_in.fragment_position;
    gbuf_normal = surface_normal;
    gbuf_albedo_spec = vec4(tex_color.rgb, specular_tex_color.r);
}

vec3 color_to_normal(vec3 c) {
    return c * 2.0 - 1.0;
}

vec3 get_surface_normal(sampler2D normal_map, vec2 tex_pos) {
    vec3 tex_normal = textureLod(normal_map, tex_pos, 0.0).rgb;
    vec3 normal = color_to_normal(tex_normal);
    return normalize(fs_in.tbn * normal);
}