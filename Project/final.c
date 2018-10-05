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
double dim= 10.0;   //  Size of world
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
int ambient   =   60;  // Ambient intensity (%)
int diffuse   =  100;  // Diffuse intensity (%)
int specular  =   30;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
float move    =   1; //movement of the light
///move ferry boat
float f1 =7;
float f2 =6;
float f3 =8;


//Texture
unsigned int texture[32]; // Textures
unsigned int sky[5];
int textmode = 1;

//DaynNight
int day = 1;
int night = 0;



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
//----------------------------------------------------------------------------------

static void cone(double x,double y,double z,
                 double dx,double dy,double dz, double xp, double yp, double zp,
                 double th, int tex, int ntex)
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
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[tex]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[ntex]);
    }
    else
        glDisable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);

    for (angle=0;angle<360;angle+=w){
        //bottom of cone
        glNormal3f(0,-1,0);
        glTexCoord2f(0,0); glVertex3d(0,-.5*dy,0);
        glTexCoord2f(0,0); glVertex3d(rad*Sin(angle+w), -.5*dy, rad*Cos(angle+w));
        glTexCoord2f(0,0); glVertex3d(rad*Sin(angle),   -.5*dy, rad*Cos(angle));
    }
    glEnd();


    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[tex]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[ntex]);
    }
    else
        glDisable(GL_TEXTURE_2D);
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

//--------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------
//ex8
static void building(double x,double y,double z,
                     double dx,double dy,double dz, double zp,
                     double th, int tex, int tex2, int daytex, int daytex2)
{
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
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[daytex]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[tex]);
    }
    else
        glDisable(GL_TEXTURE_2D);
    //  Offset
    glTranslated(x,y,z);
    glRotated(th,0,1,zp);
    glScaled(dx,dy,dz);
    //  Cube
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    //  Front
    glNormal3d(0,0,1);
    glTexCoord2f(1,0);glVertex3f(-1,-1, 1);
    glTexCoord2f(0,0);glVertex3f(+1,-1, 1);
    glTexCoord2f(0,1);glVertex3f(+1,+1, 1);
    glTexCoord2f(1,1);glVertex3f(-1,+1, 1);
    //  Back
    glNormal3d(0,0,-1);
    glTexCoord2f(1,0);glVertex3f(+1,-1,-1);
    glTexCoord2f(0,0);glVertex3f(-1,-1,-1);
    glTexCoord2f(0,1);glVertex3f(-1,+1,-1);
    glTexCoord2f(1,1);glVertex3f(+1,+1,-1);
    //  Right
    glNormal3d(1,0,0);
    glTexCoord2f(1,0);glVertex3f(+1,-1,+1);
    glTexCoord2f(0,0);glVertex3f(+1,-1,-1);
    glTexCoord2f(0,1);glVertex3f(+1,+1,-1);
    glTexCoord2f(1,1);glVertex3f(+1,+1,+1);
    //  Left
    glNormal3d(-1,0,0);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,1);glVertex3f(-1,+1,-1);

    glDisable(GL_TEXTURE_2D);
    glEnd();
    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[daytex2]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[tex2]);
    }
    else
        glDisable(GL_TEXTURE_2D);


    glBegin(GL_QUADS);

    //  Top
    glNormal3d(0,1,0);
    glColor3f(1,1,1);
    glTexCoord2f(1,0);glVertex3f(-1,+1,+1);
    glTexCoord2f(0,0);glVertex3f(+1,+1,+1);
    glTexCoord2f(0,1);glVertex3f(+1,+1,-1);
    glTexCoord2f(1,1);glVertex3f(-1,+1,-1);
    //  Bottom
    glNormal3d(0,-1,0);
    glTexCoord2f(1,0);glVertex3f(-1,-1,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,1);glVertex3f(-1,-1,+1);
    //  End

    glEnd();
    glDisable(GL_TEXTURE_2D);
    //  Undo transformations
    glPopMatrix();
}
//----------------------------------------------------------------------
void rightTri(double x,double y,double z,
              double dx,double dy,double dz, double xp, double yp,
              double th,double ph, int tex, int daytex)
{
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,yp,0);
    glRotated(ph,xp,0,0);
    glScaled(dx,dy,dz);
    float white[]={0,1,1,1};
    float black[] ={0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    //  Save transformation

    //texture
    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[daytex]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[tex]);
    }
    else
        glDisable(GL_TEXTURE_2D);


    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3d(0,-1,0);
    glTexCoord2f(1,1);glVertex3f(0.5, 0, 0.5);
    glTexCoord2f(1,0);glVertex3f(0.5, 0, -0.5);
    glTexCoord2f(0,0);glVertex3f(-0.5, 0, -0.5);
    glTexCoord2f(0,1);glVertex3f(-0.5, 0, 0.5);

    glNormal3d(0,0,-1);
    glTexCoord2f(0,0);glVertex3f(0.5,0,-0.5);
    glTexCoord2f(0,1);glVertex3f(0.5,1,-0.5);
    glTexCoord2f(1,1);glVertex3f(-0.5,1,-0.5);
    glTexCoord2f(1,0);glVertex3f(-0.5,0,-0.5);

    glNormal3d(0,1,1);
    glTexCoord2f(1,1);glVertex3f(0.5,1,-0.5);
    glTexCoord2f(0,1);glVertex3f(-0.5,1,-0.5);
    glTexCoord2f(0,0);glVertex3f(-0.5,0,0.5);
    glTexCoord2f(1,0);glVertex3f(0.5,0,0.5);
    glEnd();
    glBegin(GL_TRIANGLES);

    glNormal3d(1,0,0);
    glTexCoord2f(1,0);glVertex3f(0.5,0,0.5);
    glTexCoord2f(0,1);glVertex3f(0.5,1,-0.5);
    glTexCoord2f(0,0); glVertex3f(0.5,0,-0.5);

    glNormal3d(-1,0,0);
    glTexCoord2f(1,0);glVertex3f(-0.5,0,0.5);
    glTexCoord2f(0,1);glVertex3f(-0.5,1,-0.5);
    glTexCoord2f(0,0); glVertex3f(-0.5,0,-0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//---------------------------------------------------------------

static void ground(double x,double y,double z,
                   double dx,double dy,double dz,
                   double th, int tex,int tex2){
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
        if(day){
            glBindTexture(GL_TEXTURE_2D,texture[tex]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,texture[tex2]);
    }
    else
        glDisable(GL_TEXTURE_2D);


    //  Offset
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    //  Cube
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
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
//----------------------------------------------------------------------
/*
 *  Draw sky box
 */
 //ex25
static void Sky(double D, int day1, int night1, int day2, int night2)
{
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    //texture
    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(day){
            glBindTexture(GL_TEXTURE_2D,sky[day1]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,sky[night1]);
    }
    else
        glDisable(GL_TEXTURE_2D);
    //  Sides

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
    if(textmode){
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

        if(day){
            glBindTexture(GL_TEXTURE_2D,sky[day2]);
        }
        else
            glBindTexture(GL_TEXTURE_2D,sky[night2]);
    }
    else
        glDisable(GL_TEXTURE_2D);

    //  Top and bottom

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

    glDisable(GL_TEXTURE_2D);
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
        float Position[]  = {10*Cos(zh),ylight,10*Sin(zh),1};
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
    //ground-------------------------------------------------------------
    int i = 0;
    int j = 0;
    for (i = -10; i <10; i++){
        for(j =-10; j<2; j++)
            ground(i,-3.7,j,0.5,1,0.5,0,22,22);
    }
    //water
    for (i = -10; i <10; i++){
        for(j =2; j<5; j++)
            ground(i,-3.7,j,0.5,1,0.5,0,23,30);
    }
//---------------------------------- DRAWING THE SCENE --------------------------------------------------
    //Space Needle
    //body
    cone(0,0,0,1,2.2,1,0,0,0,0,0,0); //x,y,z,dx,dy,dz,xp,yp,zp,th
    cone(0,3,0,1,2,1,1,0,0,180,0,0);
    //top
    circle(0,5 ,0 ,1, 2, 0.7 ,2 ,1,1,1,0,0,3); //x,y,z,r,dx,dy,dz,r,g,b,xp,th,tex
    //ring
    circle(0,5 ,0, 0.5 ,4.5 ,0.2 ,4.5 ,1,1,1,0,0,4);
    circle(0,5.09 ,0, 0.5 ,4 ,0.8 ,4 ,1,1,1,0,0,1); //topwindow
    circle(0,4.9 ,0, 0.5 ,4 ,1.02 ,4 ,1,1,1,0,0,1);  //bottomwindow
    circle(0,4.4,0, 0.4 ,3.3,0.4 ,3.3,1,1,1,0,0,4); //bottom circle
    //top
    cone(0,5.4,0,1.2,1,1.2,1,0,0,180,3,3);
    cone(0,6.5,0,.08,1.6,.08,0,0,0,0,3,3);//needle
    circle(0,6.8,0, 0.3 ,0.3 ,0.3 ,0.3 ,1,0,0,0,0,2); //light

    //Buildings for city ------------------------------------------------------------
    // x,y,z,dx,dy,dz,zp,th, tex,tex2,daytex, daytex2
    //left side
    building(-2.5,-1.5,-1,1.5,1,1,0,0,19,22,8,21); //front left
    building(-5,-1.4,-2,0.7,1.1,0.7,0,0,20,22,12,4);
    building(-5,-0.2,-2,0.5,0.2,0.5,0,0,20,22,12,4);
    building(-0.5,-1,-3,0.4,1.5,0.4,0,0,19,22,9,21);
    building(-0.3,-1,-5,1,1.2,1,0,0,20,22,8,21);
    building(-1,-0.4,-8,.7,2.2,.7,0,0,7,22,18,4);

    building(-8,-0.2,-8,0.9,2.4,.7,0,0,19,22,14,4);
    building(-8,2.2,-8,0.7,0.2,0.6,1,-10,4,4,4,4); //top of ^

    building(-6.5,-1,-3,0.6,1.5,0.5,0,0,20,22,6,21);
    building(-6,-0.8,-6,0.9,1.7,0.7,0,0,20,22,9,21);
    building(-7.5,-1,-4,0.4,1.3,0.4,0,0,7,22,12,21);
    building(-9,-1.6,-2,0.7,1,0.7,0,0,19,22,11,4);
    building(-5,-0.8,-4,0.3,1.8,0.3,0,0,7,22,6,21);
    building(-2.5,-0.8,-6,0.9,1.7,0.7,0,0,19,22,9,21);
    //triple layer
    building(-4,-1.15,-8,1,1.5,1,0,0,20,22,15,4);
    building(-4,0.7,-8,0.8,0.3,0.8,0,0,20,22,15,4);
    building(-4,1,-8,0.5,0.5,0.5,0,0,20,22,15,4);

    building(-9,-1.4,-5,0.5,1,0.5,0,0,7,22,16,21);



    //right side
    //colombia tower
    building(3,0.5,-9,.5,3,.5,0,0,19,19,11,11);
    building(2.7,-0.4,-9,.5,2.2,.5,0,0,19,19,11,11);
    building(4.2,-1,-7,1,1.5,1,0,0,7,22,6,4);
    building(2,-1.2,-5.5,1,1.2,1.5,0,0,7,22,9,21);
    building(1.5,-1.2,-2.6,1,1,1,0,0,20,22,10,21);
    rightTri(1.5,-0.2,-2.6,2,1.5,2,0,1,90,0,20,10);

    building(6,-.7,-4,0.8,1.8,0.8,0,0,7,22,14,14);
    rightTri(5.65,1.3,-4,1,1,1,1,1,90,135,7,14);
    building(2.1,-1.2,-0.8,0.5,1.2,0.5,0,0,20,22,13,21);
    building(5,-1.1,-1,1,1.4,0.5,0,0,19,22,16,4);
    building(5,0.5,-1,0.8,0.2,0.5,0,0,19,22,16,4);
    building(4,-1.2,-2,0.5,1.2,0.5,0,0,7,22,12,21);
    building(4.3,-1,-4,0.5,1.4,0.5,0,0,20,22,13,21);
    building(6.8,-1.4,-0.5,0.5,1,0.5,0,0,7,22,9,21);
    building(8,-1.4,-6,1,1,1,0,0,20,22,13,21);

    //sky----------------------------------------------
    Sky(30,2,0,3,1);

    // -----trees----------------------------------------
    cone(8,-2,-0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(9,-2,-0.7,0.4,1,0.4,0,0,0,0,24,25);
    cone(7,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(8,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.2,-2,-2,0.4,1,0.4,0,0,0,0,24,25);
    cone(9.2,-2,1,0.4,1,0.4,0,0,0,0,24,25);
    cone(9,-2,-2,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.5,-2,-3,0.4,1,0.4,0,0,0,0,24,25);
    cone(7.5,-2,-3,0.4,1,0.4,0,0,0,0,24,25);
    cone(9,-2,-2,0.4,1,0.4,0,0,0,0,24,25);
    cone(9.1,-2,0,0.4,1,0.4,0,0,0,0,24,25);
    cone(7,-2,-4,0.4,1,0.4,0,0,0,0,24,25);
    cone(6.8,-2,-2,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.6,-2,0.8,0.4,1,0.4,0,0,0,0,24,25);
    cone(9.1,-2,-4.6,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.5,-2,-4.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(8,-2,-4,0.4,1,0.4,0,0,0,0,24,25);
    cone(5,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(6,-2,0.1,0.4,1,0.4,0,0,0,0,24,25);
    cone(4 ,-2,0,0.4,1,0.4,0,0,0,0,24,25);
    cone(5.5,-2,0.8,0.4,1,0.4,0,0,0,0,24,25);
    cone(3,-2,0.8,0.4,1,0.4,0,0,0,0,24,25);
    cone(2.8,-2,0,0.4,1,0.4,0,0,0,0,24,25);
    cone(2,-2,0.3,0.4,1,0.4,0,0,0,0,24,25);
    cone(1.5,-2,0.9,0.4,1,0.4,0,0,0,0,24,25);
    cone(9.1,-2,-9,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.5,-2,-9.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.5,-2,-8.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(7,-2,-8,0.4,1,0.4,0,0,0,0,24,25);
    cone(8,-2,-9,0.4,1,0.4,0,0,0,0,24,25);
    cone(8.8,-2,-8,0.4,1,0.4,0,0,0,0,24,25);
    cone(7,-2,-9,0.4,1,0.4,0,0,0,0,24,25);
    //left side trees
    cone(-1.5,-2,0.9,0.4,1,0.4,0,0,0,0,24,25);
    cone(-2,-2,0.3,0.4,1,0.4,0,0,0,0,24,25);
    cone(-2.8,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-4,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-5,-2,1,0.4,1,0.4,0,0,0,0,24,25);
    cone(-5.5,-2,0,0.4,1,0.4,0,0,0,0,24,25);
    cone(-6,-2,-0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-7,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-8,-2,0.2,0.4,1,0.4,0,0,0,0,24,25);
    cone(-7,-2,-2,0.4,1,0.4,0,0,0,0,24,25);
    cone(-7.2,-2,-0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-6.2,-2,-1.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-7.8,-2,-1,0.4,1,0.4,0,0,0,0,24,25);
    cone(-8.5,-2,0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-9.5,-2,0,0.4,1,0.4,0,0,0,0,24,25);
    cone(-9,-2,-0.5,0.4,1,0.4,0,0,0,0,24,25);
    cone(-10,-2,0.8,0.4,1,0.4,0,0,0,0,24,25);
//ferry boat__________________________________________________________

rightTri(f1,-2.2,3,3,0.8,0.8,1,1,180,315,28,27);
// x,y,z,dx,dy,dz,zp,th, tex,tex2,daytex, daytex2
building(f1,-1.9,3.2,1.5,0.09,0.6,0,0,20,29,3,3);
building(f2,-1.8,3.25,0.2,0.3,0.3,0,0,31,31,26,26);
building(f1,-1.7,3.25,0.3,0.2,0.4,0,0,31,31,26,26);
building(f3,-1.8,3.25,0.2,0.3,0.3,0,0,31,31,26,26);

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

    Print("Angle=%d,%d Light =%s Day =%s FOV=%d Projection=%s",th,ph,light?"On":"Off",day?"On":"Off",fov,mode?"Perpective":"Orthogonal");
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
    if (ch == 'm')
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
        //stop light
    else if (ch == 's')
        move = 1-move;
        //day amd night mode
    else if (ch == 'n' || ch  == 'N')
    {
        day =  1- day;
        if (day){
            ambient = 60;
            diffuse = 80;
        }
        else{
            ambient = 20;
            diffuse = 30;

        }

    }
    //move ferry
    else if (ch == '<'){
        f1 -= 0.1;
        f2 -= 0.1;
        f3 -= 0.1;
    }
    else if (ch == '>'){
        f1 += 0.1;
        f2 += 0.1;
        f3 += 0.1;
    }




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
    glutCreateWindow("Alison Ostlund_Project");
    //  Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    //  Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    //  Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    //  Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);

    //space needle
    texture[0] = LoadTexBMP("tower.bmp");
    texture[1]= LoadTexBMP("window.bmp");
    texture[2] = LoadTexBMP("light.bmp");
    texture[3] = LoadTexBMP("white.bmp");
    texture[4] = LoadTexBMP("steel.bmp");
    texture[5] = LoadTexBMP("bottom.bmp");

    //city
    texture[6] = LoadTexBMP("b1.bmp");
    texture[7] = LoadTexBMP("city.bmp");
    texture[8] = LoadTexBMP("b9.bmp");
    texture[9] = LoadTexBMP("b2.bmp");
    texture[10] = LoadTexBMP("black.bmp");
    texture[11] = LoadTexBMP("columbia.bmp");
    texture[12] = LoadTexBMP("b4.bmp");
    texture[13] = LoadTexBMP("b3.bmp");
    texture[14] = LoadTexBMP("idk.bmp");
    texture[15] = LoadTexBMP("b5.bmp");
    texture[16] = LoadTexBMP("b8.bmp");
    texture[17] = LoadTexBMP("b7.bmp");
    texture[18] = LoadTexBMP("b12.bmp");

    //night time
    texture[19] = LoadTexBMP("n3.bmp");
    texture[20] = LoadTexBMP("n4.bmp");

    //building tops
    texture[21] = LoadTexBMP("concrete.bmp");
    texture[22] = LoadTexBMP("nitecon.bmp");

    //nature and ferry
    texture[23] = LoadTexBMP("water.bmp");
    texture[24] = LoadTexBMP("tree.bmp");
    texture[25] = LoadTexBMP("ntree.bmp");
    texture[26] = LoadTexBMP("green.bmp");
    texture[27] = LoadTexBMP("f1.bmp");
    texture[28] = LoadTexBMP("nf2.bmp");
    texture[29] = LoadTexBMP("nf1.bmp");
    texture[30] = LoadTexBMP("nwater.bmp");
    texture[31] = LoadTexBMP("ngreen.bmp");

    //skybox
    sky[0] = LoadTexBMP("nightsky.bmp");
    sky[1] = LoadTexBMP("nightsky.bmp");
    sky[2] = LoadTexBMP("skyday1.bmp");
    sky[3] = LoadTexBMP("skyday2.bmp");


 ErrCheck("init");

    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    //  Return code
    return 0;
}
