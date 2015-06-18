#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __APPLE__
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl.h>
#else
#include <GL/GL.h>
#endif