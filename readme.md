A Raspberry Pi with a camera module controls an Arduino over I2C. Between each picture, the pi signals the Arduino to move the camera. The Arduino controls two TMC2208 stepper drivers, each of which controls a stepper motor. One motor is for rotating the armature around the object, and the other is for moving the camera up and down the armature. In this manner, pictures can be taken all around an object.

The pictures are then imported into [Meshroom](https://alicevision.org/) (no, I had no part in creating this), which constructs a textured 3D model of the object from the images.

# Prototype 1
This is the setup of the first design.
![setup](https://github.com/BradROlbrey/Photo-Machine/blob/v1_results/setup.png?raw=true)
The Raspberry Pi and Arduino are on top of the table, while the mechanical part sits on a box below. The ribbon cable for the camera and the wires for the stepper motors come down, and this arrangement keeps them out of the way of the armature as it rotates. The big light strip across the top is to provide even lighting onto the object, and to prevent hard shadows. 

I set the pinecone in the middle, and set the armature to take 400 pictures of the object - 40 around by 10 up-and-down. Meshroom was able to piece together most of them over the course of 2-3 hours on my 6800k and gtx 970.
![Grid-of-cameras](https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Grid%20of%20cameras.png)

Meshroom creates a .obj file with texture maps, which imported flawlessly into Blender. I used Blender to trim away some of the extra vertices Meshroom incorporated from the environment around the pinecone and the spray-can cap it was sitting on.
<img src="https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Blender-Eevee-render.jpg" width="100%" />


The lighting of the scene the object was photoed in is preserved, baked into the texture maps, so the following image was rendered with Blender's Eevee engine, i.e. in real time without the need for ray-tracing.
<img src="https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Pinecone.png" width="100%" />

# Prototype 2
(but I haven't worked on it in a while so really more like final design 😅)

Same functionality but in a more put-together form factor, aside from the lighting. My dad did the metalwork (pop rivets and aluminum bending/cutting). There used to be a raspberry pi mounted to the back in the same manner the breadboard is attached, but it appears to have run off at some point, as well as the camera...
![1-overview](https://user-images.githubusercontent.com/17125101/198064809-aea53960-e34f-4496-b2ac-d83edf908519.jpg)

The wheel and arm are the same. The electronics are the same as well, but whereas previously they were strewn about the table top, they are now mounted to the frame of the photomachine on this breadboard. On the left is a dc-to-dc converter for power; you can see the barrel plug in the previous picture. In the middle are the two TMC2208s, and on the right is the arduino that controls them. ![2-electronics_cropped](https://user-images.githubusercontent.com/17125101/198070086-30fa34a1-d19d-48c7-8493-4150f8b1e06c.jpg)


New upper stepper motor mount. Closeup of the wheel-reel mechanism on lower stepper motor. Lots of slack in the ribbon cable, and the lower stepper's wires, for when the armature spins 360 around the object. Don't mind the masking tape :)

<img src="https://user-images.githubusercontent.com/17125101/198067940-1436de13-2314-4371-bd11-aa8a580b4412.jpg" width="60%" />


Closeup of camera mount, camera not included :p

You can see the eye screws where the fishing line attaches, allows the lower stepper motor and its wheel/reel to pull the camera up and down the armature.

<img src="https://user-images.githubusercontent.com/17125101/198073189-1bd39de6-0d0a-4a27-b7a1-012a81ce77aa.jpg" width="60%" />


Closeup of the breadboard mounts. The mounts for the raspberry pi were similar. The arch gives it some spring to clamp the board tight and keep it more secure.

<img src="https://user-images.githubusercontent.com/17125101/198073388-f0bed18c-129d-49d0-b770-1cb279cac40e.jpg" width="60%" />

I 3D modeled all printed parts in Autodesk Inventor and printed them on my Prusa MK3.
