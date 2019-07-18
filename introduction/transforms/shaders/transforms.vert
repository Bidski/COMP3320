#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;
layout(location = 2) in vec2 aTexCoord;

// Output colour to the fragment shader
out vec3 ourColour;

// Output texture coordinates to the fragment shader
out vec2 TexCoord;

// Transformation matrix
uniform mat4 transform;

void main() {
    // Set colour based on information from vertex attributes
    ourColour = aColour;

    // Set coitexture coordinate based on information from vertex attributes
    TexCoord = aTexCoord;

    gl_Position = transform * vec4(aPosition.xyz, 1.0f);
}
