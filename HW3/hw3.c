//Alison Ostlund, CSCI4229, HW3 - Seattle Space Needle
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "CSCIx229.h"
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
// Lighting
int light     =   1;      //  Lighting
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
float move    =   1; //movement of the light

//Texture
unsigned int texture[1]; // Textures
int textmode = 1;



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


static void cone(double x,double y,double z,
                 double dx,double dy,double dz, double xp, double yp, double zp,
                 double th, int tex)
{
   /*Set up material color*/
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

   const int w = 1;
   double rad = .5;
   float angle = 0;
   float inc = 360/w;

   //Save Transformation
   glPushMatrix();
   //Transformations
   glTranslated(x,y,z);
   glRotated(th, xp, yp, zp);
   glScaled(dx,dy,dz);
   glColor3f(1,1,1);

   if(textmode){
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, texture[tex]);
   }
   else{
      glDisable(GL_TEXTURE_2D);
   }


   glBegin(GL_TRIANGLES);

   for (angle=0;angle<360;angle+=w){
      //bottom of cone
      glNormal3f(0,-1,0);
      glTexCoord2f(0,0); glVertex3d(0,-.5*dy,0);
      glTexCoord2f(0,0); glVertex3d(rad*Sin(angle+w), -.5*dy, rad*Cos(angle+w));
      glTexCoord2f(0,0); glVertex3d(rad*Sin(angle),   -.5*dy, rad*Cos(angle));
   }
   glEnd();


   glBindTexture(GL_TEXTURE_2D, texture[tex]);
   //sides
   glBegin(GL_QUADS);
   for(angle=0;angle<360;angle+=w){

      glNormal3f(rad*Sin(angle), 0, rad*Cos(angle));
      glTexCoord2f(angle/w/inc, 0); glVertex3d(rad*Sin(angle), -.5*dy,rad*Cos(angle));//LL

      glNormal3f(rad*Sin(angle+w), 0, rad*Cos(angle+w));
      glTexCoord2f((angle+w)/w/inc, 0); glVertex3d(rad*Sin(angle+w), -.5*dy,rad*Cos(angle+w));//LR

      glNormal3f(rad*Sin(angle+w), 0, rad*Cos(angle+w));
      glTexCoord2f((angle+w)/w/inc ,1); glVertex3d(0, .5*dy,0);//UR

      glNormal3f(rad*Sin(angle), 0, rad*Cos(angle));
      glTexCoord2f(angle/w/inc, 1); glVertex3d(0, .5*dy,0);//UL
   }
   glEnd();

   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
//ex8
/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
    //   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
  //  glNormal3f(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
  double x = -Sin(th)*Cos(ph);
  double y =  Cos(th)*Cos(ph);
  double z =          Sin(ph);
  glNormal3d(x,y,z);
  glTexCoord2d(th/360.0,ph/180.0+0.5);
  glVertex3d(x,y,z);
  //  glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));

}
static void circle(double x,double y,double z,double r, double dx, double dy, double dz,
  double red, double g, double b,double xp,  double th, int tex)
{
    const int d=5;
    int ph;
    float white[]={0,1,1,1};
    float black[] ={0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    //  Save transformation
    glPushMatrix();
    //texture
    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[tex]);


      }
      else
        glDisable(GL_TEXTURE_2D);
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
    glDisable(GL_TEXTURE_2D);
    //  Undo transformations
    glPopMatrix();
}
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glutSolidSphere(1.0,16,16);
   //  Undo transofrmations
   glPopMatrix();
}

static void ground(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th){
                   float white[]={1,1,1,1};
                   float black[] ={0,0,0,1};
                   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
                   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
                   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
                   //  Save transformation
                   glPushMatrix();
                   if(textmode){
                       glEnable(GL_TEXTURE_2D);
                       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
                       glBindTexture(GL_TEXTURE_2D,texture[2]);


                     }
                     else
                       glDisable(GL_TEXTURE_2D);
                   //  Offset
                   glTranslated(x,y,z);
                   glRotated(th,0,1,0);
                   glScaled(dx,dy,dz);
                   //  Cube
                   glBegin(GL_QUADS);
                   glColor3f(0,1,1);
                   //square
                   glNormal3f(0,1,0);

                   glTexCoord2d(0,0);glVertex3f(-1,+1,+1);
                   glTexCoord2d(1,0);glVertex3f(+1,+1,+1);
                   glTexCoord2d(1,1);glVertex3f(+1,+1,-1);
                   glTexCoord2d(0,1);glVertex3f(-1,+1,-1);


                   //  End
                  glEnd();
                   glDisable(GL_TEXTURE_2D);
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
    //  Light switch
  if (light)
  {
     //  Translate intensity to color vectors
     float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
     float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
     float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
     //  Light direction
     float Position[]  = {5*Cos(zh),ylight,5*Sin(zh),1};
     //  Draw light position as ball (still no lighting here)
     glColor3f(1,1,1);
     ball(Position[0],Position[1],Position[2] , 0.1);
     //  OpenGL should normalize normal vectors
     glEnable(GL_NORMALIZE);
     //  Enable lighting
     glEnable(GL_LIGHTING);
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
  }
  else
     glDisable(GL_LIGHTING);

   if(move){
     //  Elapsed time in seconds
     double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
     zh = fmod(90*t,360.0);
     //  Tell GLUT it is necessary to redisplay the scene
     glutPostRedisplay();

     }

    ground(0,-2.7,0,5,0.1,5,0);
    //Draw Genric Space Needle
    cone(0,0,0,1,2.2,1,0,0,0,0,0); //x,y,z,dx,dy,dz,xp,yp,zp,th
    cone(0,3,0,1,2,1,1,0,0,180,0);

    circle(0,5 ,0 ,1, 2, 0.7 ,2 ,1,1,1,0,0,5); //x,y,z,r,dx,dy,dz,r,g,b,xp,th,tex
    circle(0,5 ,0, 0.5 ,4.5 ,0.2 ,4.5 ,1,1,1,0,0,3);
    circle(0,5.09 ,0, 0.5 ,4 ,0.5 ,4 ,1,1,1,0,0,1); //window
    circle(0,4.9 ,0, 0.5 ,4 ,0.7 ,4 ,1,1,1,0,0,1);  //window
    circle(0,4.4,0, 0.4 ,3.3,0.4 ,3.3,1,1,1,0,0,3); //bottom circle
    cone(0,6.2,0,.1,1.5,.1,0,0,0,0,3);//needle
    circle(0,6.7,0, 0.3 ,0.3 ,0.3 ,0.3 ,1,0,0,0,0,6); //light


   //axes
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

    Print("Angle=%d,%d  Dim=%.1f  Light =%s FOV=%d Projection=%s",th,ph,dim,light?"On":"Off",fov,mode?"Perpective":"Orthogonal");
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
        //  Light elevation
     else if (ch=='[')
        ylight -= 0.1;
     else if (ch==']')
        ylight += 0.1;
        //toggle light
      else if (ch =='l')
           light = 1- light;
     else if (ch == 's')
          move = 1-move;




    //  Tell GLUT it is necessary to redisplay the scene
    Project(mode?fov:0,asp,dim);
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
    Project(mode?fov:0,asp,dim);

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
    Project(mode?fov:0,asp,dim);

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
    glutCreateWindow("Alison Ostlund_HW3");
    //  Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    //  Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    //  Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    //  Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    texture[0] = LoadTexBMP("tower.bmp");
    texture[1]= LoadTexBMP("window.bmp");
    texture[2] = LoadTexBMP("grass.bmp");
    texture[3] = LoadTexBMP("top.bmp");
    texture[4] = LoadTexBMP("steel.bmp");
    texture[5] = LoadTexBMP("bottom.bmp");
    texture[6] = LoadTexBMP("light.bmp");

    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    //  Return code
    return 0;
}
