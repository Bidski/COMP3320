#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// Output fragment normal to the fragment shader
out vec3 fragmentNormal;

// Output fragment position in world space to the fragment shader
out vec3 fragmentPosition;

// Output texture coordinates to the fragment shader
out vec2 textureCoords;

// Transformation matrix
uniform mat4 Hwm;
uniform mat4 Hvw;
uniform mat4 Hcv;

void main() {
    // Set texture coordinates
    textureCoords = aTexCoords;

    // Set fragment normal
    // Inverse transpose to ensure that non-uniform scaling does not affect normal direction
    fragmentNormal = transpose(inverse(mat3(Hwm))) * aNormal;

    // Set world space fragment position
    fragmentPosition = (Hwm * vec4(aPosition, 1.0f)).xyz;

    // Set vertex position
    gl_Position = Hcv * Hvw * Hwm * vec4(aPosition, 1.0f);
}
