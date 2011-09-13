#include "TransformationMatrix.h"

///////////////////////////////////////////////////////////////////////////////
// Projections

void TransformationMatrix::identityMatrix()
{
    memset(matrix, 0, sizeof(matrix));

	matrix[0][0] = 1.0f;
	matrix[1][1] = 1.0f;
	matrix[2][2] = 1.0f;
	matrix[3][3] = 1.0f;
}

void TransformationMatrix::perspectiveMatrix(const float height, const float width, const float fov, const float near, const float far)
{
    memset(matrix, 0, sizeof(matrix));

    matrix[0][0] = 1.0f / tanf(fov * 3.1415926535f / 360.0f);
    matrix[1][1] = matrix[0][0] / (height / width);
    matrix[2][2] = -(far + near) / (far - near);
    matrix[2][3] = -1.0f;
    matrix[3][2] = -2.0f * far * near / (far - near);
}

void TransformationMatrix::orthographicMatrix(const float left, const float right, const float top, const float bottom, const float near, const float far)
{
    memset(matrix, 0, sizeof(matrix));

	matrix[0][0] = 2.0f / (right - left);
	matrix[1][1] = 2.0f / (top - bottom);
	matrix[2][2] = -2.0f / (far - near);
	matrix[3][0] = -(right + left)/(right - left);
	matrix[3][1] = -(top + bottom)/(top - bottom);
	matrix[3][2] = -(far + near)/(far - near);
	matrix[3][3] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Transformations

void TransformationMatrix::translate(float x, float y, float z)
{
	matrix[0][0] += x * matrix[0][3];
	matrix[0][1] += y * matrix[0][3];
	matrix[0][2] += z * matrix[0][3];

	matrix[1][0] += x * matrix[1][3];
	matrix[1][1] += y * matrix[1][3];
	matrix[1][2] += z * matrix[1][3];

	matrix[2][0] += x * matrix[2][3];
	matrix[2][1] += y * matrix[2][3];
	matrix[2][2] += z * matrix[2][3];

	matrix[3][0] += x * matrix[3][3];
	matrix[3][1] += y * matrix[3][3];
	matrix[3][2] += z * matrix[3][3];
}

void TransformationMatrix::rotateZ(float angle)
{
	float tmp[4][2];

	tmp[0][0] = cos(angle) * matrix[0][0] - sin(angle) * matrix[0][1];
	tmp[0][1] = sin(angle) * matrix[0][0] + cos(angle) * matrix[0][1];

	tmp[1][0] = cos(angle) * matrix[1][0] - sin(angle) * matrix[1][1];
	tmp[1][1] = sin(angle) * matrix[1][0] + cos(angle) * matrix[1][1];

	tmp[2][0] = cos(angle) * matrix[2][0] - sin(angle) * matrix[2][1];
	tmp[2][1] = sin(angle) * matrix[2][0] + cos(angle) * matrix[2][1];

	tmp[3][0] = cos(angle) * matrix[3][0] - sin(angle) * matrix[3][1];
	tmp[3][1] = sin(angle) * matrix[3][0] + cos(angle) * matrix[3][1];

	matrix[0][0] = tmp[0][0];
	matrix[0][1] = tmp[0][1];

	matrix[1][0] = tmp[1][0];
	matrix[1][1] = tmp[1][1];

	matrix[2][0] = tmp[2][0];
	matrix[2][1] = tmp[2][1];

	matrix[3][0] = tmp[3][0];
	matrix[3][1] = tmp[3][1];
}

void TransformationMatrix::scale(float scalar)
{
	matrix[0][3] /= scalar;
	matrix[1][3] /= scalar;
	matrix[2][3] /= scalar;
	matrix[3][3] /= scalar;
}

void TransformationMatrix::scale(float sX, float sY, float sZ)
{
	matrix[0][0] *= sX;
	matrix[0][1] *= sY;
	matrix[0][2] *= sZ;

	matrix[1][0] *= sX;
	matrix[1][1] *= sY;
	matrix[1][2] *= sZ;

	matrix[2][0] *= sX;
	matrix[2][1] *= sY;
	matrix[2][2] *= sZ;

	matrix[3][0] *= sX;
	matrix[3][1] *= sY;
	matrix[3][2] *= sZ;
}

