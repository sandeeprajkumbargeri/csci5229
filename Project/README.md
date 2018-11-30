### CSCI 5229 Project - Chasing a UFO on the Moon

Author: Sandeep Raj Kumbargeri
CSCI 5229 - Computer Graphics - Fall 2018 - University of Colorado Boulder

Elements taken from Willem A. (Vlakkies) Schreuder's [Ex8 - Objects](http://www.prinmath.com/csci5229/F18/programs/ex8.zip),
[Ex9 - Projections](http://www.prinmath.com/csci5229/F18/programs/ex9.zip), [Ex13 - Lighting] (http://www.prinmath.com/csci5229/F18/programs/ex13.zip), [Ex14 - Texture Mapping] (http://www.prinmath.com/csci5229/F18/programs/ex14.zip), Ex17 - Draped Textures] (http://www.prinmath.com/csci5229/F18/programs/ex17.zip), Ex20 - Draped Textures] (http://www.prinmath.com/csci5229/F18/programs/ex20.zip) and lastly Ex25 - Thunderbirds] (http://www.prinmath.com/csci5229/F18/programs/ex25.zip).

Displays a scene consisting of a cubemap from the sruface of the moon with craters and mountains (draped textures) with stars and earth which can be seen on the top.

####Successfully implemented:

- Cubemap (Moon surface, stars and earth).
- Objects with textures and lighting (Ships, Spacecraft, UFO).
- Mountains/Uneven-moon-surface with texture applied (Used draped textures and provided user-support to change the depth and position).
- GIF/Video-Player (Implemented a virtual screen inside the scene, plays a small video (GIF image) in repeated playback fashion).
- Cockpit view in both fly-mode and prespective modes.

####Half-done:
- Attempted to integrate and fully develop flight capability with the spacecraft. Able to traverse the world in the directio of the lorenz attractor, but unable to render the spacecraft yet.
- Collision detection with the UFO. Once I'm able to move the spacecraft, the goal is to crash into the UFO and detect collisions in the background.

####Not yet implemented:
- Menu Items
- Flag with wavy movement
- Apollo Lander (stretch goal)

#### Challenges Faced and "Gotcha!s"
- The GIF-player seemed something very interesting to attempt. The goal was to use as less memory as possible. So, I used only one texture name and LoadTexBMP-ed to it with different textures for every 50ms to generate smooth image play-back. "This worked absolutely perfect!" is what I thought until I happened to find a segmentation fault. Upon observing the resource monitor and debugging using valgrind, there was a memory leak in the OpenGL calls. Need to dig more. Planning to use seperate texture names and then using them, if I don't find the memory leak.
- The flight emulation demonstrated in the class uses DisplayLists which I'm not using. Rendering my spacecraft and have it first follow the Lorenz attractor path is my first goal. Then I'll figure out how to follow the mouse or respond to keyboard inputs.
- Understanding the standard pipeline after the timer has been implementes has been a little challenging for me with idle, key, special functions calling glutPostRedisplay.

#### Suggested review methodology
- Compile and run the program using the following mentioned build instructions.
- The fly-mode is turned on at the start-up and you will see the eye following lorenz's eye. The flight rendering is to be worked on.
- Use the 'f/F' on the keyboard to turn off the fly-mode and you will enter prespective mode. This will show you play-back screen with the spacecraft and the UFO revolving aroung the spacecrat behind the screen. You could use 'b/B' on the keyboard to hide the screen.
- Use the arrow keys to look around and observe the sky-cube, mountains and ships in the scene. Use 'Z/z' to increase and decrease the vertical exaggeration of the mountains.
- Toggle the sky-cube using 'w/W' and this will show you the mountains completely.

#### Build instructions
- Run `make` in Windows, OS X and Linux. Download the make utility for Windows if needed.
- Run `make clean` to clean up the generated files.

Use arrow keys to change viewing angles

#### Special Key Bindings
- l - Toggles lighting
- a/A - Decrease/increase ambient light
- d/D - Decrease/increase diffuse light
- s/S - Decrease/increase specular light
- e/E - Decrease/increase emitted light
- n/N - Decrease/increase shininess
- F1 - Toggle smooth/flat shading
- F2 - Toggle local viewer mode
- F3 - Toggle light distance (1/5)
- F8 - Change ball increment
- F9 - Invert bottom normal
- m - Toggles light movement
- w/W - Toggles wskycube world
- f/F - Fly mode toggle
- [] - Lower/rise light
- m/M - Toggle move (pause and un-pause movement in the scene)
- q/Q - Toggle Cockpit
- b/B - Toggle Playback Screen
- p - Toggles ortogonal/perspective projection
- +/- - Change field of view of perspective
- x - Toggle axes
- arrows - Change view angle
- Z/z - Increase/Decrease vertical exaggeration
- PgDn/PgUp - Zoom in and out
- 0 - Reset view angle
- ESC - Exit
