#include "Accelerometer.h"

///////////////////////////////////////////////////////////////////////////////
// Public methods

Accelerometer::Accelerometer(int n) {
	joy = SDL_JoystickOpen(n);
}

Accelerometer::~Accelerometer() {
	SDL_JoystickClose(joy);
}

float Accelerometer::getSingleAxisYAcceleration() {
	Vector3f data = getRawAccelerationData();
	return getSingleAxisAcceleration(data.magnitude(), data.y);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods

/*
 * getRawAccelerationData
 * Returns a raw acceleration vector, with components expressed in Gs
 */
Vector3f Accelerometer::getRawAccelerationData() {
	Vector3f data;
	data.x = (float) SDL_JoystickGetAxis(joy, 0) / 32768.0;
	data.y = (float) SDL_JoystickGetAxis(joy, 1) / 32768.0;
	data.z = (float) SDL_JoystickGetAxis(joy, 2) / 32768.0;
	return data;
}

/*
 * getSingleAxisAcceleration
 * Returns acceleration along a single axis, assuming no acceleration along other axes and G=1.0f
 *
 * Arguments
 *		magnitude:     Magnitude of the total acceleration vector
 *		axisComponent: Component vector of the acceleration vector for the single axis
 */
float Accelerometer::getSingleAxisAcceleration(float magnitude, float axisComponent) {
	const float g = 1.0f;

	// gAxis^2 should never be negative obviously, but it might be barely so due to floating point error
	float gAxis = sqrt( fabs(axisComponent * axisComponent + g * g - magnitude * magnitude) );
	float a1 = axisComponent + gAxis;
	float a2 = axisComponent - gAxis;

	// Return a1 or a2, whichever is closer to 0.
	// This is probably not the most accurate way to choose the correct root, but it's the simplest
	return (fabs(a1) < fabs(a2))
		? a1
		: a2;
}

