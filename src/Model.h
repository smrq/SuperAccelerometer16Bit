#ifndef __MODEL_H__
#define __MODEL_H__

#include "SDL.h"

#include "Accelerometer.h"
#include "Vector3f.h"

/*
 * Model
 * Represents the current state.
 */
class Model {
	public:
		// Constructor
		// Arguments
		//		accelerometer: An Accelerometer instance
		//		sensitivity:   A constant multiplier for accelerometer data
		Model(Accelerometer *accelerometer, float sensitivity = 1.0f, float minX = 0.0f, float maxX = 1.0f);

		// Returns the current position
		float position() { return this->x; }

		// Returns the current velocity
		float velocity() { return this->v; }

		// Returns the current acceleration
		float acceleration() { return this->a; }

		// Updates the model state given a change in time.
		void tick(const int dt);

	private:
		Accelerometer *accelerometer;
		float sensitivity;
		float x, v, a;
		float minX, maxX;

		void calculatePhysics(float acceleration, float dt);
};

#endif
