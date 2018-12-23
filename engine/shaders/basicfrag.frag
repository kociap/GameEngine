#version 450 core

in vec3 normal;
in vec3 fragment_position;
in vec2 texture_coordinates;

struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light light;
uniform Material material;
uniform vec3 view_position;

out vec4 frag_color;

void main() {
    vec3 ambient = vec3(texture(material.diffuse_map, texture_coordinates));

    vec3 light_direction = normalize(light.position - fragment_position);
    float diffuse_strength = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diffuse_strength * vec3(texture(material.diffuse_map, texture_coordinates));

    vec3 view_direction = normalize(view_position - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_component = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular = vec3(texture(material.specular_map, texture_coordinates)) * specular_component * light.color;

    ambient = material.ambient_strength * ambient;
    diffuse = material.diffuse_strength * diffuse;
    specular = material.specular_strength * specular;
    frag_color = vec4(ambient + diffuse + specular, 1.0);
}