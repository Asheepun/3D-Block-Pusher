#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "stdbool.h"

typedef struct Vec2f{
	float x;
	float y;
}Vec2f;

typedef struct Vec3f{
	float x;
	float y;
	float z;

	float& operator[] (int i){
		switch(i){
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default: throw "Coord index outside of vector bound";
		}
	}

}Vec3f;

typedef struct Vec4f{
	float x;
	float y;
	float z;
	float w;

	float& operator[] (int i){
		switch(i){
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: throw "Coord index outside of vector bound";
		}
	}

}Vec4f;

typedef struct Mat4f{
	float values[4][4];
}Mat4f;

//GENERAL MATH FUNCTIONS
float normalize(float);

float getSquared(float);

float getRandom();

bool checkEqualsFloat(float, float, float);

//VEC2F FUNCTIONS
Vec2f getVec2f(float, float);

void Vec2f_set(Vec2f *, float, float);

void Vec2f_log(Vec2f);

void Vec2f_add(Vec2f *, Vec2f);

void Vec2f_sub(Vec2f *, Vec2f);

void Vec2f_mul(Vec2f *, Vec2f);

void Vec2f_div(Vec2f *, Vec2f);

void Vec2f_mulByFloat(Vec2f *, float);

void Vec2f_divByFloat(Vec2f *, float);

void Vec2f_normalize(Vec2f *);

float getMagVec2f(Vec2f);

Vec2f getAddVec2f(Vec2f, Vec2f);

Vec2f getSubVec2f(Vec2f, Vec2f);

Vec2f getMulVec2fFloat(Vec2f, float);

Vec2f getDivVec2fFloat(Vec2f, float);

float getDistanceVec2f(Vec2f, Vec2f);

Vec2f getNormalizedVec2f(Vec2f);

Vec2f getInverseVec2f(Vec2f);

float getDotVec2f(Vec2f, Vec2f);

//VEC3F FUNCTIONS

Vec3f getVec3f(float, float, float);

void Vec3f_log(Vec3f);

void Vec3f_set(Vec3f *, float, float, float);

void Vec3f_add(Vec3f *, Vec3f);

void Vec3f_sub(Vec3f *, Vec3f);

void Vec3f_mulByFloat(Vec3f *, float);

void Vec3f_mulByVec3f(Vec3f *, Vec3f);

void Vec3f_divByFloat(Vec3f *, float);

void Vec3f_normalize(Vec3f *);

void Vec3f_inverse(Vec3f *);

void Vec3f_rotate(Vec3f *, float, float, float);

float getMagVec3f(Vec3f);

bool checkEqualsVec3f(Vec3f, Vec3f, float);

Vec3f getAddVec3f(Vec3f, Vec3f);

Vec3f getSubVec3f(Vec3f, Vec3f);

Vec3f getMulVec3fFloat(Vec3f, float);

Vec3f getDivVec3fFloat(Vec3f, float);

float getDistanceVec3f(Vec3f, Vec3f);

float getDotVec3f(Vec3f, Vec3f);

Vec3f getCrossVec3f(Vec3f, Vec3f);

float getAngleBetweenVec3f(Vec3f, Vec3f);

float getAreaFromTriangleVec3f(Vec3f, Vec3f, Vec3f);

Vec3f getNormalFromTriangleVec3f(Vec3f, Vec3f, Vec3f);

bool checkLineToTriangleIntersectionVec3f(Vec3f, Vec3f, Vec3f, Vec3f, Vec3f, Vec3f *);

void Vec3f_mulByMat4f(Vec3f *, Mat4f, float);

//VEC4f FUNCTIONS

Vec4f getVec4f(float, float, float, float);

void Vec4f_log(Vec4f);

void Vec4f_mulByMat4f(Vec4f *, Mat4f);

//MAT4F FUNCTIONS

void Mat4f_mulByMat4f(Mat4f *, Mat4f);

void Mat4f_log(Mat4f);

Mat4f getIdentityMat4f();

Mat4f getRotationMat4f(float, float, float);

Mat4f getTranslationMat4f(float, float, float);

Mat4f getScalingMat4f(float);

Mat4f getPerspectiveMat4f(float, float);

Mat4f getLookAtMat4f(Vec3f, Vec3f);

#endif
