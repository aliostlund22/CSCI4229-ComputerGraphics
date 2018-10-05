//Alison Ostlund, CSCI4229, HW1
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

//ex7
//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

int th=0;        // Azimuth of view angle
int ph=0;       // Elevation of view angle
double dim=20; // Dimension of orthogonal box
double len=8; // length of axes
int red = 0; //color change
int green =0;
int blue = 1;
//lorenz Paramters
double s  = 10;
double b  = 2.6666;
double r  = 28;

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
// Z comp projection - ex7
double Zp(double x,double y,double z){
    return -Sin(th)*Cos(ph)*x + Sin(ph)*y + Cos(th)*Cos(ph)*z;
}

 // display axes and lorenz
void display()
{
    //  Clear the image and z buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Reset previous transforms
    glLoadIdentity();
    //  Set view angle
    glRotated(ph,1,0,0);
    glRotated(th,0,1,0);

    glEnable(GL_DEPTH_TEST);

    //lorenz attractor
    int i;
    /*  Coordinates  */
    double x = 1;
    double y = 1;
    double z = 1;
    /*  Time step  */
    double dt = 0.001;

    double dx = 0;
    double dy = 0;
    double dz = 0;

    glColor3f(0,1,0);
    glBegin(GL_LINES);

    //glVertex3d(x,y,z);

    for (i=0;i<50000;i++){
        dx = s*(y-x);
        dy = x*(r-z)-y;
        dz = x*y - b*z;
        x += dt*dx;
        y += dt*dy;
        z += dt*dz;
        glColor3f(red,green,blue);
        glVertex3d(x/3, y/3, z/3);
    }

    glEnd();

   //create axes taken from ex6
    //  Draw axes in white
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(len,0,0);
    glVertex3d(0,0,0);
    glVertex3d(0,len,0);
    glVertex3d(0,0,0);
    glVertex3d(0,0,len);
    glEnd();
    //  Label axes
    glRasterPos3d(len,0,0);
    Print("X");
    glRasterPos3d(0,len,0);
    Print("Y");
    glRasterPos3d(0,0,len);
    Print("Z");

    // Print and dislay the text
    glWindowPos2i(2,2);
    Print("B: %.3f S: %.3f  R: %.3f",b, s, r);

    //  Flush and swap
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
    // Reset parameters
    else if (ch == '1'){
        s  = 10;
        b  = 2.6666;
        r  = 28;
    }
    // increase s
    else if (ch == 's')
        s = s + 0.5;
    // decrease s
    else if (ch == 'S')
        s = s - 0.5;
    // increase r
    else if (ch == 'r')
        r = r + 0.5;
    // decrease r
    else if (ch == 'R')
        r = r - 0.5;
    // increase b
    else if (ch == 'b')
        b = b + 0.5;
    // decrease b
    else if (ch == 'B')
        b = b - 0.5;
    // change color to red
    else if (ch == '2'){
        red = 1;
        green = 0;
        blue = 0;
    }

    // change color to green
    else if (ch == '3'){
        red = 0;
        green =1;
        blue = 0;
    }
    // change color to blue
    else if (ch == '4'){
        red = 0;
        green = 0;
        blue = 1;
    }


    // redisplay the scene
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
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

//window reshape used ex6
void reshape(int width,int height)
{

    //  Ratio of the width to the height of the window
    double w2h = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Orthogonal projection box adjusted for the
    //  aspect ratio of the window
    glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}



int main(int argc,char* argv[])
{
    //  Initialize GLUT and process user parameters
    glutInit(&argc,argv);
    //  Request double buffered, true color window, Z-buffering
    glutInitDisplayMode(GLUT_RGB |GLUT_DEPTH | GLUT_DOUBLE);
    //  Request 500 x 500 pixel window
    glutInitWindowSize(500,500);
    //  Create the window
    glutCreateWindow("Alison Ostlund_HW1");
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
