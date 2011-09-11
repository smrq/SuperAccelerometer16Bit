#ifndef __VECTOR3F_H__
#define __VECTOR_H__

#include <stdio.h>
#include <math.h>

struct Vector3f {
	public:
		// Fields
		float x;
		float y;
		float z;

		// Constructors
		Vector3f()
			: x(0.0f), y(0.0f), z(0.0f) {}
		Vector3f(const float x, const float y, const float z)
			: x(x), y(y), z(z) {}
		Vector3f(const Vector3f &source)
			: x(source.x), y(source.y), z(source.z) {}

		// Operator overloads
		Vector3f& operator=(const Vector3f &rhs) {
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}
		Vector3f operator+(const Vector3f &rhs) const { return Vector3f(*this) += rhs; }
		Vector3f operator-(const Vector3f &rhs) const { return Vector3f(*this) -= rhs; }
		Vector3f operator*(const float &scalar) const { return Vector3f(*this) *= scalar; }
		Vector3f operator/(const float &scalar) const { return Vector3f(*this) /= scalar; }
		Vector3f & operator+=(const Vector3f &rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		Vector3f & operator-=(const Vector3f &rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		Vector3f & operator*=(const float &scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}
		Vector3f & operator/=(const float &scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

		// Methods
		float magnitude() { return sqrt(x*x + y*y + z*z); }
};

#endif
