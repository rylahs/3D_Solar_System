#include <gl/glut.h>
#include <gl/glu.h>
#include <gl/gl.h>
#include <gl/glaux.h>
#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "Planet.h"
#include "star.h"

#define MAX_CelestialBody 10
#define SUN		0
#define MERCURY 1
#define VENUS	2
#define EARTH	3
#define MARS	4
#define JUPITER	5
#define SATURN	6
#define URANUS	7
#define NEPTUNE	8
#define PLUTO	9

struct Rectangle
{
	int top;
	int bottom;
	int left;
	int right;
};

struct Rectangle boundingRect;	// Bounding rectangle
GLuint selectedObject = 0;		// Who is selected
float fAspect;					// Display aspect ratio

static GLfloat o_angle[MAX_CelestialBody] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static GLfloat radius[MAX_CelestialBody] = {
	1.392, 0.005, 0.012, 0.013, 0.007, 0.144, 0.121, 0.051, 0.050, 0.002
};
static GLfloat distanceFromSun[MAX_CelestialBody] = {
	0, 57.910, 108.200, 149.600, 227.940, 778.330, 1429.400, 2870.990, 4504.000, 5913.520
};

static GLfloat period_rotation[MAX_CelestialBody] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static GLfloat period_axis[MAX_CelestialBody] = {
	0, 0.01, 177.36, 23.439,25.19, 3.13, 26.73, 97.77, 28.32, 119.61
};

static GLfloat Orbital_inclination[MAX_CelestialBody] = {
	0, 7.0, 3.4, 0.0, 1.8, 1.3, 2.5, 0.8, 1.8, 17.1
};

static float eyex=0, eyey=300.0, eyez=0;
static GLdouble centerx=0, centery=0, centerz=0;

Star stars(10000);

static GLUquadricObj *pSphere = NULL;

GLUquadricObj * celestialBody[MAX_CelestialBody]; //glu object
GLuint tex[MAX_CelestialBody]; // texture mapping value

GLdouble wx, wy, wz;

//GLUquadricObj *earth;
//GLUquadricObj *earth1;
//GLuint tex[2]; //텍스쳐 맵핑 변수

// Light values
GLfloat LightAmbient[]=		{ 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat LightDiffuse[]=		{ 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat LightPosition[]=	{ 0.0f, 0.0f, 0.0f, 1.0f }; //조명의 위치

bool bLight = false, bLeft = false;
int mDownX, mDownY, drawMode = 0;
float anglex = 0, angley = 0, anglez = 0;

AUX_RGBImageRec *LoadBMPFile(char *filename){ // 이미지 읽어오기
	FILE *pFile = NULL;
	if(!filename) return NULL;
	pFile = fopen(filename, "r");

	if(pFile){
		fclose(pFile);
		return auxDIBImageLoad(filename);
	}
	return NULL;
}

int LoadGLTextures(){ //텍스쳐 맵핑
	int i;
	
	for(i=0; i<MAX_CelestialBody; i++)
	{
		celestialBody[i] = gluNewQuadric();
		gluQuadricDrawStyle(celestialBody[i], GLU_FILL);
		gluQuadricTexture(celestialBody[i], GL_TRUE);
	}

	AUX_RGBImageRec *textureimg[MAX_CelestialBody];
	int status=FALSE;
	memset(textureimg, 0, sizeof(void *)*1);

	if(
		(textureimg[0]=LoadBMPFile("sun.bmp")) && 
		(textureimg[1]=LoadBMPFile("mercury.bmp")) &&
		(textureimg[2]=LoadBMPFile("venus.bmp")) &&
		(textureimg[3]=LoadBMPFile("earth.bmp")) &&
		(textureimg[4]=LoadBMPFile("mars.bmp")) &&
		(textureimg[5]=LoadBMPFile("jupiter.bmp")) &&
		(textureimg[6]=LoadBMPFile("saturn.bmp")) &&
		(textureimg[7]=LoadBMPFile("uranus.bmp")) &&
		(textureimg[8]=LoadBMPFile("neptune.bmp")) &&
		(textureimg[9]=LoadBMPFile("pluto.bmp"))
		){
		glGenTextures(MAX_CelestialBody, &tex[0]);

		for( i=0; i<MAX_CelestialBody; i++){
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, textureimg[i]->sizeX, textureimg[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,textureimg[i]->data);
		}		
	}
	else return FALSE;

	for( i=0; i<MAX_CelestialBody; i++){
		if(textureimg[i]){
			if(textureimg[i]->data) free(textureimg[i]->data);
			free(textureimg[i]);
		} 
		else return FALSE;
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return TRUE;
}
void InitLight(){ //조명
	//조명 속성
	GLfloat sLight[4]={ 0.9, 0.9, 0.9,1.0 };
	GLfloat whiteLight[4] = { 0.3,0.3, 0.3, 0.33};
	GLfloat MySpotAngle[] = { 90.0};
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specref[] = { 1.0, 1.0, 1.0, 1.0 };

	//지구 속성	
	GLfloat material_ambient[]={0.4,0.4,0.4,1.0};
	GLfloat material_diffuse[]={0.9,0.9,0.9,1.0};
	GLfloat material_specular[]={1.0,1.0,1.0,1.0};
	GLfloat material_shininess[]={25.0};

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);

	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,material_ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	//크기 초기값
	//radius[0] /= 5;
	for(int i=1; i<MAX_CelestialBody; i++) {
		radius[i] *= 1000;
		//distanceFromSun[i] /= 10;
	}
}

void DrawObjects(void)
{
	// Save the matrix state and do the rotations
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		// Reset the modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		//gluLookAt(eyex, eyey, eyez,  centerx, centery, centerz,  0.0, 0.0, 1.0);
		//if(selectedObject > 0)
		//	gluLookAt(eyex, eyey, eyez,  distanceFromSun[selectedObject]*cos(o_angle[selectedObject]/8), centery, distanceFromSun[selectedObject]*sin(o_angle[selectedObject]/8),  0.0, 0.0, 1.0);
		//else
			gluLookAt(eyex, eyey, eyez,  centerx, centery, centerz,  0.0, 0.0, 1.0);
		//printf("center [%lf] [%lf] [%lf] \n", centerx, centery, centerz);
		//printf("wwwww  [%lf] [%lf] [%lf] \n", wx, wy, wz);
		printf("%d \n", selectedObject);

		//glRotatef(90, 1, 0, 0);

		glRotatef(anglex, 1, 0, 0);
		glRotatef(angley, 0, 1, 0);
		glRotatef(anglez, 0, 0, 1);

		glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);	
		glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);	
		glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);

		glEnable(GL_LIGHT1);

		glEnable(GL_LIGHT0);	
		//glEnable(GL_LIGHTING);
		// Red Nucleus

		stars.Draw();

		glDisable(GL_LIGHTING);
		
		glPushMatrix();
			//glTranslatef(0.0f, 0.0f, 0.0f);

			//gluLookAt(eyex, eyey, eyez,  distanceFromSun[selectedObject]*cos(o_angle[selectedObject]/8), centery, distanceFromSun[selectedObject]*sin(o_angle[selectedObject]/8),  0.0, 0.0, 1.0);
			if(selectedObject > 0) {
				centerx = distanceFromSun[selectedObject]*cos(o_angle[selectedObject]/8);
				centerz = distanceFromSun[selectedObject]*sin(o_angle[selectedObject]/8);
			}
			
			glInitNames();
			glPushName(0);

			glLoadName(SUN);
			glPassThrough((GLfloat)SUN);
			
			glEnable(GL_TEXTURE_2D);	
			glBindTexture(GL_TEXTURE_2D, tex[SUN]);  //GLuint
			glRotatef(period_rotation[SUN], 0, 1, 0);
			glRotatef(90, 1, 0, 0);
			gluSphere(celestialBody[SUN], radius[SUN], 30, 30);//GLUquadricObj*
			
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glEnable(GL_LIGHTING);

		glColor3ub(255,255,255);

		glLoadName(MERCURY);
		glPassThrough((GLfloat)MERCURY);
		Planet mercury(celestialBody[MERCURY], tex[MERCURY], radius[MERCURY], distanceFromSun[MERCURY], o_angle[MERCURY], period_axis[MERCURY], period_rotation[MERCURY]
		, Orbital_inclination[MERCURY]);
		mercury.Paint();

		glLoadName(VENUS);
		glPassThrough((GLfloat)VENUS);
		Planet venus(celestialBody[VENUS], tex[VENUS], radius[VENUS], distanceFromSun[VENUS], o_angle[VENUS], period_axis[VENUS], period_rotation[VENUS]
		, Orbital_inclination[VENUS]);
		venus.Paint();

		glLoadName(EARTH);
		glPassThrough((GLfloat)EARTH);
		Planet earth(celestialBody[EARTH], tex[EARTH], radius[EARTH], distanceFromSun[EARTH], o_angle[EARTH], period_axis[EARTH], period_rotation[EARTH]
		, Orbital_inclination[EARTH]);
		earth.Paint();

		glLoadName(MARS);
		glPassThrough((GLfloat)MARS);
		Planet mars(celestialBody[MARS], tex[MARS], radius[MARS], distanceFromSun[MARS], o_angle[MARS], period_axis[MARS], period_rotation[MARS]
		, Orbital_inclination[MARS]);
		mars.Paint();

		glLoadName(JUPITER);
		glPassThrough((GLfloat)JUPITER);
		Planet jupiter(celestialBody[JUPITER], tex[JUPITER], radius[JUPITER], distanceFromSun[JUPITER], o_angle[JUPITER], period_axis[JUPITER], period_rotation[JUPITER]
		, Orbital_inclination[JUPITER]);
		jupiter.Paint();

		glLoadName(SATURN);
		glPassThrough((GLfloat)SATURN);
		Planet saturn(celestialBody[SATURN], tex[SATURN], radius[SATURN], distanceFromSun[SATURN], o_angle[SATURN], period_axis[SATURN], period_rotation[SATURN]

		, Orbital_inclination[SATURN]);
		saturn.Paint();

		glLoadName(URANUS);
		glPassThrough((GLfloat)URANUS);
		Planet uranus(celestialBody[URANUS], tex[URANUS], radius[URANUS], distanceFromSun[URANUS], o_angle[URANUS], period_axis[URANUS], period_rotation[URANUS]
		, Orbital_inclination[URANUS]);
		uranus.Paint();

		glLoadName(NEPTUNE);
		glPassThrough((GLfloat)NEPTUNE);
		Planet neptune(celestialBody[NEPTUNE], tex[NEPTUNE], radius[NEPTUNE], distanceFromSun[NEPTUNE], o_angle[NEPTUNE], period_axis[NEPTUNE], period_rotation[NEPTUNE]
		, Orbital_inclination[NEPTUNE]);
		neptune.Paint();

		glLoadName(PLUTO);
		glPassThrough((GLfloat)PLUTO);
		Planet pluto(celestialBody[PLUTO], tex[PLUTO], radius[PLUTO], distanceFromSun[PLUTO], o_angle[PLUTO], period_axis[PLUTO], period_rotation[PLUTO]
		, Orbital_inclination[PLUTO]);
		pluto.Paint();
		// Restore the matrix state
	glPopMatrix();	// Modelview matrix	
}


// Called to draw scene
void RenderScene(void)
{

	glShadeModel(GL_SMOOTH);

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw the objects in the scene
	DrawObjects();
	// If something is selected, draw a bounding box around it
	if(selectedObject != 0) //selectedObject 각 Planet 개체의 숫자 enum
	{
		int viewport[4];

		// Get the viewport
		glGetIntegerv(GL_VIEWPORT, viewport);

		// Remap the viewing volume to match window coordinates (approximately)
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		// Establish clipping volume (left, right, bottom, top, near, far)
		glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
		glMatrixMode(GL_MODELVIEW);
		
		glDisable(GL_LIGHTING);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2i(boundingRect.left, boundingRect.top);
		glVertex2i(boundingRect.left, boundingRect.bottom);
		glVertex2i(boundingRect.right, boundingRect.bottom);
		glVertex2i(boundingRect.right, boundingRect.top);
		glEnd();
		glEnable(GL_LIGHTING);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Show the image
	glutSwapBuffers();
}

///////////////////////////////////////////////////////////
// Go into feedback mode and draw a rectangle around the object
#define FEED_BUFF_SIZE	32768
void MakeSelection(int nChoice)
{
	// Space for the feedback buffer
	static GLfloat feedBackBuff[FEED_BUFF_SIZE];

	// Storage for counters, etc.
	int size,i,j,count;

	// Initial minimum and maximum values
	boundingRect.right = boundingRect.bottom = -999999.0f;
	boundingRect.left = boundingRect.top =  999999.0f;

	// Set the feedback buffer
	glFeedbackBuffer(FEED_BUFF_SIZE,GL_2D, feedBackBuff);

	// Enter feedback mode
	glRenderMode(GL_FEEDBACK);

	// Redraw the scene
	DrawObjects();

	// Leave feedback mode
	size = glRenderMode(GL_RENDER);

	// Parse the feedback buffer and get the
	// min and max X and Y window coordinates
	i = 0;
	while(i < size)
	{
		// Search for appropriate token
		if(feedBackBuff[i] == GL_PASS_THROUGH_TOKEN)
			if(feedBackBuff[i+1] == (GLfloat)nChoice)
			{
				i+= 2;
				// Loop until next token is reached
				while(i < size && feedBackBuff[i] != GL_PASS_THROUGH_TOKEN)
				{
					// Just get the polygons
					if(feedBackBuff[i] == GL_POLYGON_TOKEN)
					{
						// Get all the values for this polygon
						count = (int)feedBackBuff[++i]; // How many vertices
						i++;

						for(j = 0; j < count; j++)	// Loop for each vertex
						{
							// Min and Max X
							if(feedBackBuff[i] > boundingRect.right)
								boundingRect.right = feedBackBuff[i];

							if(feedBackBuff[i] < boundingRect.left)
								boundingRect.left = feedBackBuff[i];
							i++;

							// Min and Max Y
							if(feedBackBuff[i] > boundingRect.bottom)
								boundingRect.bottom = feedBackBuff[i];

							if(feedBackBuff[i] < boundingRect.top)
								boundingRect.top = feedBackBuff[i];
							i++;
						}
					}
					else
						i++;	// Get next index and keep looking
				}
				break;
			}
			i++;
	}
}


///////////////////////////////////////////////////////////
// Process the selection, which is triggered by a right mouse
// click at (xPos, yPos).
#define BUFFER_LENGTH 64
void ProcessSelection(int xPos, int yPos)
{
	// Space for selection buffer
	static GLuint selectBuff[BUFFER_LENGTH];

	// Hit counter and viewport storage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(BUFFER_LENGTH, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	//printf("0:[%d]  1:[%d]  2:[%d]  3:[%d] \n", viewport[0], viewport[1], viewport[2], viewport[3]);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending two pixels
	// in the vertical and horizontal direction
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, 2,2, viewport);

	// Apply perspective matrix 
	gluPerspective(20, fAspect,1.0,100000.0);

	// Draw the scene
	DrawObjects();

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If a single hit occurred, display the info.
	if(hits == 1)
	{
		MakeSelection(selectBuff[3]);
		if(selectedObject == selectBuff[3]) {
			selectedObject = 0;
		}
		else {
			selectedObject = selectBuff[3];
		}
	}

	glutPostRedisplay();
}


// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	glEnable(GL_TEXTURE_2D); // Enable Testure
	// Light values and coordinates
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
}

void SpecialKeys(int key, int x, int y)
{
	switch(key){

	case GLUT_KEY_PAGE_DOWN : //줌 축소
		//radius[0] /= 5;
		for(int i=1; i<MAX_CelestialBody; i++) {
			radius[i] /= 10;
			//distanceFromSun[i] /= 10;
		}
		break;

	case GLUT_KEY_PAGE_UP : //줌 확대
		//radius[0] *= 5;
		for(int i=1; i<MAX_CelestialBody; i++) {
			radius[i] *= 10;
			//distanceFromSun[i] *= 10;
		}
		break;

	case GLUT_KEY_UP :
		if(eyey > 0)
			eyey=eyey-100;
		break;

	case GLUT_KEY_DOWN :
		if(eyey <= 29000)
			eyey=eyey+100;
		break;
		
	case GLUT_KEY_LEFT :
		//eye2=eye2-1.5;
		break;

	case GLUT_KEY_RIGHT :
		//eye2=eye2+1.5;
		break;
	}
	glutPostRedisplay();
}

//키보드 이벤트
void MyKeyboard(unsigned char key, int x, int y) { 
	switch (key) {
		case 'X':
			eyex += 5.0f;
			break;
		case 'x':
			eyex -= 5.0f;
			break;
		case 'Y':
			eyey += 5.0f;
			break;
		case 'y':
			eyey -= 5.0f;
			break;
		case 'Z':
			eyez += 5.0f;
			break;
		case 'z':
			eyez -= 5.0f;
			break;
			
		case 'q':
			centerx += 5.0f;
			break;
		case 'Q':
			centerx -= 5.0f;
			break;
		case 'w':
			centery += 5.0f;
			break;
		case 'W':
			centery -= 5.0f;
			break;
		case 'e':
			centerz += 5.0f;
			break;
		case 'E':
			centerz -= 5.0f;
			break;
	}
	glutPostRedisplay();
}


void TimerFunc(int value)
{
	o_angle[MERCURY]	+= 0.17051f;
	o_angle[VENUS]		+= 0.06676f;
	o_angle[EARTH]		+= 0.04107f;
	o_angle[MARS]		+= 0.02183f;
	o_angle[JUPITER]	+= 0.00347f;
	o_angle[SATURN]		+= 0.00140f;
	o_angle[URANUS]		+= 0.00049f;
	o_angle[NEPTUNE]	+= 0.00025f;
	o_angle[PLUTO]		+= 0.00017f;

	for(int i=0; i<MAX_CelestialBody; i++)
	{
		if(o_angle[i] > 360.0f)
			o_angle[i] -= 360.0f;
	}

	period_rotation[SUN]		+= 0.556f;
	period_rotation[MERCURY]	+= 0.256f;
	period_rotation[VENUS]		+= 0.062f;
	period_rotation[EARTH]		+= 15.041f;
	period_rotation[MARS]		+= 14.634f;
	period_rotation[JUPITER]	+= 36.0f;
	period_rotation[SATURN]		+= 35.18f;
	period_rotation[URANUS]		+= 20.112f;
	period_rotation[NEPTUNE]	+= 18.848f;
	period_rotation[PLUTO]		+= 2.347f;

	for(int i=0; i<MAX_CelestialBody; i++)
	{
		if(period_rotation[i] > 360.0f)
			period_rotation[i] -= 360.0f;
	}
	glutPostRedisplay();
	glutTimerFunc(1, TimerFunc, 1);
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y)
{
	if(Button==GLUT_LEFT_BUTTON && State==GLUT_DOWN) //마우스 클릭시 시작 좌표 저장
	{
		ProcessSelection(X, Y);
		mDownX = X;
		mDownY = Y;
		bLeft = true; //왼클릭시에만 화면 돌아가도록 설정
	}
	else if(Button==GLUT_LEFT_BUTTON && State==GLUT_UP)
		bLeft = false;
}

void MyMouseMove(GLint X, GLint Y)
{
	if(bLeft) //시작 좌표를 기준으로 시야를 회전시킴
	{
		anglex += ((float)Y-mDownY) / 3.6; 
		angley += ((float)X-mDownX) / 3.6;
		glutPostRedisplay( );
		mDownX = X;
		mDownY = Y;
	}
}



void ChangeSize(int x, int y)
{

	glViewport(0, 0, (GLsizei) x, (GLsizei) y);
	
	// Calculate aspect ratio of the window
	fAspect = (GLfloat)x/(GLfloat)y;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20, fAspect,1.0,100000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1600, 900);
	glutCreateWindow("OpenGL Atom");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(MyKeyboard);
	
	LoadGLTextures();
	SetupRC();
	InitLight();
	
	glutDisplayFunc(RenderScene);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMove);
	glutTimerFunc(500, TimerFunc, 1);
	glutMainLoop();

	return 0;
}
