#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Fragment shader output
out vec4 FragColor;

// Fragment position in view space to the fragment shader
in vec3 fragmentPosition;

// Fragment normal to the fragment shader
in vec3 fragmentNormal;

// Light position in view space
in vec3 fragmentLightPosition;

// Position of the light in view space
uniform vec3 viewPosition;

// Material properties
uniform Material material;

// Light properties
uniform Light light;

void main() {
    // Calculate ambient lighting
    vec3 ambient = light.ambient * material.ambient;

    // Calculate diffuse lighting
    vec3 norm            = normalize(fragmentNormal);
    vec3 lightDirection  = normalize(fragmentLightPosition - fragmentPosition);
    float diffuseStength = max(dot(norm, lightDirection), 0.0f);
    vec3 diffuse         = light.diffuse * (diffuseStength * material.diffuse);

    // Calculate specular lighting
    // Since we are working in view space the viewing position is at (0, 0, 0)
    vec3 viewDirection       = normalize(vec3(0.0f) - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float specularStrength   = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
    vec3 specular            = light.specular * (specularStrength * material.specular);

    // Mix the two texture value and blend our colour in
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
