#include "Shader.h"

/*
 * Constructor
 *
 * Arguments
 *     vsFilename: Filename of the vertex shader source file
 *     fsFilename: Filename of the fragment shader source file
 */
Shader::Shader(std::string vsFilename, std::string fsFilename)
{
	init(vsFilename, fsFilename);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods

/*
 * compileShader
 * Compiles a GLSL shader.
 *
 * Arguments
 *     filename: Filename of the shader source file.
 *     id:       Handle of the shader object.
 *
 * Throws
 *     FileOpenException
 *     GLSLCompilationException
 */
static void Shader::compileShader(std::string const& filename, int id)
{
    std::string source = FileIO::loadTextFile(filename);
	const char *glSource = source.c_str();

    // Compile the shader code
    glShaderSource  (id, 1, &glSource, NULL); 
    glCompileShader (id);

    // Validate compilation
    int shaderStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus); 
    if (shaderStatus != GL_TRUE) {
        char errorMessage[1024];
        glGetShaderInfoLog(id, 1024, NULL, errorMessage);
		throw GLSLCompilationException(filename, errorMessage);
    }
}

/*
 * init
 * Initializes the Shader.
 *
 * Arguments
 *     vsFilename: Filename of the vertex shader source file.
 *     fsFilename: Filename of the fragment shader source file.
 *
 * Throws
 *     FileOpenException
 *     GLSLCompilationException
 *     GLSLLinkingException
 *     GLSLValidationException
 */
void Shader::init(std::string vsFilename, std::string fsFilename)
{
    // Create shader objects
    vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile the shaders
    compileShader(vsFilename, vertexShader);
    compileShader(fsFilename, fragmentShader);

    // Create the program and attach the shaders & attributes
    shaderId = glCreateProgram();

    glAttachShader(shaderId, vertexShader);
    glAttachShader(shaderId, fragmentShader);

    glBindAttribLocation(shaderId, ATTRIB_POSITION, "Position");

    // Link the program
    glLinkProgram(shaderId);

    // Validate linking
    int shaderStatus;
    glGetProgramiv(shaderId, GL_LINK_STATUS, &shaderStatus); 
    if (shaderStatus != GL_TRUE) {
        char errorMessage[1024];
        glGetProgramInfoLog(shaderId, 1024, NULL, errorMessage);
		throw GLSLLinkingException(errorMessage);
    }

	// Validate program
    glValidateProgram(shaderId);
    glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &shaderStatus); 
    if (shaderStatus != GL_TRUE) {
        char errorMessage[1024];
        glGetProgramInfoLog(shaderId, 1024, NULL, errorMessage);
		throw GLSLValidationException(errorMessage);
    }

    // Enable the program
    glUseProgram(shaderId);
    glEnableVertexAttribArray(ATTRIB_POSITION);
}

///////////////////////////////////////////////////////////////////////////////
// Public methods


