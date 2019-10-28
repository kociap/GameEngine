#version 450 core

struct Point_Light {
    vec3 position;
    vec4 color;
    float intensity;
    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;
    float diffuse_strength;
    float specular_strength;
};

struct Directional_Light {
    vec4 color;
    vec3 direction;
    float intensity;
    float diffuse_strength;
    float specular_strength;
};

// Move to SSBO (somehow and someday)
layout(std140, binding = 0) uniform Lighting_Data {
    vec4 ambient_color;
    float ambient_strength;
    float todo_remove_shininess;
    int point_lights_count;
    int directional_lights_count;
    Point_Light[16] point_lights;
    Directional_Light[16] directional_lights;
};

struct Camera {
    vec3 position;
};

uniform Camera camera;

layout(binding = 0) uniform sampler2D gbuffer_position;
layout(binding = 1) uniform sampler2D gbuffer_normal;
layout(binding = 2) uniform sampler2D gbuffer_albedo_spec;

in vec2 tex_coords;
out vec4 frag_color;

vec3 compute_point_lighting(Point_Light light, vec3 surface_position, vec3 surface_normal, vec3 view_vec, vec3 albedo_color, float specular_factor);
vec3 compute_directional_lighting(Directional_Light light, vec3 surface_normal, vec3 view_vec, vec3 albedo_color, float specular_factor);

void main() {
    vec4 albedo_spec = texture(gbuffer_albedo_spec, tex_coords);
    if(albedo_spec == vec4(0.0)) {
        // Light doesn't bounce off pure black surfaces, so we can exit early.
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 albedo = albedo_spec.rgb;
    float specular = albedo_spec.a;
    vec3 surface_position = texture(gbuffer_position, tex_coords).rgb;
    vec3 surface_normal = texture(gbuffer_normal, tex_coords).rgb;
    vec3 view_vec = normalize(camera.position - surface_position);
    vec3 ambient = albedo * vec3(ambient_color) * ambient_strength;

    vec3 light_color = vec3(0);
    for(int i = 0; i < point_lights_count; ++i) {
        light_color += compute_point_lighting(point_lights[i], surface_position, surface_normal, view_vec, albedo, specular);
    }

    for(int i = 0; i < directional_lights_count; ++i) {
        light_color += compute_directional_lighting(directional_lights[i], surface_normal, view_vec, albedo, specular);
    }

    frag_color = vec4(ambient + light_color, 1.0);
}

vec3 compute_point_lighting(Point_Light light, vec3 surface_position, vec3 surface_normal, vec3 view_vec, vec3 albedo_color, float specular_factor) {
    vec3 light_vec = normalize(light.position - surface_position);
    vec3 half_vec = normalize(view_vec + light_vec);
    float source_frag_dist = length(light.position - surface_position);
    vec3 diffuse = albedo_color * max(dot(surface_normal, light_vec), 0.0);
    vec3 specular = vec3(specular_factor * pow(max(dot(surface_normal, half_vec), 0.0), todo_remove_shininess));

    float attentuation = light.intensity / (light.attentuation_constant + source_frag_dist * light.attentuation_linear +
                                            source_frag_dist * source_frag_dist * light.attentuation_quadratic);

    diffuse *= light.diffuse_strength;
    specular *= light.specular_strength;
    return attentuation * vec3(light.color) * (diffuse + specular);
}

vec3 compute_directional_lighting(Directional_Light light, vec3 surface_normal, vec3 view_vec, vec3 albedo_color, float specular_factor) {
    vec3 half_vec = normalize(view_vec - light.direction);
    vec3 diffuse = albedo_color * max(0.0, dot(surface_normal, -light.direction));
    vec3 specular = vec3(specular_factor * pow(max(0.0, dot(surface_normal, half_vec)), todo_remove_shininess));
    diffuse *= light.diffuse_strength;
    specular *= light.specular_strength;
    return light.intensity * vec3(light.color) * (diffuse + specular);
}