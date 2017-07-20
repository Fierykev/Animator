#include "Shader_Helper.h"
#include <fstream>

using namespace std;

void createProgram(GLuint& program, GLuint& vs, GLuint& fs,
	char* fileVS, char* fileFS)
{
	program = glCreateProgram();

	if (program)
	{
		// create vs

		ifstream file(fileVS, ios::in | ios::binary);

		string data;

		if (file.is_open())
			data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		vs = glCreateShader(GL_VERTEX_SHADER);
		GLchar* prog = (GLchar*)data.c_str();
		glShaderSource(vs, 1, &prog, NULL);
		glCompileShader(vs);

		GLint compiled;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			printf("Cannot compile Smoke.vs\n");

			GLint max_length, length;
			glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &max_length);

			GLchar* info_log = new GLchar[max_length];
			glGetShaderInfoLog(vs, max_length, &length, info_log);
			printf("%s", info_log);
			delete info_log;

			//exit(-1);
		}

		file = ifstream(fileFS, ios::in | ios::binary);

		if (file.is_open())
			data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		fs = glCreateShader(GL_FRAGMENT_SHADER);
		prog = (GLchar*)data.c_str();
		glShaderSource(fs, 1, &prog, NULL);
		glCompileShader(fs);

		glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			printf("Cannot compile Smoke.fs\n");

			GLint max_length, length;
			glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &max_length);

			GLchar* info_log = new GLchar[max_length];
			glGetShaderInfoLog(fs, max_length, &length, info_log);
			printf("%s", info_log);
			delete info_log;

			//exit(-1);
		}

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		GLint linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked) {
			printf("ERR Linking\n");

			GLint max_length, length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

			GLchar* info_log = new GLchar[max_length];
			glGetProgramInfoLog(program, max_length, &length, info_log);
			printf("%s", info_log);
			delete info_log;

			//exit(-1);
		}
	}
}