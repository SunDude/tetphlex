#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "datastruct.h"
#include "glwrapper.h"

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
