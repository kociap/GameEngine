#version 450 core

in Frag_data {
    vec3 fragment_position;
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

struct Point_light {
    vec3 position;
    vec3 color;
    float intensity;
    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;
    float diffuse_strength;
    float specular_strength;
};

uniform Material material;
uniform Point_light light;
uniform Camera camera;
uniform vec3 ambient_color;
uniform float ambient_strength;

out vec4 frag_color;

void main() {
    vec3 view_vec = normalize(camera.position - fragment_data.fragment_position);
    vec3 light_vec = normalize(light.position - fragment_data.fragment_position);
    vec3 half_vec = normalize(view_vec + light_vec);
    float source_frag_dist = length(light.position - fragment_data.fragment_position);
    vec3 tex_color = vec3(texture(material.texture_diffuse0, fragment_data.texture_coordinates));

    vec3 ambient = tex_color * ambient_color * ambient_strength;

    vec3 diffuse = tex_color * max(dot(fragment_data.normal, light_vec), 0.0);

    vec3 specular = pow(max(dot(fragment_data.normal, half_vec), 0.0), material.shininess) * light.color * tex_color;

    float attentuation = light.intensity / (light.attentuation_constant + source_frag_dist * light.attentuation_linear +
                                            source_frag_dist * source_frag_dist * light.attentuation_quadratic);

    diffuse *= attentuation * light.diffuse_strength;
    specular *= attentuation * light.specular_strength;
    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
