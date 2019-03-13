#version 450 core

in Frag_Data {
    mat3 tbn;
    vec3 fragment_position;
    vec3 light_space_frag_position;
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

struct Camera {
    vec3 position;
};

struct Point_Light {
    vec3 position;
    vec3 color;
    float intensity;
    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;
    float diffuse_strength;
    float specular_strength;
};

struct Directional_Light {
    vec3 color;
    vec3 direction;
    float intensity;
    float diffuse_strength;
    float specular_strength;
};

// Move to SSBO (somehow and someday)
uniform Point_Light[16] point_lights;
uniform Directional_Light[16] directional_lights;

uniform Material material;
uniform Camera camera;
uniform vec3 ambient_color;
uniform float ambient_strength;
uniform int point_lights_count;
uniform int directional_lights_count;
uniform sampler2D shadow_map;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 frag_color;

vec3 compute_directional_lighting(Directional_Light light, vec3 surface_normal, vec3 view_vec, vec3 tex_color, vec3 specular_tex_color);
vec3 compute_point_lighting(Point_Light light, vec3 view_vec, vec3 surface_normal, vec3 tex_color, vec3 specular_tex_color);
float compute_directional_shadow(vec3 light_space_frag_pos);
vec3 get_surface_normal(sampler2D normal_map, vec2 tex_pos);
vec3 color_to_normal(vec3 c);


void main() {
    vec3 surface_normal = material.normal_map_attached ? get_surface_normal(material.normal_map, fs_in.tex_coords) : fs_in.normal;
    vec3 light_color = vec3(0);
    vec3 view_vec = normalize(camera.position - fs_in.fragment_position);
    vec3 tex_color = vec3(texture(material.texture_diffuse, fs_in.tex_coords));
    vec3 ambient = tex_color * ambient_color * ambient_strength;

    vec3 specular_tex_color = vec3(texture(material.texture_specular, fs_in.tex_coords));
    for(int i = 0; i < point_lights_count; ++i) {
        light_color += compute_point_lighting(point_lights[i], surface_normal, view_vec, tex_color, specular_tex_color);
    }

    for(int i = 0; i < directional_lights_count; ++i) {
        float shadow_value = compute_directional_shadow(fs_in.light_space_frag_position);
        light_color += (1.0 - shadow_value) * compute_directional_lighting(directional_lights[i], surface_normal, view_vec, tex_color, specular_tex_color);
    }

    frag_color = vec4(ambient + light_color, 1.0);
}

vec3 color_to_normal(vec3 c) {
    return c * 2.0 - 1.0;
}

vec3 get_surface_normal(sampler2D normal_map, vec2 tex_pos) {
    vec3 tex_normal = textureLod(normal_map, tex_pos, 0.0).rgb;
    vec3 normal = color_to_normal(tex_normal);
    return normalize(fs_in.tbn * normal);
}

vec3 compute_point_lighting(Point_Light light, vec3 surface_normal, vec3 view_vec, vec3 tex_color, vec3 specular_tex_color) {
    vec3 light_vec = normalize(light.position - fs_in.fragment_position);
    vec3 half_vec = normalize(view_vec + light_vec);
    float source_frag_dist = length(light.position - fs_in.fragment_position);

    vec3 diffuse = tex_color * max(dot(surface_normal, light_vec), 0.0);

    vec3 specular = light.color * specular_tex_color * pow(max(dot(surface_normal, half_vec), 0.0), material.shininess);

    float attentuation = light.intensity / (light.attentuation_constant + source_frag_dist * light.attentuation_linear +
                                            source_frag_dist * source_frag_dist * light.attentuation_quadratic);

    diffuse *= attentuation * light.diffuse_strength;
    specular *= attentuation * light.specular_strength;
    return diffuse + specular;
}

vec3 compute_directional_lighting(Directional_Light light, vec3 surface_normal, vec3 view_vec, vec3 tex_color, vec3 specular_tex_color) {
    vec3 half_vec = normalize(view_vec - light.direction);
    vec3 diffuse = tex_color * max(0.0, dot(surface_normal, -light.direction));
    vec3 specular = specular_tex_color * light.color * pow(max(0.0, dot(surface_normal, half_vec)), material.shininess);
    diffuse *= light.intensity * light.diffuse_strength;
    specular *= light.intensity * light.specular_strength;
    return diffuse + specular;
}

float compute_directional_shadow(vec3 light_space_frag_pos) {
    vec3 coords = light_space_frag_pos * 0.5 + vec3(0.5);
    float lit_depth = texture(shadow_map, coords.xy).r;
    float current_depth = coords.z - 0.005;
    if(coords.z <= 1.0 && current_depth > lit_depth) {
        return 1.0;
    } else {
        return 0.0;
    }
}