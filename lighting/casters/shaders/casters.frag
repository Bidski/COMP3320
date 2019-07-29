#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    // World space direction
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    // World space position
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Constant, linear, and quadratic terms for attenuation
    float Kc;
    float Kl;
    float Kq;
};

struct SpotLight {
    // World space position and direction
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Constant, linear, and quadratic terms for attenuation
    float Kc;
    float Kl;
    float Kq;

    // Radius of the inner and outer light cones
    float phi;
    float gamma;
};

// Fragment shader output
out vec4 FragColor;

// Diffuse texture coordinates
in vec2 textureCoords;

// Fragment position in world space
in vec3 fragmentPosition;

// Fragment normal in world space
in vec3 fragmentNormal;

// Light position in view space
in vec3 fragmentLightPosition;

// Position of the camera in world space
uniform vec3 viewPosition;

// Material properties
uniform Material material;

// Directional light for the sun
uniform DirectionalLight sun;

// Point light
uniform PointLight light;

// Lamp light
uniform SpotLight lamp;

vec3 calculateAmbientLight(vec3 light, vec3 colour) {
    return light * colour;
}
vec3 calculateDiffuseLight(vec3 direction, vec3 diffuse, vec3 colour, vec3 normal) {
    vec3 lightDirection  = normalize(direction);
    float diffuseStength = max(dot(normal, lightDirection), 0.0f);
    return diffuse * diffuseStength * colour;
}
vec3 calculateSpecularLight(vec3 direction, vec3 specular, vec3 colour, vec3 normal, float shininess) {
    // Since we are working in view space the viewing position is at (0, 0, 0)
    vec3 lightDirection      = normalize(-direction);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularStrength   = pow(max(dot(direction, reflectionDirection), 0.0f), shininess);
    return specular * specularStrength * colour;
}
float calculateAttenuation(float distance, float Kc, float Kl, float Kq) {
    return 1.0f / (light.Kc + light.Kl * distance + light.Kq * distance * distance);
}

void main() {
    // *************************
    // ***   LIGHTING: SUN   ***
    // *************************
    // Calculate ambient lighting
    vec3 sun_ambient = calculateAmbientLight(sun.ambient, vec3(texture(material.diffuse, textureCoords)));

    // Calculate diffuse lighting
    vec3 sun_diffuse = calculateDiffuseLight(
        sun.direction, sun.diffuse, vec3(texture(material.diffuse, textureCoords)), normalize(fragmentNormal));

    // Calculate specular lighting
    vec3 sun_specular = calculateSpecularLight(normalize(viewPosition - fragmentPosition),
                                               sun.specular,
                                               vec3(texture(material.specular, textureCoords)),
                                               normalize(fragmentNormal),
                                               material.shininess);

    // *******************************
    // ***   LIGHTING: DIRECTION   ***
    // *******************************
    // Calculate ambient lighting
    vec3 ambient = calculateAmbientLight(light.ambient, vec3(texture(material.diffuse, textureCoords)));

    // Calculate diffuse lighting
    vec3 diffuse = calculateDiffuseLight(normalize(light.position - fragmentPosition),
                                         light.diffuse,
                                         vec3(texture(material.diffuse, textureCoords)),
                                         normalize(fragmentNormal));

    // Calculate specular lighting
    vec3 specular = calculateSpecularLight(normalize(viewPosition - fragmentPosition),
                                           light.specular,
                                           vec3(texture(material.specular, textureCoords)),
                                           normalize(fragmentNormal),
                                           material.shininess);

    // Calculate attentuation
    float distance     = length(light.position - fragmentPosition);
    float attentuation = calculateAttenuation(distance, light.Kc, light.Kl, light.Kq);
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    // *******************************
    // ***   LIGHTING: SPOTLIGHT   ***
    // *******************************
    float theta = dot(normalize(lamp.position - fragmentPosition), normalize(-lamp.direction));

    // Calculate ambient lighting
    vec3 lamp_ambient = calculateAmbientLight(lamp.ambient, vec3(texture(material.diffuse, textureCoords)));

    // Calculate diffuse lighting
    vec3 lamp_diffuse = calculateDiffuseLight(normalize(lamp.position - fragmentPosition),
                                              lamp.diffuse,
                                              vec3(texture(material.diffuse, textureCoords)),
                                              normalize(fragmentNormal));

    // Calculate specular lighting
    vec3 lamp_specular = calculateSpecularLight(normalize(viewPosition - fragmentPosition),
                                                lamp.specular,
                                                vec3(texture(material.specular, textureCoords)),
                                                normalize(fragmentNormal),
                                                material.shininess);

    // Calculate and apply intensity drop-off
    float intensity = clamp((theta - lamp.gamma) / (lamp.phi - lamp.gamma), 0.0f, 1.0f);
    lamp_diffuse *= intensity;
    lamp_specular *= intensity;

    // Calculate and apply attentuation
    distance     = length(lamp.position - fragmentPosition);
    attentuation = calculateAttenuation(distance, lamp.Kc, lamp.Kl, lamp.Kq);
    lamp_ambient *= attentuation;
    lamp_diffuse *= attentuation;
    lamp_specular *= attentuation;

    // Calculate result
    vec3 result = vec3(0.0f);
    result      = result + ambient + diffuse + specular;
    result      = result + sun_ambient + sun_diffuse + sun_specular;
    result      = result + lamp_ambient + lamp_diffuse + lamp_specular;

    // Mix the two texture value and blend our colour in
    FragColor = vec4(result, 1.0f);
}
