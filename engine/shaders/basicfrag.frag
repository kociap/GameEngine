#version 450 core

in vec3 normal;
in vec3 fragment_position;
in vec2 texture_coordinates;

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
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
};

struct Camera {
    vec3 position;
    vec3 direction;
};

struct Point_light {
    vec3 position;
    vec3 color;

    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;

    float diffuse_strength;
    float specular_strength;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutoff_angle;
    float blend_angle;
    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;
    float intensity;
};

#define point_lights_number 1

uniform Light light;
uniform Point_light point_lights[point_lights_number];
uniform Camera camera;
uniform Material material;

out vec4 frag_color;

void main() {
    vec3 ambient = vec3(texture(material.texture_diffuse0, texture_coordinates));
    ambient = material.ambient_strength * ambient;

    vec3 light_direction = normalize(light.position - fragment_position);
    float angle = dot(light.direction, -light_direction);
    if (light.blend_angle < angle) {
        float blend_strength = clamp((light.blend_angle - angle) / (light.blend_angle - light.cutoff_angle), 0.0, 1.0);

        float diffuse_strength = max(dot(normal, light_direction), 0.0);
        vec3 diffuse = diffuse_strength * vec3(texture(material.texture_diffuse0, texture_coordinates));

        vec3 view_direction = normalize(camera.position - fragment_position);
        vec3 reflect_direction = reflect(-light_direction, normal);
        float specular_component = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
        vec3 specular = vec3(texture(material.texture_specular0, texture_coordinates)) * specular_component * light.color;

        float dist = length(light.position - fragment_position);
        float attentuation = light.intensity / (light.attentuation_constant + light.attentuation_linear * dist + light.attentuation_quadratic * dist * dist);

        diffuse = material.diffuse_strength * diffuse * attentuation * blend_strength;
        specular = material.specular_strength * specular * attentuation * blend_strength;
        frag_color = vec4(ambient + diffuse + specular, 1.0);
    } else {
        frag_color = vec4(ambient, 1.0);
    }
}
