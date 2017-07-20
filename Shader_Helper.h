#ifndef __SHADER_HELPER__
#define __SHADER_HELPER__

#include <GL/glew.h>

#define BUFFER_OFFSET(_i) (reinterpret_cast<char *>(NULL) + (_i))

void createProgram(GLuint& program, GLuint& vs, GLuint& fs,
	char* fileVS, char* fileFS);

#endif