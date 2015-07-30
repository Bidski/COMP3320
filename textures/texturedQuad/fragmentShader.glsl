#version 150 core

in vec3 Colour;
in vec2 TextureCoord;

out vec4 outColour;

uniform sampler2D tex;

void main()
{
    outColour = texture(tex, TextureCoord) * vec4(Colour, 1.0);
}

