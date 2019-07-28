#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTextureCoords;

// Diffuse texture coordinates
out vec2 textureCoords;

// Output fragment position in view space to the fragment shader
out vec3 fragmentPosition;

// Output fragment normal to the fragment shader
out vec3 fragmentNormal;

// Output light position in view space to the fragment shader
out vec3 fragmentLightPosition;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

// World space light
uniform vec3 lightPosition;

void main() {
    // Set vertex position
    gl_Position = Hcv * Hvw * Hwm * vec4(aPosition, 1.0f);

    // Set view space fragment position
    fragmentPosition = (Hvw * Hwm * vec4(aPosition, 1.0f)).xyz;

    // Set view space light position
    fragmentLightPosition = (Hvw * vec4(lightPosition, 1.0f)).xyz;

    // Set fragment normal
    // Inverse transpose to ensure that non-uniform scaling does not affect normal direction
    fragmentNormal = transpose(inverse(mat3(Hvw * Hwm))) * aNormal;

    // Copy the diffuse texture coordinates to the fragment shader
    textureCoords = aTextureCoords;
}
