Introduction
============

Casters
-------

Creates a screen and clears the background to have a dark-greenish background.

Renders ten cubes, each comprised of thirty triangles (five per face).
Rendering is accomplished by using an element buffer to render triangles via 
vertex indices.

Vertex attributes are used to set the colour for each of the 14 vertices in the 
cube (one on each corner and one in the center of each face). The center of each 
face is white, and each of the corners are either red, green, or blue.

Model, view, and projection matrices are used to transform the vertices in the 
cube. The model matrix is used to rotate the cube about an axis over time. The 
view matrix is used to set the view point of the camera. Finally, the projection 
matrix is used to set the field of view of the camera and to correct for the 
shape of viewport (all example before this one drew the square as a slight 
rectangle).

An individual model matrix is created for each of the ten cubes. This allows us 
to use the same vertices and vertex attributes for all ten cubes and use the 
individual model matrices to place each cube at a different position in world.

The keyboard can now be used to move the camera around (W: forwards, 
S: backwards, A: strafe left, D: strafe right, R: rise, F: fall). The mouse can 
also me used for rotating the camera (up/down: pitch and left/right: yaw) and 
mouse scrolling will zoom the view in and out.

Face normals are specified per vertex using vertex normals so that ambient, 
diffuse, and specular lighting can be calculated. Lighting is calculated in 
view space in the fragment shader.

Material and light structures are introduced to more easily facilitate per 
object material properties and multiple lights with individual properties.

We return to using textures to apply ambient/diffuse and specular maps. Vertex 
attributes are used, once again, to specify texture coordinates.

Multiple types of lights are implemented, including directional, point, and spot 
lights. One directional, four point, and one spot light are instantiated.