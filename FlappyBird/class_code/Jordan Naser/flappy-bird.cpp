#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace std;

//======================================================
// CONSTANTS 
//======================================================
// Square size
#define size 50.0

//======================================================
// GLOBAL VARIABLES WRITTEN TO BY reshapeCallBack( )
//======================================================
// Window size
int w_height = 800;
int w_width = 800;

//======================================================
// GLOBAL VARIABLES WRITTEN TO BY displayCallBack( )
//======================================================
int display_count = 0; //keeps count of invocations

//======================================================
// GLOBAL VARIABLES WRITTEN TO BY keyboardCallBack( )
//======================================================
float theta = 0.0; // Rotation angle of bird 
int timer_period_in_ms = 100; // time between timer callbacks in milliseconds

//====================================================================
// GLOBAL VARIABLES WRITTEN TO BY motionCallBack( )
//====================================================================
int bird_r = 40;
int Eye_radius = 20; // For Flappy's eye 
float Eye_pupil = 10; // For Flappy's pupil
float bird_x = 40;
float bird_y = 40;; // position of bird
double bird_vx = 0;
double bird_vy = 0;
double bird_ax = 0;
double bird_ay = 0;
int bird_dx = 1; // incremental change inbird_x
float dt = 0.15; //0.05;
double impulse = 0;
double impulseMax = 80;
double vyMax = 40;
double vxMax = 40;

double obj_x = 1000;
double obj_y = 200;

bool hasStarted = false;
int numUpdates = 0;
//======================================================
// IDLE CALLBACK ROUTINE 
//======================================================
void bounce() {
    //bird_ax += 0;
    //bird_ay += 20;
    impulse += impulseMax;
    bird_ax += 2;
}

void boundVelocityAcceleration(double& vx, double& vy, double& ax, double& ay) {
    if (vx > vxMax) vx = vxMax;
    if (vy > vyMax) vy = vyMax;
    if (ax > 20) ax = 20;
    if (ay < -10) ay = -9;
}

void boundImpulse() {
    if (impulse > impulseMax) impulse = 33; 
    if (impulse < 0.5) impulse = 0;
}

void update()
{
    numUpdates++;
    if (numUpdates % 100 == 0) cout << "Num updates = " << numUpdates << "bird_x = " << bird_x << " bird_y = " << bird_y << " vx = " << bird_vx << " vy = " << bird_vy << " ax = " << bird_ax << " ay = " << bird_ay << endl;
    if (hasStarted) {
        //acceleration gets updated
        bird_ay += (-9.8 + impulse) * dt;
        //velocity gets updated
        bird_vx += bird_ax * dt;
        bird_vy += bird_ay * dt;
        //position gets updated
        bird_x += bird_vx * dt;
        bird_y += bird_vy * dt;
        boundVelocityAcceleration(bird_vx, bird_vy, bird_ax, bird_ay);
        impulse *= 0.3;
        boundImpulse();
    }
    /*
    bird_x += bird_dx; //Increment x-position of bird
    if (bird_x > w_width || bird_x == 0) bird_dx *= -1; //Reverse direction if at edges
      */
    if (bird_y - bird_r < 0) {
        bird_y = bird_r;
        bird_vy = 0;
        bird_ay = 0;
    }

    if (obj_x < (bird_x - w_width / 2.0)) {
        //random x
        obj_x = bird_x + w_width / 2.0 + (100.0 * rand()) / RAND_MAX;
        //random y
        obj_y = (rand()) % w_height;
    }

    glutPostRedisplay();
}

//======================================================
// RESHAPE CALLBACK ROUTINE
//======================================================
void reshapeCallback(int width, int height)
{
    //Update globals containing window size
    w_width = width;
    w_height = height;
}

//======================================================
// MOUSE MOTION CALLBACK ROUTINE 
//======================================================
void motionCallBack(int x, int y)
{
    printf("YYYMotion call back: %d, %d)\n", x, y);
    //Set bird's location to current mouse position
    bird_x = x;
    bird_y = w_height - y; //Invert mouse y (as its measured from top)

    glutPostRedisplay();
}

//======================================================
// MOUSE CALLBACK ROUTINE 
//======================================================
void mouseCallBack(int btn, int state, int x, int y)
{
    printf("Mouse call back: button=%d, state=%d, x=%d, y=%d\n", btn, state, x, y);
    if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)   exit(0);
}

//======================================================
// KEYBOARD CALLBACK ROUTINE 
//======================================================
bool isSimulating = false;
void keyboardCallBack(unsigned char key, int x, int y)
{
    printf("Keyboard call back: key=%c, x=%d, y=%d, theta=%f\n", key, x, y, theta);
    switch (key)
    {
    case 'i':
    case 'I':
        if (!isSimulating) {
            isSimulating = !isSimulating;
            printf("Idle function ON\n");
            break;
        }
        else {
            isSimulating = !isSimulating;
            glutIdleFunc(NULL);
            printf("Idle function OFF\n");
            break;
        }
        //case 'r':
        //  theta = theta + 10.0;
        //  break;
    case ' ':
        hasStarted = true;
        isSimulating = true;
        bounce();
        break;
    case 27:
    case 'q':
        cout << "Exiting" << endl;
        exit(-1);
    default:
        printf("Press i (Idle Off), I (Idle ON) or r (Rotate)");
    }

    glutPostRedisplay();
}

void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = 2 * 3.1415926 / float(num_segments);
    float c = cos(theta);//precalculate the sine and cosine
    float s = sin(theta);
    float t;

    float x = 10;//we start at angle = 0 
    float y = 0;
    
    glBegin(GL_POLYGON); //Changed "GL_LINE_LOOP" TO "GL_POLYGON" to fill in the ellipse for the body of the bird 
    for (int ii = 0; ii < num_segments; ii++)
    {
        //apply radius and offset
        glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void DrawCircle(float r, int num_segments) {
    float theta = 2 * 3.1415926 / float(num_segments);
    glBegin(GL_POLYGON);
    for (int ii = 0; ii < num_segments; ii++) {
        float xi = r * cos((theta * ii));
        float yi = r * sin((theta * ii));
        glVertex2f(xi, yi);
    }
    glEnd();
}

void Draw_Wings() {
    glBegin(GL_POLYGON);
    glVertex3f(-20.0, 15.0, 0.0);
    glVertex3f(20.0, 15.0, 0.0); 
    glVertex3f(33.0, -30.0, 0.0);
    glVertex3f(0.0, -35.0, 0.0);
    glVertex3f(-33.0, -30.0, 0.0);
    glEnd();
}

void Draw_Beak() {
    glBegin(GL_TRIANGLES);
    glVertex3f(10.0, -30.0, 0.0);
    glVertex3f(50.0, -30.0, 0.0);
    glVertex3f(10.0, -10.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(10.0, -28.0, 0.0);
    glVertex3f(40.0, -35.0, 0.0);
    glVertex3f(10.0, -40.0, 0.0);
    glEnd();
}

void Draw_Tail() {
    glBegin(GL_QUADS);
    glVertex3f(-40.0, -15.0, 0.0);
    glVertex3f(-50.0, 0.0, 0.0);
    glVertex3f(-80.0, -25.0, 0.0);
    glVertex3f(-50.0, -35.0, 0.0);
    glEnd();
}

//======================================================
// DISPLAY CALL BACK ROUTINE 
//======================================================
void displayCallBack(void)
{
    printf("Display call back %d\n", display_count++);
    cout << "pos of bird [" << bird_x << "," << bird_y << "]" << endl;

    if (bird_x > w_width / 2) {
        // Set Projection Mode and Volume
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //glOrtho(0.0, w_width, 0.0, w_height, -1.0, 1.0);
        glOrtho(bird_x - w_width / 2, bird_x + w_width / 2, 0.0, w_height, -1.0, 1.0);
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //draw bird
    glPushMatrix();

    glTranslatef(bird_x, bird_y, 0.0);
    glRotatef(theta, 0, 0, 1.0);


    glColor3ub(0, 200, 255); //Changed to light blue
    DrawEllipse(7, 4, 7, 4, 40);

    glTranslatef(30.0, 30.0, 0.0);
    DrawCircle(bird_r, 30); //Draw head

    
    glTranslatef(-30.0, -20.0, 0.0);
    glColor3ub(0, 175, 255);
    Draw_Wings(); //Draw Flappy's wings

    glTranslatef(40.0, 32.0, 0.0);
    glColor3ub(255, 255, 255); //White
    DrawCircle(Eye_radius, 20); 

    glTranslatef(0.0, 1.5, 0.0);
    glColor3ub(0, 0, 0); //Black
    DrawCircle(Eye_pupil, 20);

    glTranslatef(13.5, 18.0, 0.0);
    glColor3ub(255, 175, 0); //Orange
    Draw_Beak();

    glTranslatef(-65.0, -20.0, 0.0);
    glColor3ub(0, 175, 255);
    Draw_Tail();

    glPopMatrix();


    //draw obj
    glPushMatrix();
    glTranslatef(obj_x, obj_y, 1.0);
    glColor3f(0, 1, 0);
    DrawCircle(bird_r, 20);

    glPopMatrix();

    //Swap double buffers 
    glutSwapBuffers();
}

//======================================================
// MAIN PROGRAM
//======================================================
int main(int argc, char** argv)
{
    // Allow cmd line arguments to be passed to the glut
    glutInit(&argc, argv);

    // Create and name window
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(w_width, w_height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Flappy Bird");

    // Set Projection Mode and Volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w_width, 0.0, w_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Set clear color to black and clear window 
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    // Set up callbacks
    //glutMouseFunc(mouseCallBack);
    glutKeyboardFunc(keyboardCallBack);
    //glutMotionFunc(motionCallBack);
    glutDisplayFunc(displayCallBack);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(update);

    // Enter main event loop
    glutMainLoop();
}