#version 330 core

// Fragment shader output
out vec4 FragColor;

// Colour from the vertex shader
in vec3 objectColour;

// Fragment position in view space to the fragment shader
in vec3 fragmentPosition;

// Fragment normal to the fragment shader
in vec3 fragmentNormal;

// Position of the light in view space
in vec3 fragmentLightPosition;

// Colour of the light
uniform vec3 lightColour;

void main() {
    // Calculate ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambientColour    = ambientStrength * lightColour;

    // Calculate diffuse lighting
    vec3 norm            = normalize(fragmentNormal);
    vec3 lightDirection  = normalize(fragmentLightPosition - fragmentPosition);
    float diffuseStength = max(dot(norm, lightDirection), 0.0f);
    vec3 diffuseColour   = diffuseStength * lightColour;

    // Calculate specular lighting
    // Since we are working in view space the viewing position is at (0, 0, 0)
    vec3 viewDirection       = normalize(vec3(0.0f) - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float specularStrength   = 0.5f * pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32.0f);
    vec3 specularColour      = specularStrength * lightColour;

    // Mix the two texture value and blend our colour in
    FragColor = vec4((ambientColour + diffuseColour + specularColour) * objectColour, 1.0f);
}
