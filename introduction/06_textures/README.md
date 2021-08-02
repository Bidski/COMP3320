Introduction
============

Textures
--------

Creates a screen and clears the background to have a dark-greenish background.

Renders a single square comprised of four triangles.
Rendering is accomplished by using an element buffer to render triangles via 
vertex indices.

Vertex attributes are used to set the colour for each of the 5 vertices in the 
square (one on each corner and one in the center). Bottom left corner is red, 
top right corner is blue, top left and bottom right corners are green, center is 
white.

Two textures are applied to the square, a brick wall and a smiley face. The 
textures are mixed using a time-varying uniform. Texture coordinates are set 
using vertex attributes.

