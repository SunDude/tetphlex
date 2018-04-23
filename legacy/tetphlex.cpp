#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "drawgeometry.h"

#include <iostream>

using std::clog;
bool DEBUG = false;

const int   SCREEN_WIDTH    = 800;
const int   SCREEN_HEIGHT   = 600;
const float CAMERA_DISTANCE = 10.0f;
const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 13;

#define drawOneLine(x1,y1,x2,y2)  glBegin(GL_LINES);  \
	glVertex2f ((x1),(y1)); glVertex2f ((x2),(y2)); glEnd();

template <typename T>
void debugOut(T t) {
	if(DEBUG) {
		clog<< t;
	}
}

template<typename T, typename... Args>
void debugOut(T t, Args... args) {
	if(DEBUG) {
		clog<< t;
		debugOut(args...);
	}
};

void drawString3D(const char *str, float pos[3], float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);          // set text color
	glRasterPos3fv(pos);        // place text position

	// loop all characters in the string
	while(*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

class WinConfig{
	public:
		int width, height;
		WinConfig(int _w, int _h) : width(_w), height(_h) {
		}
} winConfig(SCREEN_WIDTH, SCREEN_HEIGHT);

void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

void toPerspective() {
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)winConfig.width, (GLsizei)winConfig.height);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(winConfig.width)/winConfig.height, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	
	// tramsform camera
	glTranslatef(0, 0, -CAMERA_DISTANCE);
	
	float cameraAngleX = 0, cameraAngleY = 0;
	glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	glRotatef(cameraAngleY, 0, 1, 0);   // heading

	glColor3f(1.0, 1.0, 1.0); // set colour to white
	glLoadIdentity();             /* clear the matrix */
	// gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);
	glutWireCube(1.0);
	debugOut("before processing\n");
	GLdouble myCubeVertices[12*3*3];
	drawGeo::getCubeVertex(myCubeVertices, 0.0f, 0.0f, 0.0f, 0.5f); // allocated new cube vertex array
	for (int i=0; i<12*3; i++) {
		for (int j=0; j<3; j++) {
			debugOut(j?", ":"", myCubeVertices[i+j]);
		}
		debugOut("\n");
	}
	debugOut("after vertex generation\n");

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_DOUBLE, 0, myCubeVertices); // specify pointer to vertex array

	glPushMatrix();
	glTranslatef(2, 2, 0);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // draw
	glPopMatrix();
	

	glDisableClientState(GL_VERTEX_ARRAY);

	glFlush();
}

void reshape(int w, int h) {
	winConfig = WinConfig(w, h);
	toPerspective();
}

void mouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) {
				glutDestroyWindow(glutGetWindow());
				exit(0);
			}
			break;
		default:
			break;
	}
}

int initGLUT(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(winConfig.width, winConfig.height);
	glutInitWindowPosition(100,100);
	glutCreateWindow("tetphlex demo");

	// glutFullScreen();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
}

void initGL() {
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	glClearColor(0, 0, 0, 0);
	//glClearStncil(0);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	// initLights();
}

int main(int argc, char** argv) {
	initGLUT(argc, argv);
	initGL();

	glutMainLoop();    
	return 0;
}

