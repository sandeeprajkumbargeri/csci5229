/*
 *  Lorenz Attractor
 *
 *  Adapted by: Sandeep Raj Kumbargeri
 *  CSCI 5229 - Computer Graphics - Fall 2018
 *
 *  Originallly "Example 6 - Coordinates" written by Willem A. (Vlakkies) Schreuder
 *  Displays Lorenz Attractor in 3D and allows user to view it by changing its variables and viewing angles
 *
 *  Use arrow keys to change viewing angles

 *  Special Key Bindings:
 *  1      Increase S
 *  q      Decrease S
 *
 *  2      Increase B
 *  w      Decrease B
 *
 *  3      Increase R
 *  e      Decrease R
 *
 *  .     Increase point size
 *  ,     Decrease point size
 *
 *  0      Reset view angle
 *  ESC    Exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define AXES_LENGTH   15
#define ORTHO_DIMNSN  5

//  Globals
int th = 0;       // Azimuth of view angle
int ph = 0;       // Elevation of view angle
double dim = AXES_LENGTH + ORTHO_DIMNSN;   // Dimension of orthogonal box
unsigned int pix = 5;

//Lorenz attractor variables
double s  = 10;
double b  = 2.6666;
double r  = 28;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch = buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf, LEN, format, args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

/*
 *  Display the scene
 */
void display()
{
  unsigned int i = 0;

  /*  Coordinates  */
  double x = 1;
  double y = 1;
  double z = 1;

  /*  Time step  */
  double dt = 0.001;

  //  Clear the image
  glClear(GL_COLOR_BUFFER_BIT);
  //  Reset previous transforms
  glLoadIdentity();
  //  Set view angle
  glRotated(ph,1,0,0);
  glRotated(th,0,1,0);

  //  Set size of each point in the pixel units (default 5)
  glPointSize(pix);
  glBegin(GL_POINTS);

  /*
   *  Integrate 50,000 steps (50 time units with dt = 0.001)
   *  Explicit Euler integration
   */
  for (i = 0; i < 50000; i++)
  {
     double dx = s*(y-x);
     double dy = x*(r-z)-y;
     double dz = x*y - b*z;

     x += dt * dx;
     y += dt * dy;
     z += dt * dz;

     glColor3ub(((int) i) % 255,((int) i) % 128, ((int) i) % 64);
     glVertex3d(x/2,y/2,z/2);
  }

  glEnd();
  //  Draw axes in white
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3d(0,0,0);
  glVertex3d(AXES_LENGTH,0,0);
  glVertex3d(0,0,0);
  glVertex3d(0,AXES_LENGTH,0);
  glVertex3d(0,0,0);
  glVertex3d(0,0,AXES_LENGTH);
  glEnd();
  //  Label axes
  glRasterPos3d(AXES_LENGTH, 0, 0);
  Print("X");
  glRasterPos3d(0, AXES_LENGTH, 0);
  Print("Y");
  glRasterPos3d(0, 0, AXES_LENGTH);
  Print("Z");

  //  Display parameters
  glWindowPos2i(5, 5);
  Print("View Angle = %d (H) and %d (V)    s = %lf    b = %lf    r = %lf     Point Size = %u", th, ph, s, b, r, pix);

  //  Flush and swap
  glFlush();
  glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
  switch(ch)
  {
    case 27:   exit(0);       break;  //  Exit the program
    case '0':  th = ph = 0;   break;  //  Reset view angle
    case 'q':  s -= 0.01;     break;
    case '1':  s += 0.01;     break;
    case 'w':  b -= 0.01;     break;
    case '2':  b += 0.01;     break;
    case 'e':  r -= 0.01;     break;
    case '3':  r += 0.01;     break;
    case '.':  pix += 1;      break;
    case ',':  (pix > 1) ? (pix -= 1) : 0;   break;
  }
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height > 0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h, +dim*w2h, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(1000,1000);
   //  Create the window
   glutCreateWindow("Lorenz Attractor");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
