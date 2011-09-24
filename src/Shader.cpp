#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////
// Public methods

Shader::~Shader()
{
	glDetachShader(shaderId, vertexShader);
	glDetachShader(shaderId, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteProgram(shaderId);
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
void Shader::compileShader(std::string const& filename, int id)
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

	// Bind vertex attribute locations
    glBindAttribLocation(shaderId, ATTRIB_POSITION, "a_Position");
    glBindAttribLocation(shaderId, ATTRIB_TEXCOORD, "a_TexCoord");

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

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(ATTRIB_POSITION);
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
}
