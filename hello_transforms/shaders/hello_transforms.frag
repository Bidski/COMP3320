#version 330 core

// Fragment shader output
out vec4 FragColor;

// Colour from the vertex shader
in vec3 ourColour;

// Texture coordinates from the vertex shader
in vec2 TexCoord;

// Texture sampler
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

// Mixing ratio for texture
uniform float mixRatio;

void main() {
    // Get texture colour from first texture
    vec4 tex1 = texture(ourTexture1, TexCoord);

    // Get texture colour from second texture
    // The texture is upside down, so invert y-axis
    vec4 tex2 = texture(ourTexture2, vec2(TexCoord.x, 1.0f - TexCoord.y));

    // Mix the two texture value and blend our colour in
    FragColor = mix(tex1, tex2, mixRatio) * vec4(ourColour, 1.0f);
}
