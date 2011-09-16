#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <stdexcept>

class FileOpenException : public std::runtime_error {
	public:
		FileOpenException() : std::runtime_error("File could not be opened") {}
		FileOpenException(std::string const& filename) : std::runtime_error("File \"" + filename + "\" could not be opened") {}
};

class GLSLCompilationException : public std::runtime_error {
	public:
		GLSLCompilationException() : std::runtime_error("GLSL compilation error") {}
		GLSLCompilationException(std::string const& message) : std::runtime_error("GLSL compilation error\n" + message) {}
		GLSLCompilationException(std::string const& filename, std::string const& message) : std::runtime_error("GLSL compilation error on file \"" + filename + "\"\n" + message) {}
};

class GLSLLinkingException : public std::runtime_error {
	public:
		GLSLLinkingException() : std::runtime_error("GLSL linking error") {}
		GLSLLinkingException(std::string const& message) : std::runtime_error("GLSL linking error\n" + message) {}
};

class GLSLValidationException : public std::runtime_error {
	public:
		GLSLValidationException() : std::runtime_error("GLSL validation error") {}
		GLSLValidationException(std::string const& message) : std::runtime_error("GLSL validation error\n" + message) {}
};

#endif
