#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <cstdio>

#include <string>
#include <vector>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "SDL_image.h"
#include "PDL.h"

class Animation {
	public:
		Animation(std::vector <std::string> frameFilenames);
		~Animation();

		int frameCount() { return count; }
		void bindFrame(int n);
		float frameWCoord(int n) { return wCoords[n]; }
		float frameHCoord(int n) { return hCoords[n]; }
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
