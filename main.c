#include <gl/freeglut.h>
#include <sys/time.h>
#include "stdio.h"
#include "car.h"
#include "offfile.h"
#include <stdint.h>
#include "qdbmp.h"

#define TIMERMSECS 16
#define TIMER_DELTA 0.0166

CARTYPE cartypes[1];
CAR car;

Object3D* track;
Object3D* trackCheckpoints;

Object3D* carBody;
Object3D* Wheel;

Object3D* Tree;




bitmap* testimage;


vector carbounds[4] ={{-0.01,0,0.01},{0.01,0,0.01},{0.01,0,-0.01},{-0.01,0,-0.01}};

char cox[30];
char coz[30];
char v[30];
char l[30];
char cp[30];

char l1[30];
char l2[30];
char l3[30];

double trackscale = 6.0;
double checkpointscale = 5.0;

struct timeval previous,currentt, startgametime,endgametime;
struct timeval laps[3];
float laptimes[3];

enum {
	ORTHO2D, PERSPECTIVE, FRUSTUM, ORTHO
} mode = PERSPECTIVE;

enum {FIXED_MIDDLE, THIRD_PERSON, TOP_DOWN} view = FIXED_MIDDLE;

#define GAP  15 // gap between subwindows
//  define the window position on screen
float main_window_x = 10;
float main_window_y = 10;

//  variables representing the window size
float main_window_w = 1000 + GAP * 2;
float main_window_h = 800 + 64 + GAP * 3;

//  variable representing the window title
char *window_title = "A brisk few laps around the track";

//  Represents the window id
int main_window;

float subwindow1_x = GAP;
float subwindow1_y = GAP;

//  variables representing the window size
float subwindow1_w = 256;
float subwindow1_h = 256;

float frametime;

//  Represents the subwindow id
int subwindow_1;




void sub_display (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

    //  Set background color to white
    glClearColor(1.0, 1.0, 1.0, 1.0);


    gluLookAt(0,15.0,0,-1,0,0,0,1,0);

    renderStats(3,0,4);

    glutSwapBuffers();
}



 //Car controls
 float throttle = 0;
 float steering = 0;
 int ebrake = 0;

//game values
 int ontrack = 1;
 int checkpoint = 0;
 int lap = 1;
 int maxlaps = 3;
 int gamewon = 0;

 int gamestart = 0;

 vector checkpointFixPosition;
 float scaleAdjustment;

 GLfloat light_position[] = {0.904,0.301,0.301,0.0};
 GLfloat light_position2[] = {0,1,0,0.0};

 void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


void setLight(void){


    /*glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS, mat_shininess);*/
	/*
	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    glLightfv (GL_LIGHT0,GL_AMBIENT,lmodel_ambient);
    glLightfv (GL_LIGHT0,GL_DIFFUSE,lmodel_diffuse);

    glLightfv(GL_LIGHT1,GL_POSITION,light_position2);
    glLightfv(GL_LIGHT1,GL_AMBIENT,lmodel_ambient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,fmodel_diffuse);*/

    glLightfv(GL_LIGHT0,GL_POSITION,light_position);

    glLightfv(GL_LIGHT1,GL_POSITION,light_position2);


	//glEnable(GL_BLEND);


}

void init_cartypes( void )
{
	CARTYPE	*cartype;
    cartype = &cartypes[0];
    cartype->mass = 1200.0;
    cartype->intertiascale = 1.0;
    cartype->halfwidth = 0.8;
    cartype->cgToFront = 2.0;
    cartype->cgToRear = 2.0;
    cartype->cgToFrontAxle = 1.25;
    cartype->cgToRearAxle = 1.25;
    cartype->cgheight = 0.55;
    cartype->wheelRadius = 0.3;
    cartype->wheelWidth = 0.2;
    cartype->tireGrip = 2.0;
    cartype->lockGrip = 0.7;
    cartype->engineForce = 8000.0;
    cartype->brakeForce = 12000.0;
    cartype->eBrakeForce = cartype->brakeForce / 2.5;
    cartype->weightTransfer = 0.2;
    cartype->maxSteer = 0.6;
    cartype->cornerStiffnessFront = 5.0;
    cartype->cornerStiffnessRear = 5.2;
    cartype->airResist = 2.5;
    cartype->rollResist = 8.0;


}

void init_car( CAR *car, CARTYPE *cartype )
{
	car->cartype = cartype;

    car->position.x = 20;
	car->position.y = 0;
	car->position.z = -44;

	car->velocity_c.x = 0;
	car->velocity_c.y = 0;
	car->velocity_c.z = 0;

	car->velocity.x = 0;
	car->velocity.y = 0;
	car->velocity.z = 0;

    car->heading = 0.5;
    car->angularvelocity = 0;
    car->absVel = 0;
    car->yawRate = 0;
    car->acceleration_c.x = 0;
    car->acceleration_c.y = 0;
    car->acceleration_c.z = 0;
    car->acceleration.x = 0;
    car->acceleration.y = 0;
    car->acceleration.z = 0;
    car->steerangle = 0;
    car->throttle = 0;
    car->brake = 0;

    car->movementscale = 0.1;
    car->ontrack = 0;

	init_car_physics(car);

}

void SetViewMode()
{
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	 if (mode == ORTHO2D){
        // 2D viwwing (no Z coordnates
        gluOrtho2D(0.0,500.0, 0.0,500.0);
    }if (mode == ORTHO){
        // This is orthographic projection
        GLdouble left = -2;
        GLdouble right= 2;
        GLdouble bottom = -2;
        GLdouble top    = 2;
        GLdouble nearVal = 0.1;
        GLdouble farVal  = 100;     // near and far clipping planes
        glOrtho(left,  right,
                  bottom,  top,
                  nearVal,  farVal);

    }else if (mode == PERSPECTIVE){
        //
        GLdouble fov	 = 90;		// degrees
        GLdouble aspect	 = 1;		// aspect ratio aspect = height/width
        GLdouble nearVal = 1;
        GLdouble farVal  = 600;     // near and far clipping planes
        gluPerspective(fov, aspect, nearVal, farVal);

    }else if (mode == FRUSTUM){
        GLdouble left = -1;
        GLdouble right= 1;
        GLdouble bottom = -1;
        GLdouble top    = 1;
        GLdouble nearVal = .05;
        GLdouble farVal  = 100;     // near and far clipping planes
        glFrustum(left,  right,
				  bottom,  top,
                  nearVal,  farVal);
    }

   glMatrixMode(GL_MODELVIEW);
}

void init(void)
{
	setLight();


    //glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);



	glClearColor(1.0,1.0,1.0,1.0);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(5.0);


    init_cartypes();
    init_car(&car, &cartypes[0]);



    //load track

    track = readOFFFile("./Models/track.off");
    trackCheckpoints = readOFFFile("./Models/checkpointscolour.off");
    carBody = readOFFFile("./Models/carv2.off");
    Wheel = readOFFFile("./Models/wheelv2.off");
    Tree = readOFFFile("./Models/Tree.off");
	mode = PERSPECTIVE;

    SetViewMode();

    // now setting the camera
    glLoadIdentity();               // start with identity transformation

	gluLookAt(	1, 1.0f, 1,
				0,0,0,
				0.0f, 1.0f,  0.0f);




    checkpointFixPosition = VectorSubtract(trackCheckpoints->positionAdjustment,track->positionAdjustment);
    checkpointFixPosition = VectorMultiplyByScaler(checkpointFixPosition,checkpointscale);
    scaleAdjustment = track->calcMax / trackCheckpoints->calcMax;


    testimage = read_bitmapfile("Untitled.bmp");




    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = {50.0};
	GLfloat lmodel_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lmodel_diffuse[] = {0.9,0.9,0.9,0.0};
    GLfloat fmodel_diffuse[] = {0.5,0.5,0.5,0.0};
    GLfloat local_view[] = {1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);

    glLightfv(GL_LIGHT0,GL_DIFFUSE,lmodel_diffuse);
    glLightfv(GL_LIGHT0,GL_AMBIENT,lmodel_ambient);

    glLightfv(GL_LIGHT1,GL_DIFFUSE,fmodel_diffuse);

    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    //glDepthRange(0.0f,1.0f);

    gettimeofday(&previous, NULL);

    printf("finished init");

}




void pressKey(int key, int x, int y)
{

    switch(key) {
        case GLUT_KEY_UP: throttle = 1.0f; break;
        case GLUT_KEY_DOWN: throttle = -1.0f; break;
        case GLUT_KEY_LEFT: steering = -1; break;
        case GLUT_KEY_RIGHT: steering = 1; break;
    }

	//glutPostRedisplay();
}

void normalKey(unsigned char key, int x, int y)
{

   	if((key=='P') || (key == 'p'))
    {
        mode = PERSPECTIVE;
        SetViewMode();
    }
    if((key=='O') || (key=='o'))
    {
        mode=ORTHO;
        SetViewMode();
    }

    if(key=='g' || key == 'G')
    {
        if(gamestart == 0 )
            gettimeofday(&startgametime,NULL);
        gamestart = 1;
    }

    if(key == ' ')
        ebrake = 1;

    if(key == '1')
        view = FIXED_MIDDLE;
    if(key == '2')
        view = THIRD_PERSON;
    if(key == '3')
        view = TOP_DOWN;


	if(key == 'q') exit(0);
	if(key == 'Q') exit(0);

	//glutPostRedisplay();
}

void normalKeyUp(unsigned char key, int x, int y)
{
    if(key == ' ')
        ebrake = 0;
}

void releaseKey(int key, int x, int y)
{
     switch(key) {

            case GLUT_KEY_UP: throttle = 0.0f; break;
            case GLUT_KEY_DOWN: throttle = 0.0f; break;
            case GLUT_KEY_LEFT: steering = 0; break;
            case GLUT_KEY_RIGHT: steering = 0; break;
    }
}


void CheckCollissions()
{
    //bounding box of car
    vector carpos[4];
    vector trackpos[3];
    //carpos[0] = Vector

    //add car position
    for(int i = 0;i<4;i++)
    {
        carpos[i] = VectorMultiplyByScaler(VectorAdd(carbounds[i],car.position),car.movementscale);

    }

    ontrack = 0;

    for(int i = 0;i<track->nfaces;i++)
    {
        //printf("\nTrack positions initial....\n");
        for(int a = 0;a<3;a++)
        {
            trackpos[a] = VectorMultiplyByScaler((vector){
                                                    .x = track->vertices[track->faces[i][a]].x,
                                                    .y = track->vertices[track->faces[i][a]].y,
                                                    .z = track->vertices[track->faces[i][a]].z}
                                                    ,trackscale);
            //PrintVector(trackpos[a]);
        }

        if(CheckCollision(&carpos,4,&trackpos,3) == 1)
        {

            //printf("Car is on track...\n");
            car.ontrack = 1;
            ontrack = 1;
        }

        if(ontrack == 1)
            break;
    }
    if(ontrack == 0)
    {
        car.ontrack = 0;
        //printf("Car is not on the track...\n");
    }

    //check checkpoints
    if(!gamewon)
    {
        vector fixscale = VectorMultiplyByScaler(checkpointFixPosition,1.2);
        for(int a = 0;a<3;a++)
        {
            trackpos[a] = (vector){.x = trackCheckpoints->vertices[trackCheckpoints->faces[checkpoint][a]].x,
                                   .y = trackCheckpoints->vertices[trackCheckpoints->faces[checkpoint][a]].y,
                                   .z = trackCheckpoints->vertices[trackCheckpoints->faces[checkpoint][a]].z};


            trackpos[a] = VectorMultiplyByScaler(trackpos[a],trackscale * scaleAdjustment);
            trackpos[a] = VectorAdd(trackpos[a], fixscale);
            glVertex3f(trackpos[a].x,trackpos[a].y,trackpos[a].z);
        }
        //check if car has hit checkpoint
        if(CheckCollision(&carpos,4,&trackpos,3) == 1)
        {
            //we have, advance checkpoint
            checkpoint = (checkpoint + 1) % trackCheckpoints->nfaces;
            if(checkpoint == 0)
            {
                gettimeofday(&laps[lap-1],NULL);
                if(lap == 1)
                    laptimes[lap-1] = ((laps[lap-1].tv_sec * 1000000 + laps[lap-1].tv_usec) -( startgametime.tv_sec * 1000000 +  startgametime.tv_usec)) / 1000000.0;
                else
                    laptimes[lap-1] = ((laps[lap-1].tv_sec * 1000000 + laps[lap-1].tv_usec) -( laps[lap-2].tv_sec * 1000000 +  laps[lap-2].tv_usec)) / 1000000.0;
                lap++;

            }
        }
    }


}


void PrintVector(vector v)
{
    printf("x %f y %f z %f\n",v.x,v.y,v.z);
}


static void animate(int value)
{

    //system("@cls||clear");

    gettimeofday(&currentt,NULL);

	glutTimerFunc(TIMERMSECS,animate,0);


	uint64_t delta_us = (currentt.tv_sec * 1000000 + currentt.tv_usec) -( previous.tv_sec * 1000000 +  previous.tv_usec);

    double delta = ((double)delta_us)/ 1000000.0;

    frametime = delta;

	previous = currentt;
    //printf("Time taken %f\n",delta);

	if(gamewon != 1 && gamestart == 1)
    {
        do_physics(&car, delta);

        CheckCollissions();

        car_input(&car, throttle, ebrake, steering,delta);
        if(lap == 4)
            gamewon = 1;
    }

    //printf("Checkpoint %d Lap %d Game Won %d Lap time 1 %f 2 %f 3 %f throttle %.3f steering %.3f\n",checkpoint +1,lap,gamewon,laptimes[0],laptimes[1],laptimes[2],car.throttle,car.steerangle);

    sprintf(v, "Velocity : %f", car.absVel);
    sprintf(cox,"X coordinate : %f", car.position.x);
    sprintf(coz,"Z coordinate : %f", car.position.z);
    sprintf(l, "Lap : %d",lap);
    sprintf(cp, "Checkpoint : %d",checkpoint+1);

    sprintf(l1,"Lap 1 %.2f",laptimes[0]);
    sprintf(l2,"Lap 2 %.2f",laptimes[1]);
    sprintf(l3,"Lap 3 %.2f",laptimes[2]);

    glutSetWindow(main_window);
    glutPostRedisplay();

    glutSetWindow(subwindow_1);

    glutPostRedisplay();
}

void drawFace(Object3D* o3d,int face)
{
        glBegin(GL_TRIANGLES);




        Face f;
        //printf("F%d 1 %d\n",i,mybone->faces[i][0]);
        f[0] = o3d->faces[face][0];
        //printf("F%d 2 %d\n",i,mybone->faces[i][1]);
        f[1] = o3d->faces[face][1];
        //printf("F%d 3 %d\n",i,mybone->faces[i][2]);
        f[2] = o3d->faces[face][2];

        //printf("V%d 1 %f\n",i,mybone->vertices[f[0]][0]);


        //vector normal = TriangleSurfaceNormal(o3d->vertices[f[0]],o3d->vertices[f[1]],o3d->vertices[f[2]]);
        //glNormal3f(normal.x, normal.y, normal.z);
        glNormal3f(o3d->normals[face].x, o3d->normals[face].y, o3d->normals[face].z);

        if(o3d->hascolors)
        {
            //printf("Vertex color %d %d %d %d %d\n",i,o3d->colors[f[0]][0],o3d->colors[f[0]][1],o3d->colors[f[0]][2],o3d->colors[f[0]][3]);
            glColor3f(o3d->colors[f[0]][0],o3d->colors[f[0]][1],o3d->colors[f[0]][2]);
        }
        glVertex3f(o3d->vertices[f[0]].x, o3d->vertices[f[0]].y,o3d->vertices[f[0]].z);
        //printf("V%d 2 %f\n",i,mybone->vertices[f[1]][0]);
        if(o3d->hascolors)
            glColor3f(o3d->colors[f[1]][0],o3d->colors[f[1]][1],o3d->colors[f[1]][2]);
        glVertex3f(o3d->vertices[f[1]].x,o3d->vertices[f[1]].y,o3d->vertices[f[1]].z);
        //printf("V%d 3 %f\n",i,mybone->vertices[f[2]][0]);
        if(o3d->hascolors)
            glColor3f(o3d->colors[f[2]][0],o3d->colors[f[2]][1],o3d->colors[f[2]][2]);
        glVertex3f(o3d->vertices[f[2]].x,o3d->vertices[f[2]].y,o3d->vertices[f[2]].z);
        glEnd();
}

void renderBitmapString(float x,float y,float z,void *font,char *string)
{
    char *c;
    glRasterPos3f(x,y,z);
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void renderStats(int xpos, int ypos, int zpos)
{
    glPushMatrix();

    glTranslatef(xpos, ypos, zpos);

    if(gamewon == 0)
    {
        if(gamestart == 0)
        {
            glColor3f(0,0,0);
            renderBitmapString(-5,0,-0.6,GLUT_BITMAP_HELVETICA_18,"Press g to start game");
            renderBitmapString(-4,0,-0.6,GLUT_BITMAP_HELVETICA_18,"Press q to quit game");
            renderBitmapString(-3,0,-0.6,GLUT_BITMAP_HELVETICA_18,"Arrow Keys to steer");
            renderBitmapString(-2,0,-0.6,GLUT_BITMAP_HELVETICA_18,"Space bar to slide");
            renderBitmapString(-1,0,-0.6,GLUT_BITMAP_HELVETICA_18,"1,2,3 to change views");
        }
        else
        {
            glColor3f(1,0,0);
            renderBitmapString(-7,0,1,GLUT_BITMAP_HELVETICA_18,"Game Statistics");

            glColor3f(0,0,0);



            //renderBitmapString(-7,0,1,GLUT_BITMAP_HELVETICA_18, ))
            renderBitmapString(-6,0,1,GLUT_BITMAP_HELVETICA_18, v);
            renderBitmapString(-5,0,1,GLUT_BITMAP_HELVETICA_18, cox);
            renderBitmapString(-4,0,1,GLUT_BITMAP_HELVETICA_18, coz);
            renderBitmapString(-3,0,1,GLUT_BITMAP_HELVETICA_18, l);
            renderBitmapString(-2,0,1,GLUT_BITMAP_HELVETICA_18, cp);
            renderBitmapString(-1,0,1,GLUT_BITMAP_HELVETICA_18, l1);
            renderBitmapString(0,0,1,GLUT_BITMAP_HELVETICA_18, l2);
            renderBitmapString(1,0,1,GLUT_BITMAP_HELVETICA_18, l3);

        }
    }
    else
    {
        glColor3f(0.365, 0.02, 0.576);
        renderBitmapString(-5,0,-0.6,GLUT_BITMAP_HELVETICA_18,"YOU WIN!");
    }

    glPopMatrix();
}

void drawOffFile(Object3D* o3d)
{
    int i;
    for(i = 0;i<o3d->nfaces;i++)
    {
        drawFace(o3d,i);
    }
}

void drawGround()
{
    glColor3f(0.321, 0.560, 0.141);//green
    glBegin(GL_POLYGON);
        glVertex3f(100,-0.1,100);
        glVertex3f(100,-0.1,-100);
        glVertex3f(-100,-0.1,-100);
        glVertex3f(-100,-0.1,100);
    glEnd();
}

void drawTrees()
{
    glPushMatrix();

    glTranslatef(0,0.6,0);
    drawOffFile(Tree);

    glTranslatef(-1.9,0,1.2);
    drawOffFile(Tree);

    glTranslatef(4.0,0,-4);
    drawOffFile(Tree);

    glPopMatrix();
}

void steeringAngle()
{

    glRotatef(-(car.heading*180/3.1415926),0,1,0);

/*    double prevangle =0;

    if(car.steerangle >= prevangle)
    {
        glRotatef(-(car.heading*180/3.1415926),0,1,0);
        prevangle = car.steerangle;
    }
    else
    {
        prevangle = car.steerangle;
    }*/
}

float previousRotation;

void DrawWheels()
{

    float carspeed = VectorMagnitude(car.velocity);


    previousRotation -= carspeed * frametime * 100.0;
    previousRotation = fmod(previousRotation,360.0);

    //printf("%f\n",previousRotation);

    //Draw Back Wheels

    glColor3f(0.4,0.4,0.4);
    glPushMatrix();
    glTranslatef(-0.5,0.0f,-0.33);
    glScalef(1.4,1.4,1.4);
    //steeringAngle();

    glPushMatrix();
    glRotatef(previousRotation,0.0,0.0,1.0);

    drawOffFile(Wheel);

    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5,0.0f,0.31);
    glScalef(1.4,1.4,1.4);
    //steeringAngle();

    glPushMatrix();
    glRotatef(previousRotation,0.0,0.0,1.0);

    drawOffFile(Wheel);

    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.6,0.0,0.31);
    glScalef(1.4,1.4,1.4);
    glPushMatrix();
    glRotatef(car.steerangle*-30,0,1,0);
    glPushMatrix();
    glRotatef(previousRotation,0.0,0.0,1.0);

    drawOffFile(Wheel);

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();

    glTranslatef(0.6,0.0,-0.31);
    glScalef(1.4,1.4,1.4);
    glPushMatrix();
    glRotatef(car.steerangle*-30,0,1,0);
    glPushMatrix();
    glRotatef(previousRotation,0.0,0.0,1.0);

    drawOffFile(Wheel);

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();


/*
    glColor3f(1.0,0.0,0.0);

    glRotatef(-90,1,0,0);
    glRotatef(90,0,0,1);

    //drawOffFile(Wheel);
    for(int i=0; i<1080; i++)
    {
        drawFace(Wheel,i);
    }

    glPopMatrix();*/
}

void drawCar()
{
    glPushMatrix();

    vector ctemp = VectorMultiplyByScaler(car.position,car.movementscale);
    glTranslatef(ctemp.x, 0, ctemp.z);


    steeringAngle();

    glTranslatef(0,0.2f,0);
    glScalef(0.3,0.3,0.3);

    //glRotatef(-90,1,0,0);
    //glRotatef(90,0,0,1);


    /*for(int i=0; i<3538; i++)
    {
        drawFace(carBody,i);
    }*/

    DrawWheels();

    glPushMatrix();

    glScalef(1.3,1.3,1.3);
    glTranslatef(0.3,0.1,0);
    glColor3f(0,0,1);

    drawOffFile(carBody);

    glPopMatrix();

    glPopMatrix();
}


void drawTrack()
{
    glPushMatrix();

    glColor3f(0.1f,0.1f,0.1f);
    //glTranslatef(0,1,0);
    glScalef(trackscale,1,trackscale);
    drawOffFile(track);
    glPopMatrix();
}

void drawTrackCheckPoints()
{
    glPushMatrix();

    glTranslatef(checkpointFixPosition.x,checkpointFixPosition.y,checkpointFixPosition.z);

    //PrintVector(position);


    glScalef(trackscale * scaleAdjustment,1,trackscale * scaleAdjustment);
    glTranslatef(0,0.1f,0);

    drawFace(trackCheckpoints,checkpoint);

    glPopMatrix();
    //drawOffFile(trackCheckpoints);
}


void drawBitmap(bitmap * bmp)
{

        glDrawPixels(512,512,GL_RGB,GL_UNSIGNED_BYTE,testimage->bmp->Data);
}


void display(void){

    //if(deltaAngle)
        //computeDir(deltaAngle);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

    //  Set background color to white
    glClearColor(0.546875, 0.7421875, 0.8359375, 1.0);
    glClearDepth(1.0f);
	vector ctemp = VectorMultiplyByScaler(car.position,car.movementscale);

    //PrintVector(ctemp);
    if(view == FIXED_MIDDLE)
    {
        gluLookAt(0,5,0,ctemp.x,ctemp.y,ctemp.z,0,1,0);
    }
    else if(view == THIRD_PERSON)
    {
        double xv = cos(car.heading); //
        double zv = sin(car.heading); //
        vector heading = (vector){.x = -xv, .y = 0 ,.z = -zv};
        heading = VectorNormalize(heading);
        heading = VectorAdd(heading,ctemp);
        heading.y = 0.5;
        //PrintVector(heading);
        gluLookAt(heading.x,heading.y,heading.z,ctemp.x,ctemp.y+0.5,ctemp.z,0,1,0);

    }
    else if(view == TOP_DOWN)
    {
        gluLookAt(0,15.0,0,-1,0,0,0,1,0);
    }

	setLight();
	/*gluLookAt(	1, 1.0f, 1,
				car.position_wc.x, car.position_wc.y, car.position_wc.z,
				0.0f, 1.0f,  0.0f);*/

    drawGround();
    drawTrees();
    drawTrack();
    drawTrackCheckPoints();

/*    drawLFWheel();
    drawLBWheel();
    drawRFWheel();
    drawRBWheel();*/
    drawCar();

    if(gamewon == 1)
        drawBitmap(testimage);

    glutSwapBuffers();
}




int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA |  GLUT_DOUBLE | GLUT_DEPTH);

	/*glutInitWindowSize(500,500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Hello OpenGL");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);

	glutKeyboardFunc(normalKey);
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutKeyboardUpFunc(normalKeyUp);*/

    glutInitWindowSize (main_window_w, main_window_h);
	glutInitWindowPosition (main_window_x, main_window_y);
	main_window = glutCreateWindow (window_title);

    glutDisplayFunc (display);
	glutReshapeFunc  (changeSize);
    glutKeyboardFunc(normalKey);
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutKeyboardUpFunc(normalKeyUp);

	init();
	/**** Subwindow 1 *****/
    subwindow_1 = glutCreateSubWindow (main_window, subwindow1_x, subwindow1_y, subwindow1_w, subwindow1_h);

	glutDisplayFunc (sub_display);
	glutReshapeFunc  (changeSize);
    glutKeyboardFunc(normalKey);
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutKeyboardUpFunc(normalKeyUp);




	glutTimerFunc(TIMERMSECS,animate,0);

	glutMainLoop();

	return 0;
}
