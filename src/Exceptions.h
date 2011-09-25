#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <stdexcept>

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

/*
 * JsonParseException
 * Thrown when parsing of a JSON file fails.
 */
class JsonParseException : public std::runtime_error {
	public:
		JsonParseException() : std::runtime_error("JSON parsing error") {}
		JsonParseException(std::string const& message) : std::runtime_error("JSON parsing error\n" + message) {}
};

#endif
