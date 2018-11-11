#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include <cmath>
#include <utility>
#include <vector>
#include <map>

using std::pair;
using std::vector;
using std::map;

#include "datastruct.h"
#include "glwrapper.h"
#include "statemanager.h"
#include "glext.h"
#include "gameobjects.h"

typedef pair<int, int> pii;

StateMachine stateMachine;
GLWrapper *myGL;

class GameBoard {
// TODO: make this into grid drawer
private:
	QuadObject cubes[BOARD_WIDTH][BOARD_HEIGHTMEM];
	QuadBlock block;
public:
	GameBoard() : block(cubes) {
		// for each CubeObject in cubes
		for (auto &col: cubes) {
			for (auto &cube: col) {
				int x = &col-cubes, y = &cube-col;
				cube.setPosition(Vect3D(x, y, 0));
				cube.setState(0);
			}
		}
		// cubes[0][0].setState(1);
	}
	//
	QuadObject getLineSeg(Vect3D a, Vect3D b, float thic) {
		QuadObject gridAid; // temporary object to set up and return
		gridAid.setState(1);
		// ===== find rotation needed to make line =====
		Vect3D u(0, 1, 0);
		Vect3D v = b-a;
		// Quad3D will be 0, 1, 0
		Vect3D n = Vect3D::cross(u, v);

		float tripleProduct = Vect3D::dot(u, Vect3D::cross(v, n));
		float rotRads = std::acos(Vect3D::dot(u, v)/(u.magnitude()*v.magnitude()));
		if (tripleProduct < 0) rotRads = 2*M_PI - rotRads;
		float rotDegs = 360*rotRads/(2*M_PI);
		// glRotatef(rotDegs, n);
		gridAid.setRotation(rotDegs, n);

		// ===== find mid point =====
		Vect3D mid = (a+b)/2;
		// glTranslatef(mid.x, mid.y, mid.z);
		gridAid.setPosition(mid);

		// ===== scale quadrilateral to be line linethickness =====
		// glScalef(thic, v.magnitude(), thic);
		gridAid.setScale(Vect3D(thic, v.magnitude(), thic));
		return gridAid;
	}
	void render() {
		// for each CubeObject in cubes
		for (auto &col: cubes) {
			for (auto &cube: col) {
				cube.render();
			}
		}
		// draw controlled block
		block.render();

		// draw grid
		float linethickness = 0.05f;
		float lowx = -0.5f;
		float highx = BOARD_WIDTH-0.5f;
		float lowy = -0.5f;
		float highy = BOARD_HEIGHT-0.5f - INVIS_ROWS;

		for (int i=0; i<BOARD_WIDTH; i++) {
			for (int j=0; j<BOARD_HEIGHT-INVIS_ROWS; j++) {
				getLineSeg(Vect3D(lowx, j-0.5f, 0), Vect3D(highx, j-0.5f, 0), linethickness).render();
			}
			getLineSeg(Vect3D(lowx, highy, 0), Vect3D(highx, highy, 0), linethickness).render();
			getLineSeg(Vect3D(i-0.5f, lowy, 0), Vect3D(i-0.5f, highy, 0), linethickness).render();
		}
		getLineSeg(Vect3D(highx, lowy, 0), Vect3D(highx, highy, 0), linethickness).render();
	}
	void update() {
		block.update();

		for (int j=0; j<BOARD_HEIGHT; j++) {
			bool full = true;
			for (int i=0; i<BOARD_WIDTH; i++) {
				if (!cubes[i][j].getState()) full = 0;
			}
			if (full) {
				for (int i=0; i<BOARD_WIDTH; i++) {
					if(cubes[i][j].getState() < fadeDelay) cubes[i][j].setState(cubes[i][j].getState()+1);
					else {
						cubes[i][j].setState(0);
						// shift the column above 1 down
						for (int jj=j; jj<BOARD_HEIGHT-1; jj++) {
							cubes[i][jj].setState(cubes[i][jj+1].getState());
							cubes[i][jj+1].setState(0);
						}
					}
				}
			}
		}
		bool top = false;
		for (int i=0; i<BOARD_WIDTH; i++) {
			if (cubes[i][BOARD_HEIGHT-1].getState()) top = true;
		}
		if (top) {
			// TODO: GAME OVER
		}
	}
	void moveBlock(int dx) {
		block.moveBlock(dx);
	}
	void rotateBlock(int dir) {
		block.rotateBlock(dir);
	}
	void dropBlock() {
		block.dropBlock();
	}
};

map<pair<int, int>, int> mapData;

class MapState: public AbstractState {
private:
	int lastUpdateTime;
	QuadObject cursorObj;
	Vect3D cursorPos;
public:
	MapState() {
		// setup play data
		cursorPos = Vect3D(0, 0, 0);
		cursorObj.setPosition(cursorPos);
		cursorObj.setScale(Vect3D(1, 1, 0.1f));
		cursorObj.setState(1); // non 0 to be rendered
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

			//gameBoard.update(); *****UPDATE HERE*****
			cursorObj.setPosition(cursorPos);
		}

		// render
		//=============================================================================

		// save the initial ModelView matrix before modifying ModelView matrix
		myGL->pushMatrix();

		// tramsform camera, TODO adjust this
		myGL->setCameraDefaults();

		// gameBoard.render(); *****RENDER HERE*****

		/*for (auto &kv : mapData) {
			int x = kv.first.first, y = kv.first.second;
			debugOut("key: (", x, ", ", y, ") val: ", kv.second, "\n");
			QuadObject qObj;
			qObj.setPosition(Vect3D(x, y, 0));
			qObj.setScale(Vect3D(1, 1, 0.1f));
			qObj.setState(kv.second); // non 0 to be rendered
			qObj.render();
		}*/

		cursorObj.render();


		myGL->showInfo();		// print max range of glDrawRangeElements

		myGL->popMatrix();
	}

	void keyPress(unsigned char key, int x, int y) {
		// debugOut(key);
		switch (key) {
			case 'w':
			case 'W':
				cursorPos.y++;
				break;
			case 's':
			case 'S':
				cursorPos.y--;
				break;
			case 'a':
			case 'A':
				cursorPos.x--;
				break;
			case 'd':
			case 'D':
				cursorPos.x++;
				break;
			case ' ':
				break;
		}
	}

	void exitState() {

	}
}myMap;

void myDisplayCB() {
	myGL->clearBuffer();
	stateMachine.update();
	myGL->swapBuffers();
}

void myKeyboardCB(unsigned char key, int x, int y) {
	stateMachine.keyPress(key, x, y);
}

int main(int argc, char **argv) {
	myGL = new GLWrapper(argc, argv);
	myGL->addDisplayCB(myDisplayCB);
	myGL->addKeyboardCB(myKeyboardCB);

	mapData[pii(1, 1)] = 1;
	mapData[pii(3, 3)] = 1;
	stateMachine.enterState(&myMap);

	// the last GLUT call (LOOP)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never return main().
	glutMainLoop(); // Start GLUT event-processing loop

	return 0;
}
