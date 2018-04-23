#include<GL/gl.h>
#include "drawgeometry.h"

static const GLdouble cubeVertexData[] = { 
	1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
	-1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

	1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
	1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

	1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
	-1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

	-1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
	-1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

	-1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
	1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

	1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
	-1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4

void drawGeo::getCubeVertex(GLdouble *out, GLdouble x, GLdouble y, GLdouble z, GLdouble length) {
	if (!out) {
		GLdouble *out = new GLdouble(12*3*3);
	}
	for (int i=0; i<12*3; i++) {
		out[3*i + 0] = cubeVertexData[3*i + 0]*length + x; // x
		out[3*i + 1] = cubeVertexData[3*i + 1]*length + y; // x
		out[3*i + 2] = cubeVertexData[3*i + 2]*length + z; // x
	}
}

