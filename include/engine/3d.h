#ifndef H3D_H_
#define H3D_H_

#include "engine/geometry.h"
#include "engine/strings.h"

#include "glad/wgl.h"
#include "glad/gl.h"

typedef struct Model{
	char name[STRING_SIZE];
	unsigned int VBO;
	unsigned int VAO;
	unsigned int numberOfTriangles;
}Model;

typedef struct Texture{
	char name[STRING_SIZE];
	unsigned int ID;
}Texture;

void Model_initFromMeshData(Model *, const unsigned char *, int);

void Model_initFromFile_obj(Model *, const char *);

void Texture_init(Texture *, const char *, unsigned char *, int, int);

void Texture_initFromFile(Texture *, const char *, const char *);

void GL3D_uniformMat4f(unsigned int, const char *, Mat4f);

void GL3D_uniformVec3f(unsigned int, const char *, Vec3f);

void GL3D_uniformVec4f(unsigned int, const char *, Vec4f);

void GL3D_uniformInt(unsigned int, const char *, int);

void GL3D_uniformFloat(unsigned int, const char *, float);

#endif
