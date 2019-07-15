#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;

// Output colour to the fragment shader
out vec3 ourColour;

void main() {
    // Set colour based on information from vertex attributes
    ourColour = aColour;

    gl_Position = vec4(aPosition.xyz, 1.0f);
}
