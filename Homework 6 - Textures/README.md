### Lighting the scene in 3D - Dunkirk

Author: Sandeep Raj Kumbargeri
CSCI 5229 - Computer Graphics - Fall 2018 - University of Colorado Boulder

Elements taken from Willem A. (Vlakkies) Schreuder's [Ex8 - Objects](http://www.prinmath.com/csci5229/F18/programs/ex8.zip),
[Ex9 - Projections](http://www.prinmath.com/csci5229/F18/programs/ex9.zip) and [Ex13 - Lighting] (http://www.prinmath.com/csci5229/F18/programs/ex13.zip)

Displays a scene consisting of two ships and two airplanes and allows the user to explore in 3D. All the objects in the scene are lighted using a lightball which can be moved around and adjusted using the key bindings.

#### How it works - Overview
- This code is written using OpenGL API to draw objects and GLUT windowing API for window creation and resizing.
- Special keyboard functions are used to map certain key presses to definite actions by registering key function callbacks.
- Project function is used to switch between the orthogonal and prespective viewing modes and "display()" function is executed whenever "glutPostRedisplay()" is called, which generates the transformation matrix based on what is to be displayed and flushes the data to the screen.
- The lighting is done locally at vertices using the normal vectors for that polygon plane.

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
- w/W - Toggles water
- [] - Lower/rise light
- p - Toggles ortogonal/perspective projection
- +/- - Change field of view of perspective
- x - Toggle axes
- arrows - Change view angle
- PgDn/PgUp - Zoom in and out
- 0 - Reset view angle
- ESC - Exit
