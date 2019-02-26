#version 450 core

in Frag_data {
    vec3 fragment_position;
    vec3 light_space_frag_position;
    vec3 normal;
    vec2 texture_coordinates;
}
fragment_data;

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    float shininess;
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

uniform Material material;
uniform Point_Light[16] point_lights;
uniform Directional_Light[16] directional_lights;
uniform Camera camera;
uniform vec3 ambient_color;
uniform float ambient_strength;
uniform int point_lights_count;
uniform int directional_lights_count;
uniform sampler2D shadow_map;

out vec4 frag_color;

vec3 compute_directional_lighting(Directional_Light light, vec3 view_vec, vec3 tex_color);
vec3 compute_point_lighting(Point_Light light, vec3 view_vec, vec3 tex_color);
float compute_directional_shadow(vec3 light_space_frag_pos);

void main() {
    vec3 light_color = vec3(0);
    vec3 view_vec = normalize(camera.position - fragment_data.fragment_position);
    vec3 tex_color = vec3(texture(material.texture_diffuse0, fragment_data.texture_coordinates));
    for(int i = 0; i < point_lights_count; ++i) {
        light_color += compute_point_lighting(point_lights[i], view_vec, tex_color);
    }

    for(int i = 0; i < directional_lights_count; ++i) {
        float shadow_value = compute_directional_shadow(fragment_data.light_space_frag_position);
        light_color += (1.0 - shadow_value) * compute_directional_lighting(directional_lights[i], view_vec, tex_color);
    }

    vec3 ambient = tex_color * ambient_color * ambient_strength;
    frag_color = vec4(ambient + light_color, 1.0);
}

vec3 compute_point_lighting(Point_Light light, vec3 view_vec, vec3 tex_color) {
    vec3 light_vec = normalize(light.position - fragment_data.fragment_position);
    vec3 half_vec = normalize(view_vec + light_vec);
    float source_frag_dist = length(light.position - fragment_data.fragment_position);

    vec3 diffuse = tex_color * max(dot(fragment_data.normal, light_vec), 0.0);

    vec3 specular = pow(max(dot(fragment_data.normal, half_vec), 0.0), material.shininess) * light.color * tex_color;

    float attentuation = light.intensity / (light.attentuation_constant + source_frag_dist * light.attentuation_linear +
                                            source_frag_dist * source_frag_dist * light.attentuation_quadratic);

    diffuse *= attentuation * light.diffuse_strength;
    specular *= attentuation * light.specular_strength;
    return diffuse + specular;
}

vec3 compute_directional_lighting(Directional_Light light, vec3 view_vec, vec3 tex_color) {
    vec3 half_vec = normalize(view_vec - light.direction);
    vec3 diffuse = tex_color * max(0.0, dot(fragment_data.normal, -light.direction));
    vec3 specular = tex_color * light.color * pow(max(0.0, dot(fragment_data.normal, half_vec)), material.shininess);
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