#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "glwrapper.h"

using std::stringstream;
using std::cout;
using std::endl;
using std::ends;

#ifdef _WIN32
// function pointer to OpenGL extensions
// glDrawRangeElements() defined in OpenGL v1.2 or greater
PFNGLDRAWRANGEELEMENTSPROC pglDrawRangeElements = 0;
#define glDrawRangeElements pglDrawRangeElements
#endif

// global variables
void (*GLWrapper::myDisplayCB)(void) = NULL;

int GLWrapper::screenWidth = SCREEN_WIDTH;
int GLWrapper::screenHeight = SCREEN_HEIGHT;
bool GLWrapper::mouseLeftDown = false;
bool GLWrapper::mouseRightDown = false;
bool GLWrapper::mouseMiddleDown = false;
GLfloat GLWrapper::mouseX = 0, GLWrapper::mouseY = 0;
GLfloat GLWrapper::cameraAngleX = 0.0f;
GLfloat GLWrapper::cameraAngleY = 0.0f;
GLfloat GLWrapper::cameraDistance = CAMERA_DISTANCE;
int GLWrapper::drawMode = 0;
int GLWrapper::maxVertices = 0;
int GLWrapper::maxIndices = 0;

// cube ///////////////////////////////////////////////////////////////////////
//	  v6----- v5
//	 /|		 /|
//	v1------v0|
//	| |		| |
//	| |v7---|-|v4
//	|/		|/
//	v2------v3

// vertex array for glDrawElements() and glDrawRangeElement() =================
// Notice that the sizes of these arrays become samller than the arrays for
// glDrawArrays() because glDrawElements() uses an additional index array to
// choose designated vertices with the indices. The size of vertex array is now
// 24 instead of 36, but the index array size is 36, same as the number of
// vertices required to draw a cube.
GLfloat cubeVertices[] = { 0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,   // v0,v1,v2,v3 (front)
	                    0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,   0.5, 0.5,-0.5,   // v0,v3,v4,v5 (right)
	                    0.5, 0.5, 0.5,   0.5, 0.5,-0.5,  -0.5, 0.5,-0.5,  -0.5, 0.5, 0.5,   // v0,v5,v6,v1 (top)
	                   -0.5, 0.5, 0.5,  -0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,   // v1,v6,v7,v2 (left)
	                   -0.5,-0.5,-0.5,   0.5,-0.5,-0.5,   0.5,-0.5, 0.5,  -0.5,-0.5, 0.5,   // v7,v4,v3,v2 (bottom)
	                    0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5, 0.5,-0.5,   0.5, 0.5,-0.5 }; // v4,v7,v6,v5 (back)

// normal array
GLfloat cubeNormals[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
	                    1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
	                    0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
	                   -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
	                    0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
	                    0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

// color array
GLfloat cubeColors[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
	                    1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
	                    1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
	                    1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
	                    0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
	                    0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)

// index array of vertex array for glDrawElements() & glDrawRangeElement()
GLushort indices[]  = { 0, 1, 2,   2, 3, 0,      // front
	                   4, 5, 6,   6, 7, 4,      // right
	                   8, 9,10,  10,11, 8,      // top
	                  12,13,14,  14,15,12,      // left
	                  16,17,18,  18,19,16,      // bottom
	                  20,21,22,  22,23,20 };    // back

// draw cube at bottom-left corner with glDrawElements
// The main advantage of glDrawElements() over glDrawArray() is that
// glDrawElements() allows hopping around the vertex array with the associated
// index values.
// In a cube, the number of vertex data in the vertex array can be reduced to
// 24 vertices for glDrawElements().
// Note that you need an additional array (index array) to store how to traverse
// the vertext data. For a cube, we need 36 entries in the index array.
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::drawQuad3D(Vect3D coord, Vect3D scale, Vect3D rotation, int flags) {
	// enble and specify pointers to vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, cubeNormals);
    glColorPointer(3, GL_FLOAT, 0, cubeColors);
    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);

	glPushMatrix();

	Vect3D u(0, 1, 0);
	Vect3D v = rotation;
	// Quad3D will be 0, 1, 0
	Vect3D n = Vect3D::cross(u, v);

	// std::cout << a << " " << b << ": u" << u << " v" << v << " n" << n << "\n";

	float tripleProduct = Vect3D::dot(u, Vect3D::cross(v, n));
	float rotRads = std::acos(Vect3D::dot(u, v)/(u.magnitude()*v.magnitude()));
	if (tripleProduct < 0) rotRads = 2*M_PI - rotRads;

	// NOTE: the following tranformations are applied in reverse
	glTranslatef(coord.x, coord.y, coord.z);	// OG 2, 2, 0
	glRotatef(360*rotRads/(2*M_PI), n.x, n.y, n.z);
	glScalef(scale.x, scale.y, scale.z);	//

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);	// disable vertex arrays
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void GLWrapper::drawLineSeg3D(Vect3D a, Vect3D b, float thic) {
	// enble and specify pointers to vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, cubeNormals);
    glColorPointer(3, GL_FLOAT, 0, cubeColors);
    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);

	glPushMatrix();
	Vect3D u(0, 1, 0);
	Vect3D v = b-a;
	// Quad3D will be 0, 1, 0
	Vect3D n = Vect3D::cross(u, v);

	// std::cout << a << " " << b << ": u" << u << " v" << v << " n" << n << "\n";

	float tripleProduct = Vect3D::dot(u, Vect3D::cross(v, n));
	float rotRads = std::acos(Vect3D::dot(u, v)/(u.magnitude()*v.magnitude()));
	if (tripleProduct < 0) rotRads = 2*M_PI - rotRads;
	// std::cout << "tripProd: "<< tripleProduct << "\n";
	// std::cout << "rotRads: "<< rotRads << " | " << 360*rotRads/(2*M_PI) << "\n";


	// NOTE: the following tranformations are applied in reverse
	Vect3D mid = (a+b)/2;
	glTranslatef(mid.x, mid.y, mid.z);	// OG 2, 2, 0
	glRotatef(360*rotRads/(2*M_PI), n.x, n.y, n.z);
	glScalef(thic, v.magnitude(), thic);	//

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);	// disable vertex arrays
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

GLWrapper::GLWrapper(int argc, char **argv) {
	// init global vars
	initSharedMem();

	// init GLUT and GL
	initGLUT(argc, argv);
	initGL();


	// check max of elements vertices and elements indices that your video card supports
	// Use these values to determine the range of glDrawRangeElements()
	// The constants are defined in glext.h
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertices);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndices);
	#ifdef _WIN32
	// get function pointer to glDrawRangeElements
		glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)wglGetProcAddress("glDrawRangeElements");
	#endif
}

// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int GLWrapper::initGLUT(int argc, char **argv) {
	// GLUT stuff for windowing
	// initialization openGL window.
	// it is called before any other GLUT routine
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);	// display mode

	glutInitWindowSize(screenWidth, screenHeight);	// window size

	glutInitWindowPosition(100, 100);				// window location

	// finally, create a window with openGL context
	// Window will not displayed until glutMainLoop() is called
	// it returns a unique ID
	int handle = glutCreateWindow(argv[0]);		// param is the title of window

	// register GLUT callback functions
	glutDisplayFunc(displayCB);
	glutTimerFunc(33, timerCB, 33);				// redraw only every given millisec
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);

	return handle;
}



// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::initGL() {
	glShadeModel(GL_SMOOTH);					// shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);		// 4-byte pixel alignment

	// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0, 0, 0, 0);					// background color
	glClearStencil(0);							// clear stencil buffer
	glClearDepth(1.0f);							// 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	initLights();
}



// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);		// need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);			// set text color
	glRasterPos2i(x, y);		// place text position

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



// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::drawString3D(const char *str, float pos[3], float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);		// need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);			// set text color
	glRasterPos3fv(pos);		// place text position

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



// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool GLWrapper::initSharedMem() {
	std::srand(glutGet(GLUT_ELAPSED_TIME));
	return true;
}



// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::clearSharedMem() {
}



// initialize lights
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::initLights() {
	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};	// ambient light
	GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};	// diffuse light
	GLfloat lightKs[] = {1, 1, 1, 1};			// specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light
	float lightPos[4] = {0, 0, 20, 1}; // positional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);						// MUST enable each light source after configuration
}



// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

void GLWrapper::setCameraDefaults() {
	glTranslatef(0, 0, -cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);	// pitch
	glRotatef(cameraAngleY, 0, 1, 0);	// heading
}

void GLWrapper::clearBuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void GLWrapper::pushMatrix() {
	glPushMatrix();
}
void GLWrapper::popMatrix() {
	glPopMatrix();
}
void GLWrapper::swapBuffers() {
	glutSwapBuffers();
}

// display info messages
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::showInfo() {
	// backup current model-view matrix
	glPushMatrix();						// save current modelview matrix
	glLoadIdentity();					// reset modelview matrix

	// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);		// switch to projection matrix
	glPushMatrix();						// save current projection matrix
	glLoadIdentity();					// reset projection matrix
	gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

	float color[4] = {1, 1, 1, 1};

	stringstream ss;
	ss << std::fixed << std::setprecision(3);

	ss << "Max Elements Vertices: " << maxVertices << ends;
	drawString(ss.str().c_str(), 1, screenHeight-TEXT_HEIGHT, color, font);
	ss.str("");

	ss << "Max Elements Indices: " << maxIndices << ends;
	drawString(ss.str().c_str(), 1, screenHeight-(2*TEXT_HEIGHT), color, font);
	ss.str("");

	// unset floating format
	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

	// restore projection matrix
	glPopMatrix();					 // restore to previous projection matrix

	// restore modelview matrix
	glMatrixMode(GL_MODELVIEW);		 // switch to modelview matrix
	glPopMatrix();					 // restore to previous modelview matrix
}

// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
// void toOrtho() {
// 	// set viewport to be the entire window
// 	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
//
// 	// set orthographic viewing frustum
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
//
// 	// switch to modelview matrix in order to set scene
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();
// }

// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void GLWrapper::toPerspective() {
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, (float)(screenWidth)/screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//=============================================================================
// CALLBACKS
//=============================================================================

int boardData[10][22];
int last = -1;
int fadeDelay = 30; // number of frames to exist before fading

void GLWrapper::addDisplayCB(void (*func)(void)) {
	myDisplayCB = func;
}
void GLWrapper::displayCB() {
	// debugOut("\nin GLWrapper::displayCB()\n");
	(*myDisplayCB)();
}


void GLWrapper::reshapeCB(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	toPerspective();
}


void GLWrapper::timerCB(int millisec) {
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}


void GLWrapper::keyboardCB(unsigned char key, int x, int y) {
	switch(key)	{
		case 27: // ESCAPE
			clearSharedMem();
			exit(0);
			break;

		case 'd': // switch rendering modes (fill -> wire -> point)
		case 'D':
			drawMode = ++drawMode % 3;
			if(drawMode == 0)		 // fill mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}
			else if(drawMode == 1)	// wireframe mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			else					// point mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			break;

		default:
			;
	}
}


void GLWrapper::mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if(state == GLUT_UP)
			mouseLeftDown = false;
	}

	else if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if(state == GLUT_UP)
			mouseRightDown = false;
	}

	else if(button == GLUT_MIDDLE_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			mouseMiddleDown = true;
		}
		else if(state == GLUT_UP)
			mouseMiddleDown = false;
	}
}


void GLWrapper::mouseMotionCB(int x, int y)
{
	if(mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if(mouseRightDown)
	{
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
}
