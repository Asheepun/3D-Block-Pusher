#ifndef RENDERER_2D_H_
#define RENDERER_2D_H_

#include "engine/text.h"
#include "engine/strings.h"
#include "engine/geometry.h"

enum Renderer2D_ShaderTypeEnum{
	RENDERER2D_VERTEX_SHADER,
	RENDERER2D_FRAGMENT_SHADER,
};

enum Renderer2D_UniformTypeEnum{
	RENDERER2D_UNIFORM_TYPE_INT,
	RENDERER2D_UNIFORM_TYPE_FLOAT,
	RENDERER2D_UNIFORM_TYPE_COLOR,
};

typedef struct Renderer2D_Color{
	float r;
	float g;
	float b;
}Renderer2D_Color;

typedef struct Renderer2D_ShaderPathTypePair{
	const char *path;
	enum Renderer2D_ShaderTypeEnum type;
}Renderer2D_ShaderPathTypePair;

typedef struct Renderer2D_Texture{
	char name[SMALL_STRING_SIZE];
	unsigned int ID;
	int width;
	int height;
}Renderer2D_Texture;

typedef struct Renderer2D_ShaderProgram{
	const char *name;
	unsigned int ID;
}Renderer2D_ShaderProgram;

typedef struct Renderer2D_Renderer{
	int width;
	int height;
	Vec2f offset;
	//float offsetX;
	//float offsetY;
	unsigned int rectangleTextureBufferID;
	unsigned int rectVAO;
	Renderer2D_Texture textTexture;

	Renderer2D_ShaderProgram textureShaderProgram;
	Renderer2D_ShaderProgram colorShaderProgram;
	Renderer2D_ShaderProgram circleShaderProgram;

	Renderer2D_ShaderProgram currentShaderProgram;
}Renderer2D_Renderer;

//INIT FUNCTIONS

void Renderer2D_init(Renderer2D_Renderer *, int, int);

void Renderer2D_Texture_init(Renderer2D_Texture *, const char *, char *, int, int);

void Renderer2D_Texture_initFromFile(Renderer2D_Texture *, const char *);

void Renderer2D_Texture_initFromText(Renderer2D_Texture *, const char *, Font);

void Renderer2D_ShaderProgram_init(Renderer2D_ShaderProgram *, const char *, Renderer2D_ShaderPathTypePair *, unsigned int);

//FREE FUNCTIONS

void Renderer2D_Texture_free(Renderer2D_Texture *);

//SETTINGS FUNCTIONS

void Renderer2D_updateDrawSize(Renderer2D_Renderer *, int, int);

//DRAWING FUNCTIONS

void Renderer2D_clear(Renderer2D_Renderer *);

void Renderer2D_setShaderProgram(Renderer2D_Renderer *, Renderer2D_ShaderProgram);

void Renderer2D_setTexture(Renderer2D_Renderer *, Renderer2D_Texture);

void Renderer2D_beginRectangle(Renderer2D_Renderer *, float, float, float, float);

void Renderer2D_beginCircle(Renderer2D_Renderer *, float, float, float);

void Renderer2D_beginText(Renderer2D_Renderer *, const char *, int, int, int, Font);

void Renderer2D_supplyUniform(Renderer2D_Renderer *, void *, const char *, enum Renderer2D_UniformTypeEnum);

void Renderer2D_drawRectangle(Renderer2D_Renderer *);

Renderer2D_Color Renderer2D_getColor(float, float, float);

//SIMPLE DRAWING FUNCTIONS

void Renderer2D_drawColoredRectangle(Renderer2D_Renderer *, float, float, float, float, Renderer2D_Color, float);

void Renderer2D_drawText(Renderer2D_Renderer *, const char *, float, float, int, Font, float);

void Renderer2D_drawTexture(Renderer2D_Renderer *, float, float, float, float, Renderer2D_Texture, float);

#endif
