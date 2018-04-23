#include "datastruct.h"

Vect3D::Vect3D() : x(0), y(0), z(0) {
}

Vect3D::Vect3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {
}

Vect3D Vect3D::operator+(const Vect3D &v) {
	return Vect3D(x+v.x, y+v.y, z+v.z);
}

Vect3D Vect3D::operator-(const Vect3D &v) {
	return Vect3D(x-v.x, y-v.y, z-v.z);
}

Vect3D Vect3D::operator*(const float &s) {
	return Vect3D(x*s, y*s, z*s);
}

Vect3D Vect3D::operator/(const float &s) {
	return Vect3D(x/s, y/s, z/s);
}

float Vect3D::magnitude() {
	return std::sqrt(x*x + y*y + z*z);
}

Vect3D Vect3D::cross(const Vect3D &u, const Vect3D &v) {
	return Vect3D(	u.y*v.z - u.z*v.y, // = x
					u.z*v.x - u.x*v.z, // = y
					u.x*v.y - u.y*v.x);	// = z
}

float Vect3D::dot(const Vect3D &u, const Vect3D &v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

std::ostream& operator<<(std::ostream& os, const Vect3D &u) {
	os << "(" << u.x << ", " << u.y << ", " << u.z << ")";
	return os;
}
