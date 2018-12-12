/*
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
- w/W - Toggles skycube world
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

*/

#include "CSCIx229.h"
#include <stdbool.h>
#include <time.h>
#include <strings.h>
#include <dirent.h>

#define GL_NORMAL(a,b,c,p,q,r,x,y,z)  glNormal3d(((q-b)*(z-c))-((y-b)*(r-c)),-((p-a)*(z-c))-((x-a)*(r-c)),((p-a)*(y-b))-((x-a)*(q-b)))
#define rgb(r,g,b) glColor3ub(r,g,b)
#define PI 3.1415926
#define FPV_ANGLE 1
#define FPV_UNIT 0.01
#define MAX_FILENAME_LENGTH 32

int axes = 0;       //  Display axes
int mode = 1;       //  Projection mode
int move = 0;       //  Move light
int th = 0;         //  Azimuth of view angle
int ph = 0;         //  Elevation of view angle2
int fov = 55;       //  Field of view (for perspective)
double asp = 1;     //  Aspect ratio
double dim = 50.0;   //  Size of world

enum Axes {axisX, axisY, axisZ, axisNONE};
enum Axes axisSelect = axisNONE;
enum Translate {translatePOS, translateNEG, translateZERO, translateNONE};
enum Translate translate = translateNONE;
enum Translate xState = translateZERO, yState = translateZERO, zState = translateZERO;

int light = 1;      //  Lighting

// Light values
int one       =   1;  // Unit value
int distance  =   20;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

float z[65][65];       //  DEM data
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location
float zmag=5;          //  DEM magnification

bool show_sky, show_overlay, show_pb_screen;
int tex_skycube[3];
int tex_ufo[3];
int tex_ma, tex_pa, tex_cd;

float temp = 0;
bool boolFPV = false;
bool boolExit = false;
int FPVangle = 0;

unsigned int texture[10];
bool boolPromptYes = false;
bool boolPromptNo = false;
//unsigned int tex_pb_screen;

int    fly=1;    //  Animated flight
double roll=0;   //  Roll angle
double pitch=0;  //  Pitch angle
double yaw=0;    //  Yaw angle
int    pwr=100;  //  Power setting (%)
double X  = 0;   //  Location
double Y  = 0;   //  Location
double Z  = 0;   //  Location
double Dx = 1;   //  Direction
double Dy = 0;   //  Direction
double Dz = 0;   //  Direction
double Sx = 1;   //  Sideways
double Sy = 0;   //  Sideways
double Sz = 0;   //  Sideways
double Ux = 0;   //  Up
double Uy = 1;   //  Up
double Uz = 0;   //  Up
double Ox = 0;   //  LookAt
double Oy = 0;   //  LookAt
double Oz = 0;   //  LookAt
double Ex = 1;   //  Eye
double Ey = 1;   //  Eye
double Ez = 1;   //  Eye

int lm = 0;
double flag[64][64][3];
unsigned int tex_flag = 0;

int winX = 1000, winY = 1000, mouseX = 500, mouseY = 500;
int mainMenu, viewMenu;
bool boolRefreshViewMenu = true;
bool boolMissionAccomplished = false;
bool boolCollisionDetected = false;
unsigned int viewMenuFileEntries = 0;
char *filenameList;
unsigned int viewFileIndex;

enum EyeCapStates {IDLE, INIT, RUNNING, STOP};
enum EyeCapStates capState = IDLE;
enum EyeCapStates viewState = IDLE;
struct timespec current_time;

typedef struct EyeCap
{
  double Ex;   //  Eye
  double Ey;   //  Eye
  double Ez;   //  Eye
  double Ox;   //  LookAt
  double Oy;   //  LookAt
  double Oz;   //  LookAt
  double Ux;   //  Up
  double Uy;   //  Up
  double Uz;   //  Up
} EyeCap;

typedef struct {float x,y,z;} Point;
typedef struct {double x,y,z;} Location;

Location astronaut;
Location ufo;


static void draw_cube(double x, double y, double z, double dx, double dy ,double dz, double th);
static void draw_cylinder(float x, float y, float z, float th, float ph, float R,float H, unsigned int slices);
static void draw_sphere(double x, double y, double z, double dx, double dy, double dz);
static void refreshViewMenu(void);
static void exitProgram(void);

/* ############################################################################################################### */

static void eyeCapture(double Epx, double Epy, double Epz, double Eox, double Eoy, double Eoz, double Eux, double Euy, double Euz)
{
  EyeCap eye;
  static FILE *capture;
  static char filename[MAX_FILENAME_LENGTH];
  static unsigned long current = 0;
  //size_t bytes = 0;

  if(capState == INIT)
  {
    //printf("Entered INIT.\n");
    current = 0;
    bzero(&current_time, sizeof(time_t));
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    bzero(filename, MAX_FILENAME_LENGTH);
    sprintf(filename, "capture/eye_%ld.cap", current_time.tv_sec);
    //printf("Filename = %s.\n", filename);
    capture = fopen(filename, "wb");
    capState = RUNNING;
    glutChangeToMenuEntry(1, "Stop Capture", 1);
  }

  if(capState == RUNNING)
  {
    bzero(&eye, sizeof(eye));
    eye.Ex = Epx;
    eye.Ey = Epy;
    eye.Ez = Epz;
    eye.Ox = Eox;
    eye.Oy = Eoy;
    eye.Oz = Eoz;
    eye.Ux = Eux;
    eye.Uy = Euy;
    eye.Uz = Euz;

    //printf("EC: %lf %lf %lf %lf %lf %lf %lf %lf %lf.\n", eye.Ex, eye.Ey, eye.Ez, eye.Ox, eye.Oy, eye.Oz, eye.Ux, eye.Uy, eye.Uz);
    fwrite ((const void *) &eye , sizeof(eye), 1, capture);
    current++;
  }

  else if(capState == STOP)
  {
    fwrite(&current, sizeof(current), 1, capture);
    fclose(capture);
    //printf("STOP request. Bytes: %lu.\n", bytes);
    capState = IDLE;
    glutChangeToMenuEntry(1, "Start Capture", 1);
    refreshViewMenu();
  }
}

static void eyeCapViewer(double Epx, double Epy, double Epz, double Eox, double Eoy, double Eoz, double Eux, double Euy, double Euz)
{
  static EyeCap saveEye, currEye;
  static FILE *viewer;
  static unsigned char *import;
  static unsigned long current = 0, total_entries = 0;
  static char path[MAX_FILENAME_LENGTH + 8];
  //size_t bytes = 0;

  if(viewState == INIT)
  {
    //printf("Entered INIT.\n");
    total_entries = 0;
    current = 0;

    bzero(path, MAX_FILENAME_LENGTH + 8);
    sprintf(path, "capture/%s", filenameList + ((viewFileIndex - 2) * MAX_FILENAME_LENGTH));
    //printf("Reading capture file \"%s\"\n", path);
    viewer = fopen(path, "rb");

    if(viewer == NULL)
    {
      printf("Error reading capture file \"%s\"\n", filenameList + ((viewFileIndex - 2) * MAX_FILENAME_LENGTH));
      return;
    }

    fseek(viewer, -sizeof(unsigned long), SEEK_END);
    fread (&total_entries, sizeof(unsigned long), 1, viewer);
    //printf("Units read = %lu. Number of entries = %lu.\n", bytes, total_entries);

    import = (unsigned char *) malloc(sizeof(EyeCap) * total_entries);
    bzero(import, sizeof(EyeCap) * total_entries);
    rewind(viewer);
    fread (import, sizeof(unsigned char), sizeof(EyeCap) * total_entries, viewer);

    fclose(viewer);

    bzero(&saveEye, sizeof(saveEye));
    saveEye.Ex = Epx;
    saveEye.Ey = Epy;
    saveEye.Ez = Epz;
    saveEye.Ox = Eox;
    saveEye.Oy = Eoy;
    saveEye.Oz = Eoz;
    saveEye.Ux = Eux;
    saveEye.Uy = Euy;
    saveEye.Uz = Euz;

    viewState = RUNNING;
  }

  if(viewState == RUNNING)
  {
    bzero(&currEye, sizeof(currEye));
    currEye = *((EyeCap *) (import + (sizeof(EyeCap) * current)));

    Ex = currEye.Ex;
    Ey = currEye.Ey;
    Ez = currEye.Ez;
    Ox = currEye.Ox;
    Oy = currEye.Oy;
    Oz = currEye.Oz;
    Ux = currEye.Ux;
    Uy = currEye.Uy;
    Uz = currEye.Uz;

    current++;

    //printf("Running. Frame: %lu.\n", current);

    if(current == total_entries)
      viewState = STOP;
  }

  else if(viewState == STOP)
  {
    free(import);

    Ex = saveEye.Ex;
    Ey = saveEye.Ey;
    Ez = saveEye.Ez;
    Ox = saveEye.Ox;
    Oy = saveEye.Oy;
    Oz = saveEye.Oz;
    Ux = saveEye.Ux;
    Uy = saveEye.Uy;
    Uz = saveEye.Uz;

    viewState = IDLE;

    //printf("STOP request.");
  }
}


static void init_flag(void)
{
    for (int x=0; x<64; x++) {
        for (int y=0; y<64; y++) {
			flag[x][y][0] = (double) (x/3.0);
			flag[x][y][1] = (double) (y/5.0);
			flag[x][y][2] = (double) (sin((((x*45)/8.0)/360.0)*2.0*PI));
		}
	}
}

void draw_flag(double tx, double ty, double tz, double sx, double sy, double sz, double rx, double ry, double rz)
{
  unsigned int x, y;
  double state;
  static bool boolInit = true;

  if(boolInit == true)
  {
    init_flag();
    boolInit = false;
  }

  glPushMatrix();

  glTranslated(tx, ty, tz);
  glRotated(rx, 1, 0, 0);
  glRotated(ry, 0, 1, 0);
  glRotated(rz, 0, 0, 1);
  glScaled(sx, sy, sz);

  glBindTexture(GL_TEXTURE_2D, texture[3]);
  draw_cylinder(-0.4, -2.25, 0, 0, 90, 0.8, 15, 8);
  draw_sphere(-0.4, +12.8, 0, 0.8, 1, 0.8);

	glColor3f(1, 1, 1);

  glBindTexture(GL_TEXTURE_2D, tex_flag);

  for (x=0; x<63; x++)
  {
      for (y=0; y<63; y++)
      {
          glBegin(GL_QUADS);
          glShadeModel(GL_SMOOTH);
          GL_NORMAL(flag[x][y][0], flag[x][y][1], flag[x][y][2], flag[x+1][y][0], flag[x+1][y][1], flag[x+1][y][2], flag[x][y+1][0], flag[x][y+1][1], flag[x][y+1][2]);
          glTexCoord2f(x/63.0, y/63.0);     glVertex3f(flag[x][y][0], flag[x][y][1], flag[x][y][2]);
          glTexCoord2f(x/63.0, (y+1)/63.0);   glVertex3f(flag[x][y+1][0], flag[x][y+1][1], flag[x][y+1][2]);
          glTexCoord2f((x+1)/63.0, (y+1)/63.0); glVertex3f(flag[x+1][y+1][0], flag[x+1][y+1][1], flag[x+1][y+1][2]);
          glTexCoord2f((x+1)/63.0, y/63.0);   glVertex3f(flag[x+1][y][0], flag[x+1][y][1], flag[x+1][y][2]);
          glEnd();
      }
  }

  glPopMatrix();

	for (y=0; y<64; y++)
  {
		state = flag[0][y][2];
		for (x=0; x<63; x++)
			flag[x][y][2] = flag[x+1][y][2];
		flag[63][y][2] = state;
	}
}

/*
 *  Draw the cockpit as an overlay
 *  Must be called last
 */
void draw_overlay(int lTex, int rTex)
{
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   glMatrixMode(GL_PROJECTION);

   glPushMatrix();

   glLoadIdentity();
   glOrtho(-asp,+asp,-1, 1,-1, 1);
   glMatrixMode(GL_MODELVIEW);

   glPushMatrix();
   glLoadIdentity();
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, lTex);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-0.5,-0.125);
   glTexCoord2d(1,0);glVertex2f(+0.0,-0.125);
   glTexCoord2d(1,1);glVertex2f(+0.0, +0.125);
   glTexCoord2d(0,1);glVertex2f(-0.5, +0.125);
   glEnd();

   glBindTexture(GL_TEXTURE_2D, rTex);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(+0.0,-0.125);
   glTexCoord2d(1,0);glVertex2f(+0.5,-0.125);
   glTexCoord2d(1,1);glVertex2f(+0.5, +0.125);
   glTexCoord2d(0,1);glVertex2f(+0.0, +0.125);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);

   glPopMatrix();

   glPopAttrib();
}

void ReadDEM(char* file)
{
   int i,j;
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);
   for (j=0;j<=64;j++)
      for (i=0;i<=64;i++)
      {
         if (fscanf(f,"%f",&z[i][j])!=1) Fatal("Error reading saddleback.dem\n");
         if (z[i][j] < zmin) zmin = z[i][j];
         if (z[i][j] > zmax) zmax = z[i][j];
      }
   fclose(f);
}


void draw_mountains(double tx, double ty, double tz, double sx, double sy, double sz, double rx, double ry, double rz, float zmag_local)
{
   int i,j;
   double z0 = (zmin+zmax)/2;
   //  Apply texture to DEM wireframe

  glPushMatrix();

  glTranslated(tx, ty, tz);
  glRotated(rx, 1, 0, 0);
  glRotated(ry, 0, 1, 0);
  glRotated(rz, 0, 0, 1);
  glScaled(sx, sy, sz);

  //glScaled(0.0625, 0.0625, 0.0625);
  glColor3f(1,1,1);
  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  //glEnable(GL_CULL_FACE);
  for (i=0;i<64;i++)
     for (j=0;j<64;j++)
     {
        float x=16*i-512;
        float y=16*j-512;
        glBegin(GL_QUADS);
        glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);
        GL_NORMAL(x+ 0,y+ 0,zmag_local*(z[i+0][j+0]-z0), x+16,y+ 0,zmag_local*(z[i+1][j+0]-z0), x+ 0,y+16,zmag_local*(z[i+0][j+1]-z0));
        glTexCoord2f((i+0)/64.,(j+0)/64.); glVertex3d(x+ 0,y+ 0,zmag_local*(z[i+0][j+0]-z0));
        glTexCoord2f((i+1)/64.,(j+0)/64.); glVertex3d(x+16,y+ 0,zmag_local*(z[i+1][j+0]-z0));
        glTexCoord2f((i+1)/64.,(j+1)/64.); glVertex3d(x+16,y+16,zmag_local*(z[i+1][j+1]-z0));
        glTexCoord2f((i+0)/64.,(j+1)/64.); glVertex3d(x+ 0,y+16,zmag_local*(z[i+0][j+1]-z0));
        glEnd();
     }
  //glDisable(GL_CULL_FACE);
  //glDisable(GL_DEPTH_TEST);
  //glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

/*
 *  Draw sky Cube
 */
static void draw_skycube(double D)
{
  glPushMatrix();
   glColor3f(1,1,1);
   //glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,tex_skycube[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,tex_skycube[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   glPopMatrix();

   //glDisable(GL_TEXTURE_2D);
}



/*
 *  Set projection
 */
void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov, asp, dim/16, 16*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim, +asp*dim, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void draw_cube(double x, double y, double z, double dx, double dy ,double dz, double th)
{
  //  Set specular color to white
  float white[] = {1,1,1,1};glNormal3f( 0, 0, +1);
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION, black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x, y, z);
   glRotated(th, 0, 1, 0);
   glScaled(dx, dy, dz);

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, +1);
   glTexCoord2f(0.0, 0.0);   glVertex3f(-1, -1, +1);
   glTexCoord2f(1.0, 0.0);   glVertex3f(+1, -1, +1);
   glTexCoord2f(1.0, 1.0);   glVertex3f(+1, +1, +1);
   glTexCoord2f(0.0, 1.0);   glVertex3f(-1, +1, +1);
   glEnd();

   glBegin(GL_QUADS);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(1.0, 0.0);    glVertex3f(+1, -1, -1);
   glTexCoord2f(0.0, 0.0);    glVertex3f(-1, -1, -1);
   glTexCoord2f(0.0, 1.0);    glVertex3f(-1, +1, -1);
   glTexCoord2f(1.0, 1.0);    glVertex3f(+1, +1, -1);
   glEnd();

   glBegin(GL_QUADS);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0, 1.0);    glVertex3f(+1, -1, +1);
   glTexCoord2f(0.0, 0.0);    glVertex3f(+1, -1, -1);
   glTexCoord2f(1.0, 0.0);    glVertex3f(+1, +1, -1);
   glTexCoord2f(1.0, 1.0);    glVertex3f(+1, +1, +1);
   glEnd();

   glBegin(GL_QUADS);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0, 0.0);    glVertex3f(-1, -1, -1);
   glTexCoord2f(0.0, 1.0);    glVertex3f(-1, -1, +1);
   glTexCoord2f(1.0, 1.0);    glVertex3f(-1, +1, +1);
   glTexCoord2f(1.0, 0.0);    glVertex3f(-1, +1, -1);
   glEnd();

   glBegin(GL_QUADS);
   //  Top
   glNormal3f( 0, +1, 0);
   glTexCoord2f(0.0, 1.0);    glVertex3f(-1, +1, +1);
   glTexCoord2f(1.0, 1.0);    glVertex3f(+1, +1, +1);
   glTexCoord2f(1.0, 0.0);    glVertex3f(+1, +1, -1);
   glTexCoord2f(0.0, 0.0);    glVertex3f(-1, +1, -1);
   glEnd();

   glBegin(GL_QUADS);
   //  Bottom
   glNormal3f( 0, -one, 0);
   glTexCoord2f(0.0, 0.0);    glVertex3f(-1, -1, -1);
   glTexCoord2f(1.0, 0.0);    glVertex3f(+1, -1, -1);
   glTexCoord2f(1.0, 1.0);    glVertex3f(+1, -1, +1);
   glTexCoord2f(0.0, 1.0);    glVertex3f(-1, -1, +1);
   //  End
   glEnd();

   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a cylinder
 */
static void draw_cylinder(float x, float y, float z, float th, float ph, float R,float H, unsigned int slices)
{
   int i,j,k;   // Counters
   int N=4*slices; // Number of slices

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(R,R,H);

   //  Two end caps (fan of triangles)
   for (j=-1;j<=1;j+=2)
      for (i=0;i<N;i++)
      {
         float th0 = j* i   *360.0/N;
         float th1 = j*(i+1)*360.0/N;
         Point P[3] = { {0,0,j} , {Cos(th0),Sin(th0),j} , {Cos(th1),Sin(th1),j} };
         Point N[3] = { {0,0,j} , {       0,       0,j} , {       0,       0,j} };
         Point T[3] = { {0,0,0} , {Cos(th0),Sin(th0),0} , {Cos(th1),Sin(th1),0} };

        glBegin(GL_POLYGON);
        for (k=0;k<3;k++)
        {
           glNormal3f(N[k].x,N[k].y,N[k].z);
           glTexCoord2f(T[k].x,T[k].y);
           glVertex3f(P[k].x,P[k].y,P[k].z);
        }
        glEnd();
      }
   //  Cylinder Body (strip of quads)
   for (i=0;i<N;i++)
   {
      float th0 =  i   *360.0/N;
      float th1 = (i+1)*360.0/N;
      Point P[4] = { {Cos(th0),Sin(th0),+1} , {Cos(th0),Sin(th0),-1} , {Cos(th1),Sin(th1),-1} , {Cos(th1),Sin(th1),+1} };
      Point N[4] = { {Cos(th0),Sin(th0), 0} , {Cos(th0),Sin(th0), 0} , {Cos(th1),Sin(th1), 0} , {Cos(th1),Sin(th1), 0} };
      Point T[4] = { {       0,th0/90.0, 0} , {       2,th0/90.0, 0} , {       2,th1/90.0, 0} , {       0,th1/90.0, 0} };

      glBegin(GL_POLYGON);
      for (k=0;k<4;k++)
      {
         glNormal3f(N[k].x,N[k].y,N[k].z);
         glTexCoord2f(T[k].x,T[k].y);
         glVertex3f(P[k].x,P[k].y,P[k].z);
      }
      glEnd();
   }

   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(int th,int ph)
{
   double x = -Sin(th)*Cos(ph);
   double y =  Cos(th)*Cos(ph);
   double z =          Sin(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0,ph/180.0+0.5);
   glVertex3d(x,y,z);
}

/*
 *  Draw a sphere at (x,y,z) radius (r)
 */
static void draw_sphere(double x, double y, double z, double dx, double dy, double dz)
{
   const int d = 5;
   int th, ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x, y, z);
   glScaled(dx, dy, dz);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);

   Vertex(0, -90);
   for (th = 0; th <= 360; th += d)
   {
      Vertex(th, d - 90);
   }
   glEnd();

   //  Latitude bands
   for (ph = d - 90; ph <= 90 - (2*d); ph += d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th = 0; th <= 360; th += d)
      {
         Vertex(th, ph);
         Vertex(th, ph + d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0, 90);
   for (th = 0; th <= 360; th += d)
   {
      Vertex(th, 90 - d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

// static void draw_blades(double x, double y, double z, double sx, double sy, double sz, double rx, double ry, double rz)
// {
//   glPushMatrix();
//   glTranslated(x, y, z);
//   glRotated(rx, 1, 0, 0);
//   glRotated(ry, 1, 0, 0);
//   glRotated(rz, 1, 0, 0);
//   glScaled(sx, sy, sz);
//
//   glColor3f(1,1,1);
//
//   glBegin(GL_TRIANGLES);
//
//   glNormal3d(0, +1, 0);
//   glVertex3f(0, +0, 0);
//   glVertex3f(-0.1, +0, +1);
//   glVertex3f(+0.1, +0, +1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-0.1, +0, -1);
//   glVertex3f(+0.1, +0, -1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-0.4, +0, 1);
//   glVertex3f(-0.6, +0, 1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-0.4, +0, -1);
//   glVertex3f(-0.6, +0, -1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(+0.4, +0, +1);
//   glVertex3f(+0.6, +0, +1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(+0.4, +0, -1);
//   glVertex3f(+0.6, +0, -1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(+1, +0, -0.1);
//   glVertex3f(+1, +0, +0.1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-1, +0, -0.1);
//   glVertex3f(-1, +0, +0.1);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(+1, +0, -0.4);
//   glVertex3f(+1, +0, -0.6);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-1, +0, +0.4);
//   glVertex3f(-1, +0, +0.6);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(-1, +0, -0.4);
//   glVertex3f(-1, +0, -0.6);
//
//   glVertex3f(0, +0, 0);
//   glVertex3f(+1, +0, +0.4);
//   glVertex3f(+1, +0, +0.6);
//
//   glEnd();
//
//   glPopMatrix();
// }

static void draw_ufo(double x, double y, double z, double r, double rx, double ry, double rz)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(180, 1, 0, 0);
   glRotated(rx, 0, 1, 0);
   glScaled(r,r,r);
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, tex_ufo[0]);
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   glPopMatrix();

   glPushMatrix();
   glTranslated(x, y - 0.3, z);
   glRotated(15, 1, 0, 0);
   glRotated(-ry, 0, 1, 0);
   glScaled(2*r,r/8,2*r);
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   glBindTexture(GL_TEXTURE_2D, tex_ufo[1]);
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   glPopMatrix();

   glPushMatrix();
   glTranslated(x, y - 0.3, z);
   glRotated(15, 1, 0, 0);
   glRotated(ry, 0, 1, 0);
   glScaled(r, r, r);
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   glBindTexture(GL_TEXTURE_2D, texture[3]);

   draw_cylinder(-1.25, -0.5, 0, 150, 90, 0.0625, 1, 8);
   draw_cylinder(+1.25, -0.5, 0, 210, 90, 0.0625, 1, 8);

   draw_cylinder(0, -0.5, -1.25, 0, 120, 0.0625, 1, 8);
   draw_cylinder(0, -0.5, +1.25, 0, 240, 0.0625, 1, 8);

   draw_sphere(-1.5, 0, 0, 0.25, 0.25, 0.25);
   draw_sphere(+1.5, 0, 0, 0.25, 0.25, 0.25);
   draw_sphere(0, 0, -1.5, 0.25, 0.25, 0.25);
   draw_sphere(0, 0, +1.5, 0.25, 0.25, 0.25);

   draw_sphere(-1.8, -1.4, 0, 0.25, 0.03125, 0.25);
   draw_sphere(+1.8, -1.4, 0, 0.25, 0.03125, 0.25);
   draw_sphere(0, -1.4, -1.8, 0.25, 0.03125, 0.25);
   draw_sphere(0, -1.4, +1.8, 0.25, 0.03125, 0.25);

   // draw_blades(0, 0, 0, 1.5, 1.5, 1.5, 0, 0, 0);

   glPopMatrix();
}

static void draw_airplane(double x, double y, double z, double dx, double dy, double dz)
{
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x, y, z);
  glRotated(270, 0, 0, 1);
  glRotated(270, 0, 1, 0);
  glRotated(0, 1, 0, 0);
  glScaled(dx, dy, dz);

  //Draw the trunk
  glBindTexture(GL_TEXTURE_2D,texture[1]);
  rgb(224, 224, 224);
  draw_cube(0, 0, 0, 0.5, 2, 0.4, 0);

  glBindTexture(GL_TEXTURE_2D,texture[0]);
  //Draw the wings
  rgb(144, 164, 174);
  draw_cube(0, 0.5, 0, 3, 0.2, 0.05, 0);

  glDisable(GL_TEXTURE_2D);
  //Draw the jet propellers under the wings
  draw_sphere(1.5, 0.5, -0.2, 0.2, 0.5, 0.2);
  draw_sphere(-1.5, 0.5, -0.2, 0.2, 0.5 ,0.2);
  glEnable(GL_TEXTURE_2D);

  //Highlight the wing endings
  rgb(245, 245, 245);
  draw_cube(3, 0.5, 0, 0.1, 0.3, 0.2, 0);
  draw_cube(-3, 0.5, 0, 0.1, 0.3, 0.2, 0);

  //Draw the tail directors
  rgb(176, 190, 197);
  draw_cube(0, -1.9, 0.6, 0.1, 0.3, 0.4, 0);
  draw_cube(0, -2.0, 0.4, 1, 0.1, 0.1, 0);

  glBindTexture(GL_TEXTURE_2D,texture[1]);

  glBegin(GL_TRIANGLES);
  //Draw the head tip
  rgb(158, 158, 158);
  GL_NORMAL(-0.5,+2, 0.4, +0.5,+2, 0.4, 0, +3, 0);
  glTexCoord2f(0.0, 0.0);   glVertex3f(-0.5,+2, 0.4);
  glTexCoord2f(1.0, 0.0);   glVertex3f(+0.5,+2, 0.4);
  glTexCoord2f(0.5, 1.0);   glVertex3f(0, +3, 0);
  glEnd();

  glBegin(GL_TRIANGLES);
  GL_NORMAL(+0.5, +2, -0.4, -0.5, +2, -0.4, 0, +3, 0);
  glTexCoord2f(0.0, 0.0);   glVertex3f(-0.5, +2, -0.4);
  glTexCoord2f(1.0, 0.0);   glVertex3f(+0.5, +2, -0.4);
  glTexCoord2f(0.5, 1.0);   glVertex3f(0, +3, 0);
  glEnd();

  glBegin(GL_TRIANGLES);
  GL_NORMAL(+0.5, +2, -0.4, +0.5, +2, +0.4, 0, +3, 0);
  glTexCoord2f(1.0, 0.0);   glVertex3f(+0.5, +2, 0.4);
  glTexCoord2f(0.0, 0.0);   glVertex3f(+0.5, +2, -0.4);
  glTexCoord2f(0.5, 1.0);   glVertex3f(0, +3, 0);
  glEnd();

  glBegin(GL_TRIANGLES);
  GL_NORMAL(-0.5, +2, -0.4, -0.5, +2, +0.4, 0, +3, 0);
  glTexCoord2f(1.0, 0.0);   glVertex3f(-0.5, +2, +0.4);
  glTexCoord2f(0.0, 0.0);   glVertex3f(-0.5, +2, -0.4);
  glTexCoord2f(0.5, 1.0);   glVertex3f(0, +3, 0);
  glEnd();

  glPopMatrix();
}

/*
 *  Draw Flight
 */
static void DrawFlight(double x0,double y0,double z0,
                       double dx,double dy,double dz,
                       double ux,double uy,double uz)
{
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double M[16] = {X0,Y0,Z0,0 , X1,Y1,Z1,0 , X2,Y2,Z2,0 , 0,0,0,1};

   //  Save current transforms
   glPushMatrix();
   glTranslated(x0,y0,z0);
   glMultMatrixd(M);

   glPushMatrix();
   glRotated(yaw,0,1,0);
   glRotated(pitch,0,0,1);
   glRotated(roll,1,0,0);

   draw_airplane(0,0,0,0.5,0.5,0.5);
   glPopMatrix();
   glPopMatrix();
}


static void init_pb_screen(unsigned int *tex_list, const char * path, unsigned int count)
{
  unsigned int current = 0;
  unsigned int path_length = strlen(path);
  char full_path[path_length+10];

  for(current = 0; current < count; current++)
  {
    sprintf(full_path, "%s-%u.bmp", path, current + 1);
    *(tex_list + current) = LoadTexBMP(full_path);
  }
}

/*
 *  Generates a polygon screen and plays multiple frames on it.
 */
static void draw_playback_screen(double tx, double ty, double tz,
                            double s,
                            double rx, double ry, double rz,
                            const char * path, unsigned int count)
{
  static unsigned int current = 0;
  static bool init = true;
  static unsigned int *tex_pb_screen;


  if(init == true)
  {
    tex_pb_screen = (unsigned int *) malloc(sizeof(unsigned int) * count);
    init_pb_screen(tex_pb_screen, path, count);
    init = false;
  }

  glPushMatrix();
  glTranslated(tx, ty, tz);
  glRotated(ry, 0, 1, 0);
  glRotated(rz, 0, 0, 1);
  glRotated(rx, 1, 0, 0);
  glScaled(s, s, 1);

  glBindTexture(GL_TEXTURE_2D, texture[3]);
  draw_cylinder(-2.0625, -1.5, 0, 0, 90, 0.0625, 2.5, 8);
  draw_cylinder(+2.0625, -1.5, 0, 0, 90, 0.0625, 2.5, 8);
  draw_cylinder(0, +1.0625 , 0, 90, 0, 0.0625, 2, 8);
  draw_cylinder(0, -1.0625 , 0, 90, 0, 0.0625, 2, 8);
  draw_sphere(-2.0625, 1, 0, 0.0625, 0.25, 0.0625);
  draw_sphere(+2.0625, 1, 0, 0.0625, 0.25, 0.0625);


  glBindTexture(GL_TEXTURE_2D, *(tex_pb_screen + current));
  glBegin(GL_QUADS);
  //  Front
  glNormal3f( 0, 0, +1);
  glTexCoord2f(0.0, 0.0);   glVertex3f(-2, -1, +0);
  glTexCoord2f(1.0, 0.0);   glVertex3f(+2, -1, +0);
  glTexCoord2f(1.0, 1.0);   glVertex3f(+2, +1, +0);
  glTexCoord2f(0.0, 1.0);   glVertex3f(-2, +1, +0);
  glEnd();

  glPopMatrix();

  current++;
  if(current == count)
    current = 0;
}

static void refreshViewMenu(void)
{
  DIR *dir;
  unsigned int current = 0;
	struct dirent *ent;

  if(viewMenuFileEntries != 0)
  {
    free(filenameList);

    glutSetMenu(viewMenu);
    for(current = 2; current < (viewMenuFileEntries + 2); current++)
      glutRemoveMenuItem(2);
  }

  viewMenuFileEntries = 0;

	if((dir = opendir("capture/")) != NULL)
	{
  	while((ent = readdir(dir)) != NULL)
		{
      if((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0))
          viewMenuFileEntries++;
		}
    closedir(dir);
  }

  filenameList = (char *) malloc(sizeof(char) * viewMenuFileEntries * MAX_FILENAME_LENGTH);
  bzero(filenameList, sizeof(char) * viewMenuFileEntries * MAX_FILENAME_LENGTH);
  current = 0;

  if((dir = opendir("capture/")) != NULL)
  {
    while((ent = readdir(dir)) != NULL)
    {
      if((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0))
      {
          sprintf(filenameList + (MAX_FILENAME_LENGTH * current), "%s", ent->d_name);
          current++;
      }
    }
    closedir(dir);
  }

  glutSetMenu(viewMenu);
  for(current = 2; current < (viewMenuFileEntries + 2); current++)
    glutAddMenuEntry(filenameList + (MAX_FILENAME_LENGTH * (current - 2)), current);


  glutSetMenu(mainMenu);
}

static void draw_obj(double tx, double ty, double tz, double sx, double sy, double sz, double rx, double ry, double rz)
{
  glPushMatrix();

  glTranslated(tx, ty - sy, tz);
  glRotated(rx, 1, 0, 0);
  glRotated(ry, 0, 1, 0);
  glRotated(rz, 0, 0, 1);
  glScaled(sx, sy, sz);

  glBindTexture(GL_TEXTURE_2D, tex_flag);
  glCallList(lm);

  glPopMatrix();
}

static void updateLocation(Location *object)
{
  double step = 0.1;

  if(axisSelect != axisNONE && translate != translateNONE)
  {
    if(axisSelect == axisX)
      xState = translate;
    else if(axisSelect == axisY)
      yState = translate;
    else if(axisSelect == axisZ)
      zState = translate;

    axisSelect = axisNONE;
    translate = translateNONE;
  }

  if(xState == translatePOS)
    object->x += step;
  else if(xState == translateNEG)
    object->x -= step;
  else if(xState == translateZERO)
    object->x += 0;

  if(yState == translatePOS)
    object->y += step;
  else if(yState == translateNEG)
    object->y -= step;
  else if(yState == translateZERO)
    object->y += 0;

  if(zState == translatePOS)
    object->z += step;
  else if(zState == translateNEG)
    object->z -= step;
  else if(zState == translateZERO)
    object->z += 0;
}

static bool collisionDetect(Location a, Location b, double distance)
{
  double measured = 0;

  measured = sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2));

  //printf("Measured: %lf. Distance: %lf\n", measured, distance);

  if(measured <= distance)
    return true;
  else
    return false;
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len = 10;  //  Length of axes
   static unsigned int SpinAngle = 0;
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   // if((capState == RUNNING) || (capState == STOP))
   //  printf("DP: %lf %lf %lf %lf %lf %lf %lf %lf %lf.\n", Ex, Ey, Ez, Ox, Oy, Oz, Ux, Uy, Uz);

   eyeCapture(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);
   eyeCapViewer(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);
   gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);

   if(boolRefreshViewMenu == true)
   {
     refreshViewMenu();
     boolRefreshViewMenu = false;
   }

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

  //  Draw Sky Cube
   if(show_sky) draw_skycube(64);

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Emission[]  = {0.0,0.0,0.0,1.0};
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        float white[]     = {1,1,1,1};
        float Shinyness[] = {16};
        //  Light position
        float Position[]  = {distance*Cos(zh), ylight, distance*Sin(zh), 1.0};
        ufo.x = Position[0];
        ufo.y = Position[1];
        ufo.z = Position[2];

        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        draw_ufo(Position[0], Position[1], Position[2], 1, SpinAngle, SpinAngle, SpinAngle);

        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
       glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
       //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);

        glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
        glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
        glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0f);
        glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   }
   else
     glDisable(GL_LIGHTING);

  if(fly)
   DrawFlight(X,Y,Z , Dx,Dy,Dz , Ux,Uy,Uz);

  draw_flag(-25, -5, -40, 1, 1, 1, 0, 0, 0);

  if(show_pb_screen)
    draw_playback_screen(-25, -14.7, -35, 4, 0, 0, 0, "textures/playback/playback", 132);

   draw_mountains(0, -64, -40, 0.1250, 0.0625, 0.0625, 270, 0, 180, zmag + 5);
   draw_mountains(40, -64, 0, 0.1250, 0.0625, 0.0625, 270, 0, 90, zmag - 1);
   draw_mountains(0, -64, 40, 0.1250, 0.0625, 0.0625, 270, 0, 0, zmag + 2);
   draw_mountains(-40, -64, 0, 0.1250, 0.0625, 0.0625, 270, 0, 270, zmag - 3);

  updateLocation(&astronaut);
  draw_obj(astronaut.x , astronaut.y ,astronaut.z, 10,10,10, 0, SpinAngle, 0);

  if(boolCollisionDetected == false)
    boolCollisionDetected = collisionDetect(ufo, astronaut, 10);

  if(astronaut.y > 75)
    boolMissionAccomplished = true;

   //  Draw axes
   glColor3f(1,1,1);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   //  Switch off textures so it doesn't apply to -48the rest
   glDisable(GL_TEXTURE_2D);

   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   glDisable(GL_DEPTH_TEST);

   if(boolCollisionDetected == true)
   {
     draw_overlay(tex_cd, tex_pa);
     show_overlay = true;
     xState = translateNONE;
     yState = translateZERO;
     zState = translateZERO;
   }
   else if(boolMissionAccomplished == true)
   {
     draw_overlay(tex_ma, tex_pa);
     show_overlay = true;
     xState = translateNONE;
     yState = translateZERO;
     zState = translateZERO;
   }

   else
   {
     //  Display parameters
     glWindowPos2i(5, 45);
     Print("Angle  = %d %d   Dim = %1f\n   FOV = %d   Projection = %s   Light = %s   Zmag = %f   temp = %f   ", th, ph, dim, fov, mode ? "Perpective":"Orthogonal", light ? "On":"Off", zmag, temp);
     glWindowPos2i(5, 25);
     Print("X = %lf   Y = %lf   Z = %lf   Ex = %lf   Ey = %lf   Ez = %lf   Yaw = %lf   Pitch = %lf   Roll = %lf", X, Y, Z, Ex, Ey, Ez, yaw, pitch, roll);
     glWindowPos2i(5, 5);
     Print("Capture: %s   y = %lf", (capState == RUNNING)?"ON":"OFF", astronaut.y);
   }

   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();

  if(boolPromptYes == true)
  {
    boolCollisionDetected = false;
    boolMissionAccomplished = false;
    show_overlay = false;

    astronaut.x = 0;
    astronaut.y = -40;
    astronaut.z = 0;

    Ex = -0.0;
    Ey = 0.0;
    Ez = 60.0;
    boolPromptYes = false;
  }

  else if(boolPromptNo == true)
    exitProgram();

   SpinAngle += 2;

   if(SpinAngle == 360)
     SpinAngle = 0;

   if(boolExit == true)
    exit(0);
}

/*
 *  GLUT calls this routine every 50ms
 */
void timer(int toggle)
{
  static double rotationTh = 0;
  static double rotationPh = 0;

   //  Toggle movement
   if (toggle>0)
      move = !move;
   //  Increment light position
   else
      zh = (zh+5)%360;
   //  Animate flight using Lorenz transform
   if (fly)
   {
     double distX = (double) ((mouseX - (winX/2.0))/winX);
     double distY = (double) ((mouseY - (winY/2.0))/winY);
     rotationTh += (distX/10.0);
     rotationPh += (distY/10.0);

      //  Lorenz integration parameters
      double dt = 0.003;
      double s = -1.7;
      double b = 2.66;
      double r = 50;
      //  Old vectors
      double D,Nx,Ny,Nz;
      double Dx0 = Dx;
      double Dy0 = Dy;
      double Dz0 = Dz;
      double Ux0 = Ux;
      double Uy0 = Uy;
      double Uz0 = Uz;

      //  Fix degenerate case
      if (X==0 && Y==0 && Z==0) Y = Z = 40;
      //  Update position
      Dx = s*(Y-X);
      Dy = X*(r-Z)-Y;
      Dz = X*Y - b*Z;

      X += dt*Dx;
      Y += dt*Dy;
      Z += dt*Dz;
      //  Normalize DX
      D = sqrt(Dx*Dx+Dy*Dy+Dz*Dz);
      Dx /= D;
      Dy /= D;
      Dz /= D;
      //  Calculate sideways
      Sx  = Dy0*Dz-Dz0*Dy;
      Sy  = Dz0*Dx-Dx0*Dz;
      Sz  = Dx0*Dy-Dy0*Dx;
      //  Calculate Up
      Ux  = Dz*Sy - Dy*Sz;
      Uy  = Dx*Sz - Dz*Sx;
      Uz  = Dy*Sx - Dx*Sy;
      //  Normalize Up
      D = sqrt(Ux*Ux+Uy*Uy+Uz*Uz);
      Ux /= D;
      Uy /= D;
      Uz /= D;
      //  Eye and lookat position
      Ex = X-7*Dx;
      Ey = Y-7*Dy;
      Ez = Z-7*Dz;
      Ox = X;
      Oy = Y;
      Oz = Z;
      //  Next DX
      Nx = s*(Y-X);
      Ny = X*(r-Z)-Y;
      Nz = X*Y - b*Z;
      //  Pitch angle
      pitch = 180*acos(Dx*Dx0+Dy*Dy0+Dz*Dz0);
      //  Roll angle
      D = (Ux*Ux0+Uy*Uy0+Uz*Uz0) / (Dx*Dx0+Dy*Dy0+Dz*Dz0);
      if (D>1) D = 1;
      roll = (Nx*Sx+Ny*Sy+Nz*Sz>0?+1:-1)*960*acos(D);
      //  Yaw angle
      yaw = 0;
      //  Power setting (0-1)
      if (Dy>0.8)
         pwr = 100;
      else if (Dy>-0.2)
         pwr = 20+100*Dy;
      else
         pwr = 0;
   }

   //  Static Roll/Pitch/Yaw
   else
   {
       if(boolFPV)
       {
         Ox = Ex + (2*dim*Sin(FPVangle));
         Oy = Ey;
         Oz = Ez - (2*dim*Cos(FPVangle));

         ph = temp;
         Ux = 0; Uy = Cos(ph); Uz = 0;
       }

       else
       {
          Ex = -2*dim*Sin(th)*Cos(ph);
          Ey = +2*dim        *Sin(ph);
          Ez = +2*dim*Cos(th)*Cos(ph);
          Ox = Oy = Oz = 0;
          X = Y = Z = 0;
          Dx = 1; Dy = 0; Dz = 0;
          Ux = 0; Uy = Cos(ph); Uz = 0;
      }
   }
   //  Set timer to go again
   if (move && toggle >= 0)
    glutTimerFunc(50, timer, 0);

    // eyeCapViewer(Ex,Ey,Ez , Ox,Oy,Oz , Ux,Uy,Uz);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the wfree(image);indow is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t, 360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  if(boolFPV)
   {
      if (key == GLUT_KEY_RIGHT)
          FPVangle += FPV_ANGLE;

      else if (key == GLUT_KEY_LEFT)
          FPVangle -= FPV_ANGLE;

      else if (key == GLUT_KEY_UP)
      {
          Ex += +2*dim*Sin(FPVangle) * FPV_UNIT;
          Ez += -2*dim*Cos(FPVangle) * FPV_UNIT;
      }

      else if (key == GLUT_KEY_DOWN)
      {
         Ex -= 2*dim*Sin(FPVangle) * FPV_UNIT;
         Ez -= -2*dim*Cos(FPVangle) * FPV_UNIT;
      }

      FPVangle %= 360;
   }

   else
   {
     //  Right arrow key - increase angle by 5 degrees
     if (key == GLUT_KEY_RIGHT)
        th += 5;
     //  Left arrow key - decrease angle by 5 degrees
     else if (key == GLUT_KEY_LEFT)
        th -= 5;
     //  Up arrow key - increase elevation by 5 degrees
     else if (key == GLUT_KEY_UP)
        ph += 5;
     //  Down arrow key - decrease elevation by 5 degrees
     else if (key == GLUT_KEY_DOWN)
        ph -= 5;
  }

   //  PageUp key - increase dim
  if (key == GLUT_KEY_PAGE_UP)
      dim -= 0.5;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim += 0.5;
    //  Smooth color model
    else if (key == GLUT_KEY_F1)
       smooth = 1-smooth;
    //  Local Viewer
    else if (key == GLUT_KEY_F2)
       local = 1-local;
    else if (key == GLUT_KEY_F3)
       distance = (distance==1) ? 5 : 1;
    //  Toggle ball increment
    else if (key == GLUT_KEY_F8)
       inc = (inc==10)?3:10;
    //  Flip sign
    else if (key == GLUT_KEY_F9)
       one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Update state
   timer(-1);

   //  Tell GLUT it is necessary to redisplay the scene
   //glutPostRedisplay();
}

static void exitProgram(void)
{
  if(capState == INIT)
    capState = IDLE;
  else if(capState == RUNNING)
    capState = STOP;

  if(viewState == INIT)
    viewState = IDLE;
  else if(viewState == RUNNING)
    viewState = STOP;

  boolExit = true;
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
       exitProgram();
    //  Reset view angle
    else if (ch == '0')
       th = ph = 0;
    //  Fly
    else if (ch == 'f' || ch == 'F')
      dim = (fly = !fly) ? 50 : 30;
    //  Toggle axes
    else if (ch == 'o' || ch == 'O')
       axes = 1-axes;
    //  Toggle lighting
    else if (ch == 'l' || ch == 'L')
       light = 1-light;
    //  Switch projection mode
    else if (ch == 'p' || ch == 'P')
       mode = 1-mode;
    //  Toggle light movement
    else if (ch == 'm' || ch == 'M')
    {
       //move = 1-move;
       timer(1);
     }
    //  Move light
    else if (ch == '<')
       zh += 1;
    else if (ch == '>')
       zh -= 1;
    //  Change field of view angle
    else if (ch == '-' && ch > 1)
       fov++;
    else if ((ch == '+' || ch == '=') && ch < 179)
       fov--;
    //  Light elevation
    else if (ch=='[')
       ylight -= 1;
    else if (ch==']')
       ylight += 1;
    else if (ch=='{')
       distance -= 1;
    else if (ch=='}')
       distance += 1;

    //  Ambient level
    else if (ch=='a' && ambient>0)
       ambient -= 5;
    else if (ch=='A' && ambient<100)
       ambient += 5;
    //  Diffuse level
    else if (ch=='d' && diffuse>0)
       diffuse -= 5;
    else if (ch=='D' && diffuse<100)
       diffuse += 5;
    //  Specular level
    else if (ch=='s' && specular>0)
       specular -= 5;
    else if (ch=='S' && specular<100)
       specular += 5;
    //  Emission level
    else if (ch=='e' && emission>0)
       emission -= 5;
    else if (ch=='E' && emission<100)
       emission += 5;
    //  Shininess level
    else if (ch=='n' && shininess>-1)
       shininess -= 1;
    else if (ch=='N' && shininess<7)
       shininess += 1;

    else if (ch == 'U')
      Ey += 0.25;
    else if (ch == 'u')
      Ey -= 0.25;

    else if ((ch == 'w') || (ch == 'W'))
      show_sky = !show_sky;
    else if (ch == 'o' || (ch == 'o'))
          show_overlay = !show_overlay;
    else if (ch == 'q' || (ch == 'q'))
          show_pb_screen = !show_pb_screen;
    else if (ch == 'v' || (ch == 'V'))
          boolFPV = !boolFPV;
    else if (ch == 'c' || (ch == 'C'))
    {
      if(viewState == IDLE)
      {
        if(capState == IDLE)
          capState = INIT;

        if(capState == RUNNING)
          capState = STOP;
      }
    }

    else if (ch == 'x' || (ch == 'X'))
      axisSelect = axisX;
    else if (ch == 'y' || (ch == 'Y'))
      axisSelect = axisY;
    else if (ch == 'z' || (ch == 'Z'))
      axisSelect = axisZ;

    else if (ch == '2' || (ch == 'b'))
      translate = translateNEG;
    else if (ch == '5' || (ch == 'g'))
      translate = translateZERO;
    else if (ch == '8' || (ch == 't'))
      translate = translatePOS;

    else if (ch == 'j')
        temp -= 0.1;
    else if (ch == 'k')
        temp += 0.1;

    //  Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);
    //  Reproject
    Project(mode?fov:0,asp,dim);
    //  Animate if requested
    //glutIdleFunc(move?idle:NULL);
    //  Tell GLUT it is necessary to redisplay the scene
    //  Update state
    timer(-1);
    //glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double) width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);
   //  Set projection
   winX = width;
   winY = height;

   Project();
}

// /*
//  *  Translate mouse (x,y) to world coordinates
//  */
// Point Mouse2World(int x,int y)
// {
//    Point p;
//    p.x = 2*asp*x       /(float)(width -1) - asp;
//    p.y = 2*(height-1-y)/(float)(height-1) - 1;
//    p.z = 0;
//    return p;
// }

// /*
//  *  Distance to point
//  */
// double Distance(Point p,int k)
// {
//    double dx = p.x - P[k].x;
//    double dy = p.y - P[k].y;
//    double dz = p.z - P[k].z;
//    return sqrt(dx*dx+dy*dy+dz*dz);
// }

/*
 *  GLUT calls this routine when a mouse is moved
 */
void motion(int x,int y)
{
   mouseX = x;
   mouseY = y;

   //printf("winX: %d\twinY: %d\tmouseX: %d\tmouseY: %d.\n", winX, winY, mouseX, mouseY);
}

/*
 *  GLUT calls this routine when a mouse button is pressed or released
 */
void mouse(int button, int state, int x, int y)
{
  if(show_overlay)
  {
     if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
     {
        if(x > winX/2)
        {
          if(y <= winY/2)
            boolPromptYes = true;
          else if(y > winY/2)
            boolPromptNo = true;

          /*printf("%s", boolPromptYes?"Yes\n":(boolPromptNo?"No\n":""));

          boolPromptYes = false;
          boolPromptNo = false;*/
        }
     }
   }
}


void mainMenuHandler(int value)
{
  if(value == 1)
  {
    if(viewState == IDLE)
    {
      if(capState == IDLE)
        capState = INIT;

      if(capState == RUNNING)
        capState = STOP;
    }
  }

  else if(value == 3)
    exitProgram();
}


void viewMenuHandler(int value)
{
  if(value == 1)
    boolRefreshViewMenu = true;

  if(value > 1)
  {
    printf("Hello\n");
    viewFileIndex = value;

    if((viewState == IDLE) && (capState == IDLE))
      viewState = INIT;
  }
}

static void glutMenuSetup(void)
{
  viewMenu = glutCreateMenu(viewMenuHandler);
    glutAddMenuEntry("Refresh", 1);

  mainMenu = glutCreateMenu(mainMenuHandler);
    glutAddMenuEntry("Start Capture", 1);
    glutAddSubMenu ("Viewer", viewMenu);
    glutAddMenuEntry("Exit", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char* argv[])
{
    show_sky = true;
    show_overlay = false;
    show_pb_screen = true;
    astronaut.x = 0;
    astronaut.y = -40;
    astronaut.z = 0;

    //  Initialize GLUT
    glutInit(&argc, argv);
    //  Request double buffered, true color window with Z buffering at 600x600
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(winX, winY);
    glutCreateWindow("Sandeep Raj Kumbargeri - Moon Moon Moon..");
    //glutFullScreen();

    //  Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMenuSetup();

    //  Load textures
    texture[0] = LoadTexBMP("textures/trunk.bmp");
    texture[1] = LoadTexBMP("textures/wings.bmp");
    texture[2] = LoadTexBMP("textures/rockies.bmp");
    texture[3] = LoadTexBMP("textures/metal.bmp");

    //  Load skybox texture
    tex_skycube[0] = LoadTexBMP("textures/skycube_sides.bmp");
    tex_skycube[1] = LoadTexBMP("textures/skycube_topbottom.bmp");

    tex_cd = LoadTexBMP("textures/cd.bmp");
    tex_ma = LoadTexBMP("textures/ma.bmp");
    tex_pa = LoadTexBMP("textures/pa.bmp");
    tex_ufo[0] = LoadTexBMP("textures/ufo1.bmp");
    tex_ufo[1] = LoadTexBMP("textures/ufo2.bmp");

    tex_flag = LoadTexBMP("textures/us.bmp");

    lm = LoadOBJ("obj/astronaut.obj");

    //  Load DEM
    ReadDEM("textures/saddleback.dem");

    timer(1);

    //texture[2] = LoadTexBMP("water.bmp");

    //glutIdleFunc(idle);

    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    return 0;
}
