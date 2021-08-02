Introduction
============

Multi-Cube
----------

Creates a screen and clears the background to have a dark-greenish background.

Renders ten cubes, each comprised of thirty triangles (five per face).
Rendering is accomplished by using an element buffer to render triangles via 
vertex indices.

Vertex attributes are used to set the colour for each of the 14 vertices in the 
cube (one on each corner and one in the center of each face). The center of each 
face is white, and each of the corners are either red, green, or blue.

Two textures are applied to the each face of the cube, a brick wall and a smiley 
face. The textures are mixed using a time-varying uniform. Texture coordinates 
are set using vertex attributes.

Model, view, and projection matrices are used to transform the vertices in the 
cube. The model matrix is used to rotate the cube about an axis over time. The 
view matrix is used to set the view point of the camera. Finally, the projection 
matrix is used to set the field of view of the camera and to correct for the 
shape of viewport (all example before this one drew the square as a slight 
rectangle).

An individual model matrix is created for each of the ten cubes. This allows us 
to use the same vertices and vertex attributes for all ten cubes and use the 
individual model matrices to place each cube at a different position in world.

