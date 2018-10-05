//Alison Ostlund, CSCI4229, HW2 - Seattle Space Needle
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
int trans = 0;    //turn on the transformed objects
int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim= 8.0;   //  Size of world
double Ex = 0; //Eye pos
double Ey = 0;
double Ez = 15;
double Cx = 0; //camera pos
double Cz = 25;
int fp =0; //toggle for first person view
int fpangle=0;  //first person angle
int tamount = 0.05; //turning amount for first person
//color change


//ex7
//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

//taken from ex6
#define LEN 8192  // Maximum length of text string
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

// Z comp projection - ex7
double Zp(double x,double y,double z){
    return -Sin(th)*Cos(ph)*x + Sin(ph)*y + Cos(th)*Cos(ph)*z;
}

static void cone(double x,double y,double z,
                 double dx,double dy,double dz, double xp, double yp, double zp,
                 double th){


    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,xp,yp,zp);
    glScaled(dx,dy,dz);

    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    int angle;
    for(angle = 0; angle <=360; angle+=10){

        glVertex3d(0,-.5,0);
        glVertex3d(Cos(angle),-.5,Sin(angle));
        glVertex3d(Cos(angle+10),-.5,Sin(angle+10));

        glVertex3d(0,0.5,0);
        glVertex3d(Cos(angle),-.5,Sin(angle));
        glVertex3d(Cos(angle+10),-.5,Sin(angle+10));
    }
    glEnd();
    glPopMatrix();
}
//ex8
/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
    //   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
    glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}
static void circle(double x,double y,double z,double r, double dx, double dy, double dz,
  double red, double g, double b,double xp,  double th)
{
    const int d=5;
    int ph;


    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glRotated(th,xp,0,0);
    glScaled(dx*r,dy*r,dz*r);


    //  South pole cap
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(red,g,b);
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
static void ground(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th){
                   //  Save transformation
                   glPushMatrix();
                   //  Offset
                   glTranslated(x,y,z);
                   glRotated(th,0,1,0);
                   glScaled(dx,dy,dz);
                   //  Cube
                   glBegin(GL_QUADS);
                   glColor3f(0,1,1);
                   //square

                   glVertex3f(-1,+1,+1);
                   glVertex3f(+1,+1,+1);
                   glVertex3f(+1,+1,-1);
                   glVertex3f(-1,+1,-1);

                   //  End
                  glEnd();
                  //  Undo transformations
                  glPopMatrix();

                 }

void display()
{

    const double len=5;
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    //  Undo previous transformations
    glLoadIdentity();
    if(fp){
        Cx = +1.5*dim*Sin(fpangle);
        Cz = -1.5*dim*Cos(fpangle);
        gluLookAt(Ex,Ey,Ez,Cx*Ex,-.1+Ey,Cz+Ez,0,1,0);
        glWindowPos2i(5,20);
        //Print('First Person: %s Viewing Angle: %d',fp?"On":"Off", fpangle);
        Print("First Person: %s Viewing angle: %d, %d, %d",fp?"On":"Off", fpangle, Cx, Cz);

    }
    else {
        if(mode)//  Perspective - set eye position
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
    }
    ground(0,-2.7,0,5,0.1,5,0);
    //Draw Genric Space Needle
    cone(0,0,0,.5,5,.5,0,0,0,0); //x,y,z,dx,dy,dz,xp,yp,zp,th
    cone(0,3,0,.5,4,.5,1,0,0,180);
    circle(0,5 ,0 ,1, 2, 0.7 ,2 ,0.5,0.5,0.5,0,0); //x,y,z,r,dx,dy,dz,r,g,b,xp,th
    circle(0,5 ,0, 0.5 ,4.5 ,0.2 ,4.5 ,1,1,1,0,0);
    cone(0,6.2,0,.1,2,.1,0,0,0,0);
    circle(0,7,0, 0.3 ,0.3 ,0.3 ,0.3 ,1,0,0,0,0);

if(trans){
    //transformed Space Needles
    //1
    cone(-4,3,0,.2,3,.2,.5,0,0,90);
    cone(-4,3,2,.2,2,.2,.75,0,0,270);
    circle(-4,3,3, 0.7, 1.2,0.4,1.2, 0.5, 0.5,0.5,0.5,90);
    circle(-4,3,3, 0.3, 3,0.2,3, 1,1,1,0.5,90);
    cone(-4,3,3.4,.07,.7,.07,0.5,0,0,90);
    circle(-4,3,3.75,0.2,0.2,0.2,0.2, 1,0,0,0.5,90);

    //2
    cone(3,0,-2,.2,2.5,.2,0,0,0,0); //x,y,z,dx,dy,dz,xp,yp,zp,th
    cone(3,1.5,-2,.2,1.5,.2,1,0,0,180);
    circle(3,2.5 ,-2 ,0.7, 0.9, 0.4 ,0.9 ,0.5,0.5,0.5,0,0); //x,y,z,r,dx,dy,dz,r,g,b,xp,th
    circle(3,2.5 ,-2, 0.25 ,3 ,0.1 ,3 ,1,1,1,0,0);
    cone(3,2.9,-2,.02,0.35,.02,0,0,0,0);
    circle(3,3.1,-2, 0.18 ,0.18 ,0.18 ,0.18 ,1,0,0,0,0);
}    //axes
  glColor3f(1,1,1);
    if (axes)
    {
        glColor3f(0,0,1);
        glBegin(GL_LINES);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(len,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,len+2.5,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,len);
        glEnd();
        //  Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len+2.5,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }
      glColor3f(1,1,1);
    //  Display parameters
    glWindowPos2i(5,5);

    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
    //  Render the scene and make it visible
    glFlush();
    glutSwapBuffers();

}

//key pressed
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
    //  Reset view angle
    else if (ch == '0')
        th = ph = 0;
    //toggle axes
    else if (ch == '1')
      axes = 1-axes;
    //first person navigatoin
    else if (ch == 'f' || ch == 'F')
        fp = 1-fp;
    if(fp){
        if(ch == 'w'|| ch =='W'){ //forwars
            Ex += Cx*.03;
            Ez += Cz*.03;
        }
        else if (ch == 'a'|| ch =='A'){
            fpangle -= 5;
        }
        else if(ch == 's'|| ch =='S'){ //back
            Ex -= Cx*.03;
            Ez -= Cz*.03;
        }
        else if(ch == 'd'|| ch =='D'){
            fpangle += 5;
        }
        else if (ch =='q' || ch =='Q'){ //reset fp view
          Ex = 0; //Eye pos
          Ey = 0;
          Ez = 15;
          Cx = 0; //camera pos
          Cz = 25;
          fpangle =0;
        }
        fpangle = fpangle%360;
    }
    //  Switch display mode
    else if (ch == 'm')
        mode = (1-mode);
    else if (ch == 'M')
        mode = (1-mode);
    else if (ch == 't')
        trans = 1- trans;

    //  Tell GLUT it is necessary to redisplay the scene
    Project();
    glutPostRedisplay();
}
//arrow keys from ex6
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


int main(int argc,char* argv[])
{
    //  Initialize GLUT and process user parameters
    glutInit(&argc,argv);
    //  Request double buffered, true color window, Z-buffering
    glutInitDisplayMode(GLUT_RGB |GLUT_DEPTH | GLUT_DOUBLE);
    //  Request 500 x 500 pixel window
    glutInitWindowSize(600,600);
    //  Create the window
    glutCreateWindow("Alison Ostlund_HW2");
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
