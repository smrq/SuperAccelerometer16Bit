#ifndef __ACCELEROMETER_H__
#define __ACCELEROMETER_H__

#include "SDL.h"
#include "Vector3f.h"

/*
 * Animation
 * Represents a series of GL textures for an animation.
 */
class Accelerometer {
	public:
		// Constructor
		// Argument:
		//		n: The index of the SDL joystick for the accelerometer
		Accelerometer(int n = 0);

		// Destructor
		~Accelerometer();

		// Returns acceleration along the Y axis, assuming no acceleration along other axes and G=1.0f
		float getSingleAxisYAcceleration();

	private:
		SDL_Joystick *joy;

		Vector3f getRawAccelerationData();
		float getSingleAxisAcceleration(float magnitude, float axisComponent);
};

#endif
