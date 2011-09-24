#include "Model.h"

///////////////////////////////////////////////////////////////////////////////
// Public methods

Model::Model(Accelerometer *accelerometer, float sensitivity, float minX, float maxX)
{
	this->accelerometer = accelerometer;
	this->sensitivity = sensitivity;
	this->minX = minX;
	this->maxX = maxX;
	this->x = 0.0f;
	this->v = 0.0f;
	this->a = 0.0f;
}

void Model::tick(const int dt)
{
	float acceleration = accelerometer->getSingleAxisYAcceleration() * sensitivity;
	calculatePhysics(acceleration, 0.001f * dt);
	
	// Limit position to bounds
	// If the model hits the bounds, set v and a to zero
	if (this->x < minX) {
		this->x = minX;
		this->v = 0.0f;
		this->a = 0.0f;		
	}

	if (this->x > maxX) {
		this->x = maxX;
		this->v = 0.0f;
		this->a = 0.0f;		
	}

	printf("X: %.5f, V: %.5f, A: %.5f\n",
			this->x, this->v, this->a);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods

void Model::calculatePhysics(float newA, float dt)
{
	// v' = v + a*dt
	float newV = this->v + this->a*dt;
	// x' = x + v*dt + 1/2 a*dt^2
	float newX = this->x + this->v*dt + 0.5f*this->a*dt*dt;

	this->a = newA;
	this->v = newV;
	this->x = newX;
}
