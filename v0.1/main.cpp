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

StateMachine stateMachine;
GLWrapper *myGL;

class TetrisState: public AbstractState {
private:
	int boardData[10][22];
	int lastUpdateTime;
	int fadeDelay = 0.5f * FPS;
public:
	TetrisState() {
		// setup play data

		// set board to 0
		for (auto &col: boardData) {
			for (auto &row: col) {
				row = 0;
			}
		}
	}
	void enterState(StateMachine *sm) {
		// resume play

		lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
	}

	void updateState(StateMachine *sm) {
		debugOut("updating myTetris state\n");
		// update physics
		int now = glutGet(GLUT_ELAPSED_TIME);
		// cout << now-last << "\n";
		if (now-lastUpdateTime >= 1000.0f/FPS) {
			lastUpdateTime = now;
			for (int i=0; i<10; i++) {
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
			}
		}

		// render
		//=============================================================================

		// save the initial ModelView matrix before modifying ModelView matrix
		myGL->pushMatrix();

		// tramsform camera
		myGL->setCameraDefaults();

		float mx, my, mz;
		mx = my = mz = 0.80f; // scales cubes by factor

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
				if (boardData[i][j]) myGL->drawQuad3D(Vect3D(ii, jj, 0), Vect3D(mx, my, mz), Vect3D(0, 1, 0), boardData[i][j]>fadeDelay/2);
				myGL->drawLineSeg3D(Vect3D(lowx, jj-0.5f, 0), Vect3D(highx, jj-0.5f, 0), linethickness);
			}
			myGL->drawLineSeg3D(Vect3D(lowx, highy, 0), Vect3D(highx, highy, 0), linethickness);
			myGL->drawLineSeg3D(Vect3D(ii-0.5f, lowy, 0), Vect3D(ii-0.5f, highy, 0), linethickness);
		}

		myGL->drawLineSeg3D(Vect3D(highx, lowy, 0), Vect3D(highx, highy, 0), linethickness);

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
