#pragma once
#include <cmath>
#include <iostream>

using std::clog;

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
