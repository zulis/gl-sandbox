#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include "core/GL.h"

//void printErrorsGL(const char* func, int line);

//#define logError(...) {printf("Error (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");}
//#define logWarning(...) {printf("Warning (%s:%i): ", __FUNCTION__, __LINE__);printf(__VA_ARGS__);printf("\n");}
#define note(...) {/*printf("Note: ");*/printf(__VA_ARGS__);printf("\n");}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
	#include <windows.h>
	#define WIN32_LEAN_AND_MEAN
	enum console_color
	{
		red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	};
	inline std::ostream& operator<< (std::ostream& s, const console_color& c)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, c);
		return s;
	}
	#define error(...) { std::cout << red; printf(__VA_ARGS__); std::cout << white << std::endl; }
#else
	#define error(...) { printf(__VA_ARGS__);printf("\n"); }	
#endif

//#define logErrorsGL() printErrorsGL(__FUNCTION__, __LINE__)

/*
void printErrorsGL(const char* func, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;

	glErr = glGetError();

	while(glErr != GL_NO_ERROR)
	{
		printf("%s:%i :\n", func, line);

		switch(glErr)
		{

			case GL_INVALID_ENUM:
				printf("glError: Invalid enum \n");
				break;

			case GL_INVALID_VALUE:
				printf("glError: Invalid value\n");
				break;

			case GL_INVALID_OPERATION:
				printf("glError: Invalid operation \n");
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				printf("glError: Invalid framebuffer operation \n");
				break;

			case GL_OUT_OF_MEMORY:
				printf("glError: Out of memory \n");
				break;

#ifdef GL_STACK_UNDERFLOW

			case GL_STACK_UNDERFLOW:
				printf("glError: Stack underflow \n");
				break;
#endif

#ifdef GL_STACK_OVERFLOW

			case GL_STACK_OVERFLOW:
				printf("glError: Stack underflow \n");
				break;
#endif

			default:
				printf("glError: unrecognized error \n");
		}

		glErr = glGetError();
	}
}
*/