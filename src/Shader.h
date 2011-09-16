#ifndef __SHADER_H__
#define __SHADER_H__

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"

#include "Exceptions.h"
#include "FileIO.h"

class Shader {
	public:
		Shader(std::string vsFilename, std::string fsFilename);
		~Shader(); // TODO what to destroy

		void bind(); // TODO whats this for
		void unbind(); // TODO whats this for
		unsigned int id() { return shaderId; }

	private:
        unsigned int shaderId;          // The handle for the shader program
		unsigned int vertexShader;      // The handle for the vertex shader
		unsigned int fragmentShader;    // The handle for the fragment shader

		static void compileShader(std::string const& filename, int id);
		void init(std::string vsFilename, std::string fsFilename)
}

#endif
