#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include "datastruct.h"
#include "glwrapper.h"
#include "statemanager.h"
#include "glext.h"
#include "gameobjects.h"

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

int cubeCount = 36;

StateMachine stateMachine;
GLWrapper *myGL;

class CubeObject: private GameObject {
private:
	int state; // TODO convert to enum
public:
	CubeObject() {
		renderObjects = new RenderObject();
		renderObjects->data = Polygon3D(cubeVertices, cubeNormals, cubeColors, indices, cubeCount);;
		setScale(Vect3D(0.8f, 0.8f, 0.8f));
	}
	void render() {
		if (state>0) {
			renderObjects->render(transformation);
		}
	}
	void update() {

	}
	void setState(int s) {
		state = s;
	}
	void setPosition(Vect3D newPos) {
		GameObject::setPosition(newPos);
	}
	void setRotation(float deg, Vect3D newRot) {
		GameObject::setRotation(deg, newRot);
	}
	void setScale(Vect3D newScale) {
		GameObject::setScale(newScale);
	}
};

class GameBoard {
private:
	CubeObject cubes[BOARD_WIDTH][BOARD_HEIGHT];
public:
	GameBoard() {
		// for each CubeObject in cubes
		for (auto &col: cubes) {
			for (auto &cube: col) {
				int x = &col-cubes, y = &cube-col;
				cube.setPosition(Vect3D(x, y, 0));
				cube.setState(0);
			}
		}
		cubes[0][0].setState(1);
	}
	void render() {
		// for each CubeObject in cubes
		for (auto &col: cubes) {
			for (auto &cube: col) {
				cube.render();
			}
		}
	}
	void update() {
		// TODO update logic
		/*for (int i=0; i<10; i++) {
			for (int j=1; j<22; j++) {
				if (boardData[i][j] && !boardData[i][j-1]) {
					boardData[i][j-1] = 1;
					boardData[i][j] = 0;
				}
			}
		}
		for (int j=0; j<22; j++) {
			bool full = true;
			for (int i=0; i<10; i++) {
				if (!boardData[i][j]) full = 0;
			}
			if (full) {
				for (int i=0; i<10; i++) {
					if(boardData[i][j] < fadeDelay) boardData[i][j]++;
					else boardData[i][j] = 0;
				}
			}
			else break;
		}
		bool top = false;
		for (int i=0; i<10; i++) {
			if (boardData[i][21]) top = true;
		}
		if (top) {
			for (int i=0; i<10; i++) {
				for (int j=0; j<22; j++) {
					if(boardData[i][j]) boardData[i][j] = min(std::rand()%3, 1);
				}
			}
		}

		if (std::rand()%5) {
			boardData[std::rand()%10][21] = true;
		}*/
	}
};

class TetrisState: public AbstractState {
private:
	GameBoard gameBoard;
	int lastUpdateTime;
	int fadeDelay = 0.5f * FPS;
public:
	TetrisState() {
		// setup play data

	}
	void enterState(StateMachine *sm) {
		// resume play

		lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
	}

	void updateState(StateMachine *sm) {
		// update physics
		int now = glutGet(GLUT_ELAPSED_TIME);
		// cout << now-last << "\n";
		if (now-lastUpdateTime >= 1000.0f/FPS) {
			lastUpdateTime = now;
			gameBoard.update();
		}

		// render
		//=============================================================================

		// save the initial ModelView matrix before modifying ModelView matrix
		myGL->pushMatrix();

		// tramsform camera, TODO adjust this
		myGL->setCameraDefaults();

		// draw grid
		float linethickness = 0.05f;
		float xoff = -5;
		float yoff = -11;
		float lowx = -5.5f;
		float highx = 4.5f;
		float lowy = -11.5f;
		float highy = 8.5f;

		for (int i=0; i<10; i++) {
			float ii = i+xoff;
			for (int j=0; j<20; j++) {
				float jj = j+yoff;
				//myGL->drawLineSeg3D(Vect3D(lowx, jj-0.5f, 0), Vect3D(highx, jj-0.5f, 0), linethickness);
			}
			//myGL->drawLineSeg3D(Vect3D(lowx, highy, 0), Vect3D(highx, highy, 0), linethickness);
			//myGL->drawLineSeg3D(Vect3D(ii-0.5f, lowy, 0), Vect3D(ii-0.5f, highy, 0), linethickness);
		}
		//myGL->drawLineSeg3D(Vect3D(highx, lowy, 0), Vect3D(highx, highy, 0), linethickness);

		gameBoard.render();

		myGL->showInfo();		// print max range of glDrawRangeElements

		myGL->popMatrix();
	}

	void exitState() {

	}
}myTetris;

void myDisplayCB() {
	myGL->clearBuffer();
	stateMachine.update();
	myGL->swapBuffers();
}

int main(int argc, char **argv) {
	myGL = new GLWrapper(argc, argv);
	myGL->addDisplayCB(myDisplayCB);

	stateMachine.changeState(&myTetris);

	// the last GLUT call (LOOP)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never return main().
	glutMainLoop(); // Start GLUT event-processing loop

	return 0;
}
