#include "Animation.h"

/*
 * Constructor
 *
 * Arguments
 *     frameFilenames: A list of filenames for textures corresponding to each frame of animation.
 */
Animation::Animation(std::vector<std::string> frameFilenames)
{
	count = frameFilenames.size();
	textures = new unsigned int[count];
	wCoords = new float[count];
	hCoords = new float[count];
	aspectRatios = new float[count];

	glGenTextures(count, textures);

	int i = 0;
	for (std::vector<std::string>::iterator iter = frameFilenames.begin();
		 iter != frameFilenames.end();
		 iter++)
	{
		printf("Loading texture %d: %s\n", i, (*iter).c_str());
		loadTexture(*iter, textures[i], wCoords[i], hCoords[i], aspectRatios[i]);
		i++;
	}
}

/*
 * Destructor
 */
Animation::~Animation()
{
	glDeleteTextures(count, textures);
	delete textures;
	delete wCoords;
	delete hCoords;
	delete aspectRatios;
}

///////////////////////////////////////////////////////////////////////////////
// Public methods

/*
 * bindFrame
 * Binds the given animation frame.
 *
 * Arguments
 *     n: The index of the animation frame to bind.
 */
void Animation::bindFrame(int n)
{
	glBindTexture(GL_TEXTURE_2D, textures[n]);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods

/*
 * loadTexture
 * Loads texture data from the given file into the given GL texture.
 *
 * Arguments
 *     filename: Filename of the texture file.
 *     texture:  Handle of the GL texture.
 */
void Animation::loadTexture(std::string filename, unsigned int texture, float &wCoord, float &hCoord, float &aspectRatio)
{
	SDL_Surface *rawSurface = IMG_Load(filename.c_str());
	SDL_Surface *surface = resizeSurface(rawSurface);

	int format = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	wCoord = ((float)rawSurface->w) / ((float)surface->w);
	hCoord = ((float)rawSurface->h) / ((float)surface->h);
	aspectRatio = ((float)rawSurface->w) / ((float)rawSurface->h);

	SDL_FreeSurface(rawSurface);
	SDL_FreeSurface(surface);
}

SDL_Surface *Animation::resizeSurface(SDL_Surface *surface)
{
	// Calculate smallest power-of-2 dimensions that contain the surface
	int newWidth = 1, newHeight = 1;
	while (newWidth < surface->w) {
		newWidth <<= 1;
	}
	while (newHeight < surface->h) {
		newHeight <<= 1;
	}

	// Blit to the new surface
	SDL_PixelFormat *format = surface->format;
	SDL_Surface *newSurface = SDL_CreateRGBSurface(surface->flags, newWidth, newHeight, format->BitsPerPixel,
								format->Rmask, format->Gmask, format->Bmask, format->Amask);
	SDL_BlitSurface(surface, NULL, newSurface, NULL);

	return newSurface;
}
