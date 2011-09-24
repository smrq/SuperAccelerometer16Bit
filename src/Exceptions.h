#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <stdexcept>

/*
 * FileOpenException
 * Thrown when a file cannot be opened.
 */
class FileOpenException : public std::runtime_error {
	public:
		FileOpenException() : std::runtime_error("File could not be opened") {}
		FileOpenException(std::string const& filename) : std::runtime_error("File \"" + filename + "\" could not be opened") {}
};

/*
 * GLSLCompilationException
 * Thrown when compilation of a GLSL shader fails.
 */
class GLSLCompilationException : public std::runtime_error {
	public:
		GLSLCompilationException() : std::runtime_error("GLSL compilation error") {}
		GLSLCompilationException(std::string const& message) : std::runtime_error("GLSL compilation error\n" + message) {}
		GLSLCompilationException(std::string const& filename, std::string const& message) : std::runtime_error("GLSL compilation error on file \"" + filename + "\"\n" + message) {}
};

/*
 * GLSLCompilationException
 * Thrown when linking of a GLSL shader fails.
 */
class GLSLLinkingException : public std::runtime_error {
	public:
		GLSLLinkingException() : std::runtime_error("GLSL linking error") {}
		GLSLLinkingException(std::string const& message) : std::runtime_error("GLSL linking error\n" + message) {}
};

/*
 * GLSLCompilationException
 * Thrown when validation of a GLSL shader fails.
 */
class GLSLValidationException : public std::runtime_error {
	public:
		GLSLValidationException() : std::runtime_error("GLSL validation error") {}
		GLSLValidationException(std::string const& message) : std::runtime_error("GLSL validation error\n" + message) {}
};

#endif
