### CSCI 5229 Project - Chasing a UFO on the Moon

Author: Sandeep Raj Kumbargeri
CSCI 5229 - Computer Graphics - Fall 2018 - University of Colorado Boulder

Elements taken and extensively modified from Willem A. (Vlakkies) Schreuder's [Ex8 - Objects](http://www.prinmath.com/csci5229/F18/programs/ex8.zip),
[Ex9 - Projections](http://www.prinmath.com/csci5229/F18/programs/ex9.zip), [Ex13 - Lighting] (http://www.prinmath.com/csci5229/F18/programs/ex13.zip), [Ex14 - Texture Mapping] (http://www.prinmath.com/csci5229/F18/programs/ex14.zip), [Ex17 - Draped Textures] (http://www.prinmath.com/csci5229/F18/programs/ex17.zip), [Ex20 - Cockpit] (http://www.prinmath.com/csci5229/F18/programs/ex20.zip), [Ex24 - Display Lists] (http://www.prinmath.com/csci5229/F18/programs/ex24.zip), [Ex25 - Thunderbirds] (http://www.prinmath.com/csci5229/F18/programs/ex25.zip) and lastly [Ex25 - Thunderbirds] (http://www.prinmath.com/csci5229/F18/programs/ex25.zip).

Astronaut moon escape: The mission is to steer the astronaut away from the moon surface without crashing into the revolving UFO.

####Successfully implemented:

- SkyCube (Moon surface, stars and earth)
- Waving U.S Flag with its flagstaff (textures and lighting applied) (Assume this to be non-fabric flag driven by the staff as there is no atmosphere on the moon)
- Video Playback Screen with its plantable structure - Plays the movie of U.S astronauts planting flags on all different planets [Jockey - Planets] (https://youtu.be/FzCUhF5SHw4)
- Got this cool idea which I was very excited to work on -> Developed EyeCapture and EyeViewer functionality - Lets the user record their travel in the scene/world and bundles the data in .cap files. These can be played back using the right-mouse-button bound context menus in the game window. The eye is returned to the current view after playback.
- Developed dynamic menus with refreshing menu list functionality. The menus automatically detect the new files in the capture folder, lists them and further allows the user to manually refresh whenever needed. Also implemented auto-refresh to list the newly generated capture files after every capture. Also properly handled both capture and viewer state-machines in "exit the program" scenario while recording and playback.
- Developed objects with textures and lighting (Spacecraft, UFO). Imported and rendered Z2 Spacesuit OBJ model from [NASA 3D Resources] (https://nasa3d.arc.nasa.gov/detail/nmss-z2).  Implemented a way to translate the astronaut in all three axes and help him escape the moon.
- Real-Time Collision Detection of any object with any other. Used for detecting collision between the astronaut and the UFO, thus reaching the end of the game.
- Developed and implemented overlays for "Collision Detected" and "Mission Accomplished" scenarios and mapped the mouse pointer to allow the user to seamlessly choose between playing again and quitting the game.
- Eye capture and astronaut-play are independent and thus can be executed at the same time.
- Improved the first person viewing from homework 4. Now, you can look around and traverse through the world seamlessly.
- Mountains/Uneven-moon-surface with texture and lighting applied using draped textures.
- GIF/Video-Player (Implemented a virtual screen inside the scene, plays a small video (GIF image) in repeated playback fashion).


#### Challenges Faced and "Gotcha!s"
- I would like to express these during the "show and tell".

#### Suggested play methodology
- Once the program is built and opened, use the arrow keys to loo around the world in perspective mode. Use 'v' key to switch to the first person view and explore the world, use u/U to move up and down in Y-Axis. Observe the textures, lighting mountains (draped textures with lighting enabled).
- Observe the waving flag and the playback screen. Toggle the video playback screen using 'q'/'Q'. Press 'c' or right-mouse-click -> "Start Capture" to start capturing the eye.
- Press 'c' or right-mouse-click -> "Stop Capture" to stop recording. Use right-mouse-click -> View menu to find the recent recordings and click to play them. These recordings can be found at "./capture" directory. These are binary files and cannot be read using a text editor.
- You will be returned to the old view where you started to play the recordings. Drag and drop new files into the folder and make new recordings and see how the View menu automatically refreshed. You can also try "Refresh" option for refreshing it manually.
- Change the location of the UFO and observe lighting being applied to all the objects in the scene.
- Press X or Y or Z and then 8 or 5 or 2 to control the astronaut (look for the key-bindings info below). You can have him escape the world and the mission would be accomplished or have the UFO crash into him, in which case the collision is detected and the game is over. In both these cases, observe different overlay prompts show up on the screen. Use the left mouse key to choose to replay the game or exit the program.
- Also try exiting the program using "Esc" or right-mouse-button -> "Exit" while recording and playback and observe the state machines being handled properly.

#### Build instructions
- Run `make` in Windows, OS X and Linux. Download the make utility for Windows if needed.
- Run `make clean` to clean up the generated files.

Use arrow keys to change viewing angles

#### Special Key Bindings
- l/L - Toggles lighting
- a/A - Decrease/increase ambient light
- d/D - Decrease/increase diffuse light
- s/S - Decrease/increase specular light
- e/E - Decrease/increase emitted light
- n/N - Decrease/increase shininess
- F1 - Toggle smooth/flat shading
- F3 - Toggle light distance (1/5)
- w/W - Toggles viewing the SkyCube
- f/F - Fly mode toggle (flies the airplane using Lorenz Attractor coordinates - for EyeCapture & Viewer demonstration purpose only)
- [ ] - Lower/rise the UFO light source
- { } - Move the UFO light source near and away from the origin
- m/M - Toggle move (pause and un-pause motion in the scene)
- q/Q - Toggle the playback screen
- v/V - Toggle first person view and perspective view
- c/C - Start/Stop capturing or recording
- b/B - Toggle Playback Screen
- p/P - Toggles orthogonal/perspective projection
- +/- - Change field of view of perspective
- To move the astronaut, press x/X or y/Y or z/Z to select the axis of translation and press
8 once for continuous positive translation in the selected axis/direction
5 once for no translation in selected axis/Direction
2 once for continuous negative translation in the selected axis/direction
- o/O - Toggle axes
- arrows - Change view angle in both first person and perspective views
- PgDn/PgUp - Increase and Decrease the dimension of the world
- 0 - Reset view angle
- ESC - Safely do a graceful exit from the program

#### Mouse Click Bindings
- Right Click - Capture/Record, View and File-Select Menus
- Left Click - For Yes/No at Collision-Detection/Mission-Accomplished prompts

#### P.S
- The ErrCheck throws an error in draw_playback_screen function. This could probably be because the textures are being loaded in the middle of the program. That's my best guess as I happened to use gdb to find out that this is being cause by the LoadTexBMP for no real reason.
- I used few large textures as I use a 4K screen and smaller textures render very poorly when zoomed in. I have the smaller textures with me and am more than happy to replace the large textures.
