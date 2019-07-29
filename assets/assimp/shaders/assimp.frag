#version 330 core

#define NR_DIFFUSE_MAPS 10
#define NR_SPECULAR_MAPS 10
#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D diffuse[NR_DIFFUSE_MAPS];
    sampler2D specular[NR_SPECULAR_MAPS];
    float shininess;

    int diffuse_count;
    int specular_count;
};

// Fragment shader output
out vec4 FragColor;

// Fragment position in world space
in vec3 fragmentPosition;

// Fragment normal in world space
in vec3 fragmentNormal;

// Texture coordinates
int vec2 textureCoords;

// Material properties
uniform Material material;

// Directional light for the sun
uniform DirectionalLight sun;

// Point light
uniform PointLight lights[NR_POINT_LIGHTS];

// Lamp light
uniform SpotLight lamp;

float calculateAttenuation(float distance, float Kc, float Kl, float Kq);
vec3 calculateAmbientLight(vec3 light, vec3 colour);
vec3 calculateDiffuseLight(vec3 direction, vec3 diffuse, vec3 colour, vec3 normal);
vec3 calculateSpecularLight(vec3 direction, vec3 specular, vec3 colour, vec3 normal, float shininess);
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection);


void main() {
    // *************************
    // ***   LIGHTING: SUN   ***
    // *************************
    vec3 sun_light =
        calculateDirectionalLight(sun, normalize(fragmentNormal), normalize(viewPosition - fragmentPosition));

    // *******************************
    // ***     LIGHTING: POINT     ***
    // *******************************
    vec3 point_light = vec3(0.0f);
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        point_light +=
            calculatePointLight(lights[i], normalize(fragmentNormal), normalize(viewPosition - fragmentPosition));
    }

    // *******************************
    // ***   LIGHTING: SPOTLIGHT   ***
    // *******************************
    vec3 spot_light = calculateSpotLight(lamp, normalize(fragmentNormal), normalize(viewPosition - fragmentPosition));

    // Calculate result
    vec3 result = vec3(0.0f);
    result      = result + sun_light;
    result      = result + point_light;
    result      = result + spot_light;

    // Mix the two texture value and blend our colour in
    FragColor = vec4(result, 1.0f);
}

float calculateAttenuation(float distance, float Kc, float Kl, float Kq) {
    return 1.0f / (Kc + Kl * distance + Kq * distance * distance);
}

vec3 calculateAmbientLight(vec3 light, vec3 colour) {
    return light * colour;
}
vec3 calculateDiffuseLight(vec3 direction, vec3 diffuse, vec3 colour, vec3 normal) {
    vec3 lightDirection  = normalize(direction);
    float diffuseStength = max(dot(normal, lightDirection), 0.0f);
    return diffuse * diffuseStength * colour;
}
vec3 calculateSpecularLight(vec3 direction, vec3 specular, vec3 colour, vec3 normal, float shininess) {
    vec3 lightDirection      = normalize(-direction);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularStrength   = pow(max(dot(direction, reflectionDirection), 0.0f), shininess);
    return specular * specularStrength * colour;
}
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);
    vec3 ambient        = vec3(0.0f);
    vec3 diffuse        = vec3(0.0f);
    vec3 specular       = vec3(0.0f);

    for (int i = 0; i < material.diffuse_count; ++i) {
        // Ambient lighting
        ambient += calculateAmbientLight(light.ambient, vec3(texture(material.diffuse[i], textureCoords)));

        // Diffuse lighting
        diffuse += calculateDiffuseLight(lightDirection,
                                         light.diffuse,
                                         vec3(texture(material.diffuse[i], textureCoords)),
                                         normalize(fragmentNormal));
    }

    for (int i = 0; i < material.specular_count; ++i) {
        // Specular lighting
        specular += calculateSpecularLight(viewDirection,
                                           light.specular,
                                           vec3(texture(material.specular[i], textureCoords)),
                                           normalize(fragmentNormal),
                                           material.shininess);
    }

    return ambient + diffuse + specular;
}
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragmentPosition);
    vec3 ambient        = vec3(0.0f);
    vec3 diffuse        = vec3(0.0f);
    vec3 specular       = vec3(0.0f);

    for (int i = 0; i < material.diffuse_count; ++i) {
        // Ambient lighting
        ambient += calculateAmbientLight(light.ambient, vec3(texture(material.diffuse[i], textureCoords)));

        // Diffuse lighting
        diffuse += calculateDiffuseLight(lightDirection,
                                         light.diffuse,
                                         vec3(texture(material.diffuse[i], textureCoords)),
                                         normalize(fragmentNormal));
    }

    for (int i = 0; i < material.specular_count; ++i) {
        // Specular lighting
        specular += calculateSpecularLight(viewDirection,
                                           light.specular,
                                           vec3(texture(material.specular[i], textureCoords)),
                                           normalize(fragmentNormal),
                                           material.shininess);
    }

    // Calculate attentuation
    float distance     = length(light.position - fragmentPosition);
    float attentuation = calculateAttenuation(distance, light.Kc, light.Kl, light.Kq);
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    return ambient + diffuse + specular;
}
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragmentPosition);
    vec3 ambient        = vec3(0.0f);
    vec3 diffuse        = vec3(0.0f);
    vec3 specular       = vec3(0.0f);

    for (int i = 0; i < material.diffuse_count; ++i) {
        // Ambient lighting
        ambient += calculateAmbientLight(light.ambient, vec3(texture(material.diffuse[i], textureCoords)));

        // Diffuse lighting
        diffuse += calculateDiffuseLight(lightDirection,
                                         light.diffuse,
                                         vec3(texture(material.diffuse[i], textureCoords)),
                                         normalize(fragmentNormal));
    }

    for (int i = 0; i < material.specular_count; ++i) {
        // Specular lighting
        specular += calculateSpecularLight(viewDirection,
                                           light.specular,
                                           vec3(texture(material.specular[i], textureCoords)),
                                           normalize(fragmentNormal),
                                           material.shininess);
    }

    // Calculate and apply intensity drop-off
    float theta     = dot(normalize(light.position - fragmentPosition), normalize(-light.direction));
    float intensity = clamp((theta - light.gamma) / (light.phi - light.gamma), 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;

    // Calculate attentuation
    float distance     = length(light.position - fragmentPosition);
    float attentuation = calculateAttenuation(distance, light.Kc, light.Kl, light.Kq);
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    return ambient + diffuse + specular;
}
