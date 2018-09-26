/*
 *  Projections
 *
 *  Draw 27 cubes to demonstrate orthogonal & prespective projections
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=30.0;   //  Size of world

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))
#define rgb(r,g,b) glColor3ub(r,g,b)

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
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
static void cube(double x,double y,double z, double dx,double dy,double dz, double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

static void ship(double x, double y, double z, double dx, double dy, double dz, double th)
{
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th, 0, 1, 0);
  glScaled(dx,dy,dz);

  glColor3f(1,1,1);

  glBegin(GL_QUADS);

  rgb(176,190,197);

  //ship top base
  glVertex3f(-1,-1, 1);
  glVertex3f(+1,-1, 1);
  glVertex3f(+1,+1, 1);
  glVertex3f(-1,+1, 1);

  rgb(120,144,156);

  //ship bottom center
  glVertex3f(+1,-1, +1);
  glVertex3f(0,-1, 0);
  glVertex3f(0,+1, 0);
  glVertex3f(+1,+1, +1);

  glVertex3f(-1,-1,+1);
  glVertex3f(0,-1, 0);
  glVertex3f(0,+1, 0);
  glVertex3f(-1,+1, +1);

  glEnd();

  glBegin(GL_TRIANGLES);

  rgb(176,190,197);

  //ship top triangles
  glVertex3f(-1,+1, 1);
  glVertex3f(+1,+1, 1);
  glVertex3f(0,+2, 1);

  glVertex3f(-1,-1, 1);
  glVertex3f(0,-2, 1);
  glVertex3f(+1,-1, 1);

  rgb(120,144,156);

  //ship bottom triangles
  glVertex3f(-1,+1, 1);
  glVertex3f(0,+1, 0);
  glVertex3f(0,+2, 1);

  glVertex3f(+1,+1, 1);
  glVertex3f(0,+1, 0);
  glVertex3f(0,+2, 1);

  glVertex3f(-1,-1, 1);
  glVertex3f(0,-1, 0);
  glVertex3f(0,-2, 1);

  glVertex3f(+1,-1, 1);
  glVertex3f(0,-1, 0);
  glVertex3f(0,-2, 1);

  glEnd();

  //ship cargo level up
  rgb(141,110,99);
  cube(0,0,1.2, 0.5, 1.0, 0.2, 0);
  rgb(161,136,127);
  cube(0,0,1.6, 0.25, 0.75, 0.2, 0);
  rgb(188,170,164);
  cube(0, 0.25, 2.2, 0.1, 0.1, 0.4, 0);
  rgb(215,204,200);
  cube(0, -0.25, 2.0, 0.1, 0.1, 0.2, 0);

  glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere (version 1)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x,double y,double z,double dx, double dy, double dz)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

static void airplane(double x, double y, double z, double dx, double dy, double dz, double th)
{
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th, 0, 1, 0);
  glScaled(dx,dy,dz);

  //trunk
  rgb(224,224,224);
  cube(0,0,0, 0.5, 2, 0.4, 0);

  //wings
  rgb(144,164,174);
  cube(0,0.5,0, 3, 0.2, 0.05, 0);

  //jet propellers
  sphere(1.5,0.5,0,0.2, 0.5 ,0.2);
  sphere(-1.5,0.5,0,0.2, 0.5 ,0.2);

  //wing endings
  rgb(245,245,245);
  cube(3,0.5,0, 0.1, 0.3, 0.2, 90);
  cube(-3,0.5,0, 0.1, 0.3, 0.2, 90);

  //tail directors
  rgb(176,190,197);
  cube(0,-1.7,0.6, 0.1, 0.3, 0.4, 0);
  cube(0,-1.9,0.4, 1, 0.1, 0.1, 180);



  glBegin(GL_TRIANGLES);

  //airplane pointy head
  rgb(158,158,158);
  glVertex3f(-0.5,+2, 0.4);
  glVertex3f(+0.5,+2, 0.4);
  glVertex3f(0,+3, 0);

  glVertex3f(-0.5,+2, -0.4);
  glVertex3f(+0.5,+2, -0.4);
  glVertex3f(0,+3, 0);

  glVertex3f(+0.5,+2, 0.4);
  glVertex3f(+0.5,+2, -0.4);
  glVertex3f(0,+3, 0);

  glVertex3f(-0.5,+2, +0.4);
  glVertex3f(-0.5,+2, -0.4);
  glVertex3f(0,+3, 0);


  glColor3f(1,1,1);

  //propeller fan
  glVertex3f(0,+3, 0);
  glVertex3f(-0.1,+3, 1);
  glVertex3f(+0.1,+3, 1);

  glVertex3f(0,+3, 0);
  glVertex3f(-0.1,+3, -1);
  glVertex3f(+0.1,+3, -1);

  glVertex3f(0,+3, 0);
  glVertex3f(-0.4,+3, 1);
  glVertex3f(-0.6,+3, 1);

  glVertex3f(0,+3, 0);
  glVertex3f(-0.4,+3, -1);
  glVertex3f(-0.6,+3, -1);

  glVertex3f(0,+3, 0);
  glVertex3f(+0.4,+3, 1);
  glVertex3f(+0.6,+3, 1);

  glVertex3f(0,+3, 0);
  glVertex3f(+0.4,+3, -1);
  glVertex3f(+0.6,+3, -1);

  glVertex3f(0,+3, 0);
  glVertex3f(+1,+3, -0.1);
  glVertex3f(+1,+3, +0.1);

  glVertex3f(0,+3, 0);
  glVertex3f(-1,+3, -0.1);
  glVertex3f(-1,+3, +0.1);

  glVertex3f(0,+3, 0);
  glVertex3f(+1,+3, -0.4);
  glVertex3f(+1,+3, -0.6);

  glVertex3f(0,+3, 0);
  glVertex3f(-1,+3, +0.4);
  glVertex3f(-1,+3, +0.6);

  glVertex3f(0,+3, 0);
  glVertex3f(-1,+3, -0.4);
  glVertex3f(-1,+3, -0.6);

  glVertex3f(0,+3, 0);
  glVertex3f(+1,+3, +0.4);
  glVertex3f(+1,+3, +0.6);

  glEnd();
  glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   int i,j,k;
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   /*//  Draw cubes
   for (i=-2;i<=4;i++)
      for (j=-3;j<=1;j++)
         for (k=-8;k<=1;k++)
            cube(i,j,k , 0.1,0.3,0.1 , 0);*/

  rgb(41,182,246);
  cube(0,0,-10, 30,30,10,0);

  ship(0,0,0,1,1,1,0);
  ship(5,10,0,1,2,2,0);
  airplane(5,5,8,1,0.5,0.5,30);
  airplane(-5,-5,4,2,2,2,330);


   //  Draw axes
   glColor3f(1,1,1);
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
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
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
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Projections");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
