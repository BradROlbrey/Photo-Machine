A Raspberry Pi with a camera module controls an Arduino over I2C. Between each picture, the pi signals the Arduino to move the camera. The Arduino controls two TMC2208 stepper drivers, each of which controls a stepper motor. One motor is for rotating the armature around the object, and the other is for moving the camera up and down the armature. In this manner, pictures can be taken all around an object.

The pictures are then imported into [Meshroom](https://alicevision.org/), which constructs a textured 3D model of the object from the images.

# Prototype 1
This is the setup of the first design.
![alt text](https://github.com/BradROlbrey/Photo-Machine/blob/v1_results/setup.png?raw=true)
The Raspberry Pi and Arduino are on top of the table, while the mechanical part sits on a box below. The ribbon cable for the camera and the wires for the stepper motors come down, and this arrangement keeps them out of the way of the armature as it rotates. The big light strip across the top is to provide even lighting onto the object, and to prevent hard shadows. 

I set the pinecone in the middle, and set the armature to take 400 pictures of the object - 40 around by 10 up-and-down. Meshroom was able to piece together most of them over the course of 2-3 hours on my 6800k and gtx 970.
![alt text](https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Grid%20of%20cameras.png)

Meshroom creates a .obj file with texture maps, which imported flawlessly into Blender. I used Blender to trim away some of the extra vertices Meshroom incorporated from the environment around the pinecone and the spray-can cap it was sitting on.
![alt text](https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Blender-Eevee-render.jpg)

The lighting of the scene the object was photoed in is preserved, baked into the texture maps, so the following image was rendered with Blender's Eevee engine, i.e. in real time without the need for ray-tracing.
![alt text](https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Pinecone.png)
