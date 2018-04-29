#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "glext.h"
#include "datastruct.h"

// constants
const int	SCREEN_WIDTH	= 600;
const int	SCREEN_HEIGHT	= 800;
const GLfloat CAMERA_DISTANCE = 15.0f;
const int	TEXT_WIDTH		= 8;
const int	TEXT_HEIGHT		= 13;

class GLWrapper {
private:
	static void initGL();
	static int initGLUT(int argc, char **argv);
	static bool initSharedMem();
	static void clearSharedMem();
	static void initLights();

	// global variables
	static void (*myDisplayCB)(void);

	static constexpr void *font = GLUT_BITMAP_8_BY_13;
	static int screenWidth;
	static int screenHeight;
	static bool mouseLeftDown;
	static bool mouseRightDown;
	static bool mouseMiddleDown;
	static GLfloat mouseX, mouseY;
	static GLfloat cameraAngleX;
	static GLfloat cameraAngleY;
	static GLfloat cameraDistance;
	static int drawMode;
	static int maxVertices;
	static int maxIndices;

public:
	static bool init(); // TODO: try init all
	static void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
	static void setCameraDefaults();
	static void clearBuffer();
	static void pushMatrix();
	static void popMatrix();
	static void swapBuffers();

	// call back functions
	static void addDisplayCB(void (*func)(void));
	static void displayCB();
	static void reshapeCB(int w, int h);
	static void timerCB(int millisec);
	static void keyboardCB(unsigned char key, int x, int y);
	static void mouseCB(int button, int stat, int x, int y);
	static void mouseMotionCB(int x, int y);
	// TODO: function pointers from out of class, or overloading?

	// methods
	// void drawString(const char *str, int x, int y, float color[4], void *font);
	static void toPerspective();
	// void toOrtho();
	static void drawString3D(const char *str, float pos[3], float color[4], void *font);
	//static void drawQuad3D(Vect3D coord, Vect3D scale, Vect3D rotation, int flags);
	//static void drawLineSeg3D(Vect3D a, Vect3D b, float thic);
	static void drawPoly3D(Polygon3D &data, Transformation &trans) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 0, data.normals);
		glColorPointer(3, GL_FLOAT, 0, data.colors);
		glVertexPointer(3, GL_FLOAT, 0, data.vertices);

		glPushMatrix();

		// TODO APPLY TRANSFORMATION	//
		glTranslatef(trans.position.x, trans.position.y, trans.position.z);	// OG 2, 2, 0
		glRotatef(trans.degrees, trans.rotation.x, trans.rotation.y, trans.rotation.z);
		glScalef(trans.scale.x, trans.scale.y, trans.scale.z);	//


		glDrawElements(GL_TRIANGLES, data.count, GL_UNSIGNED_SHORT, data.indices);

		glPopMatrix();

		glDisableClientState(GL_VERTEX_ARRAY);	// disable vertex arrays
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	static void showInfo();

	GLWrapper(int argc, char **argv);
};
