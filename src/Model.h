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
		Model(Accelerometer *accelerometer, float sensitivity = 1.0f);

		// Returns the current position
		float position() { return this->x; }

		// Returns the current velocity
		float velocity() { return this->v; }

		// Returns the current acceleration
		float acceleration() { return this->a; }

		// Returns the current jerk (derivative of acceleration)
		float jerk() { return this->j; }

		// Updates the model state given a change in time.
		void tick(const int dt);

	private:
		Accelerometer *accelerometer;
		float sensitivity;
		float x;
		float v;
		float a;
		float j;

		static float getTrueYAcceleration(Vector3f acceleration);
};

#endif
