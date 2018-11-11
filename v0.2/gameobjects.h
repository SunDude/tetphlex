#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "datastruct.h"
#include "glwrapper.h"

typedef vector<vector<bool>> Grid;

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
static GLfloat cubeVertices[] = { 0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,   // v0,v1,v2,v3 (front)
	                    0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,   0.5, 0.5,-0.5,   // v0,v3,v4,v5 (right)
	                    0.5, 0.5, 0.5,   0.5, 0.5,-0.5,  -0.5, 0.5,-0.5,  -0.5, 0.5, 0.5,   // v0,v5,v6,v1 (top)
	                   -0.5, 0.5, 0.5,  -0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,   // v1,v6,v7,v2 (left)
	                   -0.5,-0.5,-0.5,   0.5,-0.5,-0.5,   0.5,-0.5, 0.5,  -0.5,-0.5, 0.5,   // v7,v4,v3,v2 (bottom)
	                    0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5, 0.5,-0.5,   0.5, 0.5,-0.5 }; // v4,v7,v6,v5 (back)

// normal array
static GLfloat cubeNormals[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
	                    1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
	                    0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
	                   -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
	                    0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
	                    0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

// color array
static GLfloat cubeColors[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
	                    1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
	                    1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
	                    1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
	                    0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
	                    0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)

// index array of vertex array for glDrawElements() & glDrawRangeElement()
static GLushort indices[]  = { 0, 1, 2,   2, 3, 0,      // front
	                   4, 5, 6,   6, 7, 4,      // right
	                   8, 9,10,  10,11, 8,      // top
	                  12,13,14,  14,15,12,      // left
	                  16,17,18,  18,19,16,      // bottom
	                  20,21,22,  22,23,20 };    // back

const int cubeCount = 36;

class RenderObject {
public:
	Polygon3D data;
	// Polygon3D(GLfloat *vert, GLfloat *norm, GLfloat *color, GLushort *idx, int c)

	RenderObject() {
	}

	void render(Transformation &trans) {
		GLWrapper::drawPoly3D(data, trans);
	}
};

class GameObject {
private:
protected:
	Transformation transformation;
	RenderObject *renderObjects;
public:
	GameObject() {
		renderObjects = NULL;
		transformation.setIdentity();
	}
	virtual void render() {
		renderObjects->render(transformation);
	}
	virtual void update() {

	}
	virtual void setPosition(Vect3D newPos) {
		transformation.setPosition(newPos);
	}
	virtual void setRotation(float deg, Vect3D newRot) {
		transformation.setRotation(deg, newRot);
	}
	virtual void setScale(Vect3D newScale) {
		transformation.setScale(newScale);
	}
};

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

class Blocks {
public:
	Grid grid;
	Blocks() {
	}
	Blocks(vector<vector<bool>> g) {
		grid = g;
	}
};
static Blocks TileSet[][4] = {
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

// wallKickDelta[rotation direction][block type/2][current][tests/5][x/y]
int wallKickDelta[2][2][NUM_ROTATIONS][5][2] = {
	{ // LEFT
		{ // I block
			{ // 0
				{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}
			},
			{ // R
				{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}
			},
			{ // 2
				{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}
			},
			{ // L
				{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}
			}
		},
		{ // J, L, S, T, Z blocks
			{ // 0
				{0, 0}, {1, 0}, {1, -1}, {0, -2}, {1, -2}
			},
			{ // R
				{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}
			},
			{ // 2
				{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}
			},
			{ // L
				{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}
			}
		}
	},
	{ // RIGHT
		{ // I block
			{ // 0
				{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}
			},
			{ // R
				{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}
			},
			{ // 2
				{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}
			},
			{ // L
				{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}
			}
		},
		{ // J, L, S, T, Z blocks
			{ // 0
				{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}
			},
			{ // R
				{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}
			},
			{ // 2
				{0, 0}, {1, 0}, {1, -1}, {0, -2}, {1, -2}
			},
			{ // L
				{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}
			}
		}
	}
};

class QuadBlock {
private:
	int type, rot;
	int offx, offy;
	int state; // TODO convert to enum
	QuadObject (*board)[BOARD_HEIGHTMEM];
public:
	void makeNewBlock() {
		type = rand()%7;
		rot = 0;
		offx = BOARD_WIDTH/2 - 2; // centre block slightly
		offy = BOARD_HEIGHT - 2; // spawn further on board rather than off
	}
	QuadBlock(QuadObject cubes[][BOARD_HEIGHTMEM]) {
		makeNewBlock();
		state = 1;
		board = cubes;
	}
	bool testPosD(int dx, int dy, int r=-1) {
		Grid &testGrid = TileSet[type][r<0?rot:r].grid;
		// debugOut("rotation = ", r<0?rot:r, "\n");
		for (int w = 0; w<testGrid.size(); w++) {
			for (int h = 0; h<testGrid[w].size(); h++) {
				if (testGrid[w][h]) {
					int chkx = w + offx + dx, chky = h + offy + dy;
					if (chky < 0 || chky >= BOARD_HEIGHT || chkx < 0 || chkx >= BOARD_WIDTH) {
						// debugOut("hit boundary:(", w, ", ", h, ")\n");
						return false;
					}
					else if (board[chkx][chky].getState()) {
						// debugOut("hit block below\n");
						return false;
					}
				}
			}
		}
		return true;
	}
	void render() {
		Grid &myGrid = TileSet[type][rot].grid;
		if (state>0) {
			// render all blocks
			QuadObject renderAid;
			renderAid.setState(1);
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
	void update() {
		Grid &myGrid = TileSet[type][rot].grid;
		// debugOut("curpos: (", offx, ", ", offy, ")\n");
		bool bottom = false;

		if (testPosD(0, -1)) { // can move (0, -1) which is down one
			//debugOut("can move down\n");
			offy--;
		}
		else {
			//debugOut("=== AT BOTTOM ===\n");
			for (int w = 0; w<myGrid.size(); w++) {
				for (int h = 0; h<myGrid[w].size(); h++) {
					if (myGrid[w][h]) {
						if (h+offy < BOARD_HEIGHT-INVIS_ROWS) { // not off screen
							board[w+offx][h+offy].setState(1);
						}
					}
				}
			}
			// reset to new block
			makeNewBlock();
		}
	}
	void moveBlock(int dx) {
		if (testPosD(dx, 0)) {
			offx += dx;
		}
	}
	void rotateBlock(int dir) {
		// dir is [left:0, right:1]
		if (type == 3) { // is square block, do nothing
			return;
		}

		// convert direction to -1 or +1 then ensure it's in range and positive
		int newRot = ((rot + (dir*2 - 1) % NUM_ROTATIONS) + NUM_ROTATIONS) % NUM_ROTATIONS;

		// using aid from wallKickDelta[rotation direction][block type/2][current][tests/5][x/y]
		for (int i=0; i<5; i++) {
			int dx = wallKickDelta[dir][type%2][rot][i][0],
				dy = wallKickDelta[dir][type%2][rot][i][1];
			if (testPosD(dx, dy, newRot)) { // found valid rotation
				offx += dx;
				offy += dy;
				rot = newRot;
				break; // can stop looking for valid rotations
			}
		}
	}
	void dropBlock() {
		int last = 0;
		for (int i=0; i<BOARD_HEIGHT; i++) {
			if (testPosD(0, -i)) {
				last = -i;
			}
			else {
				break;
			}
		}
		offy += last;
	}
	void setState(int s) {
		state = s;
	}
	int getState() {
		return state;
	}
};
