#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <cstdio>

#include <string>
#include <vector>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "SDL_image.h"
#include "PDL.h"

/*
 * Animation
 * Represents a series of GL textures for an animation.
 */
class Animation {
	public:
		// Constructor
		// Arguments
		//		frameFilenames: A list of filenames for textures corresponding to each frame of animation.
		Animation(std::vector <std::string> frameFilenames);

		// Destructor
		~Animation();

		// Returns the number of frames in the animation.
		int frameCount() { return count; }

		// Binds the GL texture for the specified frame of animation.
		void bindFrame(int n);

		// Returns the texture coordinate for the right of the given frame.
		float frameWCoord(int n) { return wCoords[n]; }

		// Returns the texture coordinate for the bottom of the given frame.
		float frameHCoord(int n) { return hCoords[n]; }

		// Returns the aspect ratio for the given frame.
		float aspectRatio(int n) { return aspectRatios[n]; }

	private:
		int count;
		unsigned int *textures;
		float *wCoords;
		float *hCoords;
		float *aspectRatios;

		static void loadTexture(std::string filename, unsigned int texture, float &wCoord, float &hCoord, float &aspectRatio);
		static SDL_Surface *resizeSurface(SDL_Surface *surface);
};

#endif
