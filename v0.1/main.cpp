#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include <cmath>
#include <vector>

using std::vector;

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

const int cubeCount = 36;

StateMachine stateMachine;
GLWrapper *myGL;

class QuadObject: private GameObject {
private:
	int state; // TODO convert to enum
public:
	QuadObject() {
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
	int getState() {
		return state;
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

typedef vector<vector<bool>> Grid;

class Blocks {
public:
	Grid grid;
	Blocks() {
	}
	Blocks(vector<vector<bool>> g) {
		grid = g;
	}
};
Blocks TileSet[][4] = {
	{
		Blocks({{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0}}),

		Blocks({{0, 0, 0, 0},
				{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0}}),

		Blocks({{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0}}),

		Blocks({{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0},
				{0, 0, 0, 0}})
	},
	{
		Blocks({{0, 1, 1},
				{0, 1, 0},
				{0, 1, 0}}),

		Blocks({{0, 0, 0},
				{1, 1, 1},
				{0, 0, 1}}),

		Blocks({{0, 1, 0},
				{0, 1, 0},
				{1, 1, 0}}),

		Blocks({{1, 0, 0},
				{1, 1, 1},
				{0, 0, 0}}),
	},
	{
		Blocks({{0, 1, 0},
				{0, 1, 0},
				{0, 1, 1}}),

		Blocks({{0, 0, 0},
				{1, 1, 1},
				{1, 0, 0}}),

		Blocks({{1, 1, 0},
				{0, 1, 0},
				{0, 1, 0}}),

		Blocks({{0, 0, 1},
				{1, 1, 1},
				{0, 0, 0}}),
	},
	{
		Blocks({{0, 0, 0},
				{0, 1, 1},
				{0, 1, 1},
				{0, 0, 0}}),

		Blocks({{0, 0, 0},
				{0, 1, 1},
				{0, 1, 1},
				{0, 0, 0}}),

		Blocks({{0, 0, 0},
				{0, 1, 1},
				{0, 1, 1},
				{0, 0, 0}}),

		Blocks({{0, 0, 0},
				{0, 1, 1},
				{0, 1, 1},
				{0, 0, 0}}),
	},
	{
		Blocks({{0, 1, 0},
				{0, 1, 1},
				{0, 0, 1}}),

		Blocks({{0, 0, 0},
				{0, 1, 1},
				{1, 1, 0}}),

		Blocks({{1, 0, 0},
				{1, 1, 0},
				{0, 1, 0}}),

		Blocks({{0, 1, 1},
				{1, 1, 0},
				{0, 0, 0}}),
	},
	{
		Blocks({{0, 1, 0},
				{0, 1, 1},
				{0, 1, 0}}),

		Blocks({{0, 0, 0},
				{1, 1, 1},
				{0, 1, 0}}),

		Blocks({{0, 1, 0},
				{1, 1, 0},
				{0, 1, 0}}),

		Blocks({{0, 1, 0},
				{1, 1, 1},
				{0, 0, 0}}),
	},
	{
		Blocks({{0, 0, 1},
				{0, 1, 1},
				{0, 1, 0}}),

		Blocks({{0, 0, 0},
				{1, 1, 0},
				{0, 1, 1}}),

		Blocks({{0, 1, 0},
				{1, 1, 0},
				{1, 0, 0}}),

		Blocks({{1, 1, 0},
				{0, 1, 1},
				{0, 0, 0}}),
	}
};

class QuadBlock {
private:
	int type, rot;
	int offx, offy;
	int state; // TODO convert to enum
public:
	void makeNewBlock() {
		type = rand()%7;
		rot = 0;
		offx = BOARD_WIDTH/2 - 2; // centre block slightly
		offy = BOARD_HEIGHT - 2; // spawn further on board rather than off
	}
	QuadBlock() {
		makeNewBlock();
		state = 1;
	}
	void render() {
		if (state>0) {
			// render all blocks
			QuadObject renderAid;
			renderAid.setState(1);
			Grid &myGrid = TileSet[type][rot].grid;
			for (int w = 0; w<myGrid.size(); w++) {
				for (int h = 0; h<myGrid[w].size(); h++) {
					if (myGrid[w][h]) {
						renderAid.setPosition(Vect3D(offx + w, offy + h, 0));
						if (offy+h < BOARD_HEIGHT-INVIS_ROWS) { // not off grid
							renderAid.render();
						}
					}
				}
			}
		}
	}
	void update(QuadObject cubes[][BOARD_HEIGHTMEM]) {
		debugOut("curpos: (", offx, ", ", offy, ")\n");
		bool bottom = false;
		Grid &myGrid = TileSet[type][rot].grid;
		for (int w = 0; w<myGrid.size(); w++) {
			for (int h = 0; h<myGrid[w].size(); h++) {
				if (myGrid[w][h]) {
					int chkx = w + offx, chky = h + offy - 1;
					if (chky < 0 || cubes[chkx][chky].getState()) {
						bottom = true;
					}
				}
			}
		}
		if (bottom) {
			for (int w = 0; w<myGrid.size(); w++) {
				for (int h = 0; h<myGrid[w].size(); h++) {
					if (myGrid[w][h]) {
						if (h+offy < BOARD_HEIGHT-INVIS_ROWS) { // not off screen
							cubes[w+offx][h+offy].setState(1);
						}
					}
				}
			}
			// reset to new block
			makeNewBlock();
		}
		else { // can move down
			offy--;
		}
	}
	void setState(int s) {
		state = s;
	}
	int getState() {
		return state;
	}
};

class GameBoard {
private:
	QuadObject cubes[BOARD_WIDTH][BOARD_HEIGHTMEM];
	QuadBlock block;
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
		block.update(cubes);

		for (int j=0; j<BOARD_HEIGHT; j++) {
			bool full = true;
			for (int i=0; i<BOARD_WIDTH; i++) {
				if (!cubes[i][j].getState()) full = 0;
			}
			if (full) {
				for (int i=0; i<BOARD_WIDTH; i++) {
					if(cubes[i][j].getState() < fadeDelay) cubes[i][j].setState(cubes[i][j].getState()+1);
					else cubes[i][j].setState(0);
				}
				// TODO: shift board above 1 down
			}
			else break;
		}
		bool top = false;
		for (int i=0; i<BOARD_WIDTH; i++) {
			if (cubes[i][BOARD_HEIGHT-1].getState()) top = true;
		}
		if (top) {
			// TODO: GAME OVER
		}
	}
	void input() {
		
	}
};

class TetrisState: public AbstractState {
private:
	GameBoard gameBoard;
	int lastUpdateTime;
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
