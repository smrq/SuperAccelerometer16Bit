#include "Model.h"

///////////////////////////////////////////////////////////////////////////////
// Public methods

Model::Model(Accelerometer *accelerometer, float sensitivity)
{
	this->accelerometer = accelerometer;
	this->sensitivity = sensitivity;
	this->x = 0.0f;
	this->v = 0.0f;
	this->a = 0.0f;
}

void Model::tick(const int dt)
{
	const float dtSec = 0.001f * dt;
	float acceleration = accelerometer->getSingleAxisYAcceleration() * sensitivity;
	
	this->j = (acceleration - this->a) / dtSec;
	this->a = acceleration;
	this->v += acceleration * dtSec;
	// this->x = ???

	printf("X: %.5f, V: %.5f, A: %.5f, J: %.5f\n",
			this->x, this->v, this->a, this->j);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods

/*
 * getTrueYAcceleration
 * Calculates single-axis Y acceleration, assuming G=1.0 and no acceleration on other axes.
 * 
 * Arguments
 *		acceleration: The source 3-axis acceleration vector
 * Returns
 *		
 */
float Model::getTrueYAcceleration(Vector3f acceleration)
{
	const float g = 1.0f;
	float mag = acceleration.magnitude();
	// gy^2 should never be negative obviously, but it might be barely
	// negative due to floating point error for very low true acceleration
	float gy = sqrt( fabs(acceleration.y * acceleration.y + g * g - mag * mag) );
    float a1 = acceleration.y + gy;
	float a2 = acceleration.y - gy;

	// Return a1 or a2, whichever is closer to 0
	return (fabs(a1) < fabs(a2))
        ? a1
        : a2;
}
