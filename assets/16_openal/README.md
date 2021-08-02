Introduction
============

OPENAL
------

Creates a screen and clears the background to have a dark-greenish background.

Renders a single model comprised of multiple meshes. Model is loaded using the  
Open Asset Importer library.

Model, view, and projection matrices are used to transform the vertices in the  
model. The model matrix is used to rotate the model about the z-axis, as well as  
scale and translate it into a nice viewing position. The view matrix is used to  
set the view point of the camera. Finally, the projection matrix is used to set  
the field of view of the camera and to correct for the shape of viewport.

Diffuse and specular maps are used for object lighting. A directional light,  
multiple point lights, and a spot light are used to light the scene.

The keyboard can be used to move the camera around (W: forwards,  
S: backwards, A: strafe left, D: strafe right, R: rise, F: fall). The mouse can  
also me used for rotating the camera (up/down: pitch and left/right: yaw) and  
mouse scrolling will zoom the view in and out.

The spacebar can also be used to trigger the playback of an audio sound bite.  
The sound bite is loaded with libsndfile and played with OpenAL.
