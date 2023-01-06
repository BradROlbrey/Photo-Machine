## Photogrammetry Machine

After discovering the concept of photogrammetry - synthesizing pictures of an object or scene to make a 3D model of it - I decided to make a robot to take pictures of items for me, so I could have 3D models of them!

A Raspberry Pi with a camera module takes pictures and controls an Arduino over I2C. Between each picture, the pi signals the Arduino to move the camera. The Arduino controls two TMC2208 stepper drivers, each of which controls a stepper motor. One motor is for rotating the armature around the object, and the other is for moving the camera up and down the armature. In this manner, pictures can be taken all around an object. I decided to spin the camera around the object instead of simply spinning the object itself; this way the background moves with the image, so as to not confuse the image processing software.

The Pi also provides Ethernet/USB so I can conveniently get those pictures to a more powerful computer. The pictures are imported into [Meshroom](https://alicevision.org/) (I had no part in creating this), which processes them to construct a textured 3D model of the object.

The orange pieces in the pictures below are all parts I 3D modeled in Autodesk Inventor and 3D printed on my Prusa MK3.


## How It Works

This is the setup of the first design.
![setup](https://github.com/BradROlbrey/Photo-Machine/blob/v1_results/setup.png?raw=true)
The Raspberry Pi and Arduino are on top of the table, while the mechanical part sits on a box below. The ribbon cable for the camera and the wires for the stepper motors come down, and this arrangement keeps them out of the way of the armature as it rotates. The big light strip across the top is to provide even lighting onto the object, and to prevent hard shadows. 

I set the pinecone in the middle, and set the armature to take 400 pictures of the object - 40 around by 10 up-and-down. Meshroom was able to piece together most of them over the course of 2-3 hours on my 6800k and gtx 970.
![Grid-of-cameras](https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Grid%20of%20cameras.png)

Meshroom creates a .obj file with texture maps, which imported flawlessly into Blender. I used Blender to trim away some of the extra vertices Meshroom incorporated from the environment around the pinecone and the spray-can cap it was sitting on.
<img src="https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Blender-Eevee-render.jpg" width="100%" />


The lighting of the scene the object was photoed in is preserved, baked into the texture maps, so the following image was rendered with Blender's Eevee engine, i.e. in real time without the need for ray-tracing.
<img src="https://raw.githubusercontent.com/BradROlbrey/Photo-Machine/v1_results/Pinecone.png" width="100%" />


## New Design

Same functionality but in a more put-together form factor, aside from the lighting. My dad did the metalwork (pop rivets and aluminum bending/cutting). The arm is the same, carried over from the previous model. The electronics are the same as well, but whereas previously they were strewn about the table top, they are now mounted to the frame of the photomachine. The camera appears to have ran off at some point.
![1-overview](https://user-images.githubusercontent.com/17125101/198064809-aea53960-e34f-4496-b2ac-d83edf908519.jpg)


Below is the Raspberry Pi, and above is the breadboard with the other components. Power comes in through the barrel plug in between. 

<img alt="Raspberry Pi Mounted CloseUp" src="https://user-images.githubusercontent.com/17125101/211061392-f3a7742a-5123-484f-b58d-f8734200073b.jpg" width="60%" />


On the left is a dc-to-dc converter for power. In the middle are the two TMC2208 stepper drivers, and on the right is the Arduino that controls them. The stepper motors run on 12V, passed through from the stepper drivers, while the drivers themselves run on 5V provided by the converter. The Arduino and Raspberry Pi also run on 5V.
![2-electronics_cropped](https://user-images.githubusercontent.com/17125101/198070086-30fa34a1-d19d-48c7-8493-4150f8b1e06c.jpg)


New upper stepper motor mount. Closeup of the wheel-reel mechanism on lower stepper motor. Lots of slack in the ribbon cable, and the lower stepper's wires, for when the armature spins 360 around the object. Don't mind the masking tape :)

<img src="https://user-images.githubusercontent.com/17125101/198067940-1436de13-2314-4371-bd11-aa8a580b4412.jpg" width="60%" />


Closeup of camera mount (an earlier version) with camera, showing it slotted into place.

<img src="https://user-images.githubusercontent.com/17125101/201186878-6733e588-1771-45b1-9bb7-181623c5eb90.jpg" width="60%" />

Closeup of camera mount (current version) without camera but attached to armature. You can see the eye screws where the fishing line attaches, allowing the lower stepper motor and its wheel/reel to pull the camera up and down the armature.

<img src="https://user-images.githubusercontent.com/17125101/198073189-1bd39de6-0d0a-4a27-b7a1-012a81ce77aa.jpg" width="60%" />


Closeup of the Raspberry Pi mounts. The mounts for the breadboard are similar. The arch gives it some spring to clamp the Pi and aluminum bar tight, keeping each side secure.

![Raspberry Pi Mounts CloseUp_crop1](https://user-images.githubusercontent.com/17125101/211068994-c4054af6-d4eb-4551-af1f-3253638f55d2.jpeg)
