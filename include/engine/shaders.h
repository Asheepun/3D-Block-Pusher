#ifndef SHADERS_H_
#define SHADERS_H_

#ifdef _WIN32
#include "glad/wgl.h"
#endif

#include "glad/gl.h"

unsigned int getCompiledShader(const char *, GLenum);

#endif
