#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <cmath>
#include <iostream>

using std::clog;
using glm::mat4;
using glm::vec3;
using glm::vec4;

#define DEBUG true

const float FPS = 60;

template<typename T>
T min(T a, T b) {
	if (b<a) return b;
	else return a;
}

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


class Vect3D {
private:
public:
		float x, y, z;

		Vect3D();
		Vect3D(float _x, float _y, float _z);

		Vect3D operator+(const Vect3D &v);
		Vect3D operator-(const Vect3D &v);
		Vect3D operator*(const float &s);
		Vect3D operator/(const float &s);

		float magnitude();
		vec3 toVec3() {
			return vec3(x, y, z);
		}

		static Vect3D cross(const Vect3D &u, const Vect3D &v);
		static float dot(const Vect3D &u, const Vect3D &v);

		friend std::ostream& operator<<(std::ostream& os, const Vect3D &u);
};

// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

class AbstractState;

class StateMachine {
private:
	AbstractState *currentState; // change to stack
public:
	StateMachine();
	void changeState(AbstractState *newState);
	AbstractState* getCurrentState();
	void update();
};

class AbstractState {
private:
public:
	virtual void enterState(StateMachine *sm);
	virtual void updateState(StateMachine *sm);
	virtual void exitState();
};

class Polygon3D {
public:
	GLfloat *vertices, *normals, *colors;
	GLushort *indices;
	int count;
	Polygon3D() {
		vertices = normals = colors = NULL;
		indices = NULL;
		count = 0;
	}
};

class Transformation {
public:
	Vect3D position, rotation, scale;
	float degrees;
	void setIdentity() {
		position = rotation = scale = Vect3D(0, 0, 0);
		degrees = 0;
	}
	mat4 getTransformation() {
		mat4 transformation = mat4(1.0f);
		transformation = glm::translate(transformation, position.toVec3());
		transformation = glm::scale(transformation, scale.toVec3());
		transformation = glm::rotate(transformation, degrees, rotation.toVec3());
		return transformation;
	}
};
