#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "datastruct.h"
#include "glwrapper.h"

class RenderObject {
public:
	Polygon3D data;

	RenderObject() {
	}

	void render(Transformation &trans) {
		GLWrapper::drawPoly3D(data, trans);
	}
};

class GameObject {
private:
	RenderObject renderObject;
	Transformation transformation;
public:
	GameObject() {
		transformation.setIdentity();
	}
	void render() {
		renderObject.render(transformation);
	}
};
