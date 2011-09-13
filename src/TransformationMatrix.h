#ifndef __TRANSFORMATIONMATRIX_H__
#define __TRANSFORMATIONMATRIX_H__

#include <cstdio>
#include <cmath>

#include "PDL.h"

class TransformationMatrix
{
	private:
		float matrix[4][4];

	public:
		// Constructor
		TransformationMatrix() { identityMatrix(); }

		// Accessor
		const float *getRawMatrix() { return &matrix[0][0]; }

		// Projections
		void identityMatrix();
		void perspectiveMatrix(const float height, const float width, const float fov, const float near, const float far);
		void orthographicMatrix(const float left, const float right, const float top, const float bottom, const float near, const float far);

		// Transformations
		void translate(float x, float y, float z = 0.0f);
		void rotateZ(float angle);
		void scale(float scalar);
		void scale(float sX, float sY, float sZ = 1.0f);
};

#endif
