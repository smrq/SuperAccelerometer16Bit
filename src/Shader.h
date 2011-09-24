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

/*
 * Shader
 * Represents a GLSL shader program.
 */
class Shader {
	public:
		// Constructor
		// Arguments
		//		vsFilename: Filename for a GLSL vertex shader
		//		fsFilename: Filename for a GLSL fragment shader
		Shader(std::string vsFilename, std::string fsFilename) { init(vsFilename, fsFilename); }

		// Destructor
		~Shader();

		// Binds this shader to the GL context.
		void bind() { glUseProgram(shaderId); }

		// Unbinds this shader from the GL context.
		void unbind() { glUseProgram(0); }

		// Returns the handle for the shader program.
		unsigned int id() { return shaderId; }

		// Returns the handle for a shader uniform
		int uniform(const char *name) { return glGetUniformLocation(shaderId, name); }

		enum {
			// Handle for the position attribute
			ATTRIB_POSITION,
			// Handle for the texture coordinate attribute
			ATTRIB_TEXCOORD
		};

	private:
        unsigned int shaderId;          // The handle for the shader program
		unsigned int vertexShader;      // The handle for the vertex shader
		unsigned int fragmentShader;    // The handle for the fragment shader

		static void compileShader(std::string const& filename, int id);
		void init(std::string vsFilename, std::string fsFilename);
};

#endif
