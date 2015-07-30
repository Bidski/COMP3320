#version 150 core

in vec2 position;
in vec2 textureCoord;
in vec3 colour;

out vec3 Colour;
out vec2 TextureCoord;

void main()
{
    Colour = colour;
    TextureCoord = textureCoord;
    gl_Position = vec4(position, 0.0, 1.0);
}

