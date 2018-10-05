### Scene in 3D - Dunkirk

Author: Sandeep Raj Kumbargeri
CSCI 5229 - Computer Graphics - Fall 2018 - University of Colorado Boulder

Elements taken from Willem A. (Vlakkies) Schreuder's [Ex8 - Objects](http://www.prinmath.com/csci5229/F18/programs/ex8.zip) and
[Ex9 - Projections](http://www.prinmath.com/csci5229/F18/programs/ex9.zip)

Displays a scene consisting of two ships and two airplanes and allows the user to explore in 3D.

#### How it works - Overview
- This code is written using OpenGL API to draw objects and GLUT windowing API for window creation and resizing.
- Special keyboard functions are used to map certain key presses to definite actions by registering key function callbacks.
- Project function is used to switch between the orthogonal and prespective viewing modes and "display()" function is executed
whenever "glutPostRedisplay()" is called, which generates the transformation matrix based on what is to be displayed and flushes
the data to the screen.

#### Build instructions
- Run `make` in Windows, OS X and Linux. Download the make utility for Windows if needed.
- Run `make clean` to clean up the generated files.

Use arrow keys to change viewing angles

#### Special Key Bindings
- a/A - Toggle displaying axes
- c/C - Toggle displaying water
- m/M - Toggle the viewing mode (Orthogonal/Prespective)
- l/L - Increase field of view
- k/K - Decrease field of view
- w/W - Move X positive
- s/S - Move X negative
- a/A - Move Y negative
- d/D - Move Y positive
- z/Z - Move Z negative
- q/Q - Move Z positive
- 0 - Reset view angle
- ESC - Exit
