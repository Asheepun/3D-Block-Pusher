#include "engine/geometry.h"

#include "stdbool.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//GENERAL MATH FUNCTIONS
float normalize(float x){
	return x / fabs(x);
}

float getSquared(float x){
	return x * x;
}

int ___x = 1;
int ___a = 65521;
int ___c = 0;
int ___m = 17364;

float getRandom(){

	___x = (___a * ___x + ___c) % ___m;
	float r = (float)___x;
	r /= ___m;

	return r;
	
}

float *coord(void *vector_p, int index){
	return ((float *)vector_p) + index;
}

//VEC2F FUNCTIONS

Vec2f getVec2f(float x, float y){
	Vec2f v = { x, y };
	return v;
}

void Vec2f_set(Vec2f *v, float x, float y){
	v->x = x;
	v->y = y;
}

void Vec2f_log(Vec2f v){
	printf("{ x: %f, y: %f }\n", v.x, v.y);
}

void Vec2f_add(Vec2f *v1, Vec2f v2){
	v1->x += v2.x;
	v1->y += v2.y;
}

void Vec2f_sub(Vec2f *v1, Vec2f v2){
	v1->x -= v2.x;
	v1->y -= v2.y;
}

void Vec2f_mul(Vec2f *v1, Vec2f v2){
	v1->x *= v2.x;
	v1->y *= v2.y;
}

void Vec2f_div(Vec2f *v1, Vec2f v2){
	v1->x /= v2.x;
	v1->y /= v2.y;
}

void Vec2f_mulByFloat(Vec2f *v1, float a){
	v1->x *= a;
	v1->y *= a;
}

void Vec2f_divByFloat(Vec2f *v1, float a){
	v1->x /= a;
	v1->y /= a;
}

void Vec2f_normalize(Vec2f *v){
	Vec2f_divByFloat(v, getMagVec2f(*v));
}

float getMagVec2f(Vec2f v){
	return sqrt(v.x * v.x + v.y * v.y);
}

Vec2f getAddVec2f(Vec2f v1, Vec2f v2){
	return getVec2f(v1.x + v2.x, v1.y + v2.y);
}

Vec2f getSubVec2f(Vec2f v1, Vec2f v2){
	return getVec2f(v1.x - v2.x, v1.y - v2.y);
}

Vec2f getMulVec2fFloat(Vec2f v, float a){
	return getVec2f(v.x * a, v.y * a);
}

Vec2f getDivVec2fFloat(Vec2f v, float a){
	return getVec2f(v.x / a, v.y / a);
}

float getDistanceVec2f(Vec2f v1, Vec2f v2){
	return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

Vec2f getNormalizedVec2f(Vec2f v){
	return getVec2f(v.x / getMagVec2f(v), v.y / getMagVec2f(v));
}

Vec2f getInverseVec2f(Vec2f v){
	return getVec2f(1 / v.x, 1 / v.y);
}

float getDotVec2f(Vec2f v1, Vec2f v2){
	return v1.x * v2.x + v1.y * v2.y;
}

//VEC3F FUNCTIONS
Vec3f getVec3f(float x, float y, float z){
	Vec3f v = { x, y, z };
	return v;
}

void Vec3f_log(Vec3f v){
	printf("{ x: %f, y: %f, z: %f }\n", v.x, v.y, v.z);
}

void Vec3f_add(Vec3f *v1, Vec3f v2){
	v1->x += v2.x;
	v1->y += v2.y;
	v1->z += v2.z;
}

void Vec3f_sub(Vec3f *v1, Vec3f v2){
	v1->x -= v2.x;
	v1->y -= v2.y;
	v1->z -= v2.z;
}

void Vec3f_mulByFloat(Vec3f *v, float f){
	v->x *= f;
	v->y *= f;
	v->z *= f;
}

void Vec3f_mulByVec3f(Vec3f *v1, Vec3f v2){
	v1->x *= v2.x;
	v1->y *= v2.y;
	v1->z *= v2.z;
}

void Vec3f_divByFloat(Vec3f *v, float x){
	v->x /= x;
	v->y /= x;
	v->z /= x;
}

void Vec3f_normalize(Vec3f *v){
	Vec3f_divByFloat(v, getMagVec3f(*v));
}

void Vec3f_inverse(Vec3f *v){
	v->x *= -1;
	v->y *= -1;
	v->z *= -1;
}

void Vec3f_rotate(Vec3f *point, float rotX, float rotY, float rotZ){

	//x axis
	float mag = sqrt(point->y * point->y + point->z * point->z);

	float deg = atan2(point->z, point->y);

	deg += rotX;

	point->y = mag * cos(deg);
	point->z = mag * sin(deg);

	//y axis
	mag = sqrt(point->x * point->x + point->z * point->z);

	deg = atan2(point->x, point->z);

	deg += rotY;

	point->x = mag * sin(deg);
	point->z = mag * cos(deg);

	//z axis
	mag = sqrt(point->x * point->x + point->y * point->y);

	deg = atan2(point->y, point->x);

	deg += rotZ;

	point->x = mag * cos(deg);
	point->y = mag * sin(deg);

}

float getMagVec3f(Vec3f v){
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

bool checkEqualsVec3f(Vec3f v1, Vec3f v2, float tolerance){
	return fabs(v1.x - v2.x) < tolerance
		&& fabs(v1.y - v2.y) < tolerance
		&& fabs(v1.z - v2.z) < tolerance;
}

Vec3f getAddVec3f(Vec3f v1, Vec3f v2){
	return getVec3f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3f getSubVec3f(Vec3f v1, Vec3f v2){
	return getVec3f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3f getMulVec3fFloat(Vec3f v, float a){
	return getVec3f(v.x * a, v.y * a, v.z * a);
}

Vec3f getDivVec3fFloat(Vec3f v, float a){
	return getVec3f(v.x / a, v.y / a, v.z / a);
}

float getDistanceVec3f(Vec3f v1, Vec3f v2){
	return getMagVec3f(getSubVec3f(v1, v2));
}

float getDotVec3f(Vec3f v1, Vec3f v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3f getCrossVec3f(Vec3f v1, Vec3f v2){
	Vec3f v = {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x,
	};

	return v;
}

float getAngleBetweenVec3f(Vec3f v1, Vec3f v2){
	float a = getMagVec3f(getSubVec3f(v1, v2));
	float b = getMagVec3f(v1);
	float c = getMagVec3f(v2);

	return acos((a * a - b * b - c * c) / (-2 * b * c));
}

float getAreaFromTriangleVec3f(Vec3f v1, Vec3f v2, Vec3f v3){
	return getMagVec3f(getCrossVec3f(getSubVec3f(v1, v2), getSubVec3f(v1, v3))) / 2;
	//float a = getMagVec3f(getSubVec3f(v1, v2));
	//float b = getMagVec3f(getSubVec3f(v1, v3));
	//float c = getMagVec3f(getSubVec3f(v2, v3));

	//return (b * c / 2) * sqrt(1 - getSquare((b * b + c * c - a * a) / (2 * b * c)));
}

Vec3f getNormalFromTriangleVec3f(Vec3f v1, Vec3f v2, Vec3f v3){
	Vec3f N = getCrossVec3f(getSubVec3f(v2, v1), getSubVec3f(v3, v1));

	Vec3f_normalize(&N);

	return N;
}

bool checkLineToTriangleIntersectionVec3f(Vec3f l1, Vec3f l2, Vec3f t1, Vec3f t2, Vec3f t3, Vec3f *collisionPoint_p){

	//printf("infunc---\n");

	Vec3f N = getCrossVec3f(getSubVec3f(t2, t1), getSubVec3f(t3, t1));

	Vec3f P = getSubVec3f(l1, getMulVec3fFloat(getSubVec3f(l2, l1), getDotVec3f(getSubVec3f(l1, t1), N) / getDotVec3f(getSubVec3f(l2, l1), N)));

	*collisionPoint_p = P;

	float totalArea = getAreaFromTriangleVec3f(t1, t2, t3);
	float a1 = getAreaFromTriangleVec3f(t1, t2, P);
	float a2 = getAreaFromTriangleVec3f(t1, t3, P);
	float a3 = getAreaFromTriangleVec3f(t2, t3, P);

	/*
	printf("%f\n", totalArea);
	printf("%f\n", a1);
	printf("%f\n", a2);
	printf("%f\n", a3);
	Vec3f_log(P);

	printf("donefunc---\n");
	*/

	return a1 + a2 + a3 <= totalArea + 0.001;

	return false;

	/*
	float c1 = (P.x - t1.x) * (t2.y - t1.y) - (P.y - t1.y) * (t2.x - t1.x);
	float c2 = (P.x - t2.x) * (t3.y - t2.y) - (P.y - t2.y) * (t3.x - t2.x);
	float c3 = (P.x - t3.x) * (t1.y - t3.y) - (P.y - t3.y) * (t1.x - t3.x);

	Vec3f_log(N);
	Vec3f_log(P);

	printf("donefunc---\n");

	return c1 > 0 && c2 > 0 && c3 > 0
	|| c1 < 0 && c2 < 0 && c3 < 0
	|| c1 == 0 && c2 == 0 && c3 == 0;
	*/

}

void Vec3f_mulByMat4f(Vec3f *v_p, Mat4f m, float w){

	Vec4f v4 = getVec4f(v_p->x, v_p->y, v_p->z, w);

	Vec4f_mulByMat4f(&v4, m);

	v_p->x = v4.x;
	v_p->y = v4.y;
	v_p->z = v4.z;

}

//VEC4F FUNCTIONS


Vec4f getVec4f(float x, float y, float z, float w){
	Vec4f v = { x, y, z, w };
	return v;
}

void Vec4f_log(Vec4f v){
	printf("{ %f, %f, %f, %f }\n", v.x, v.y, v.z, v.w);
	
}

void Vec4f_mulByMat4f(Vec4f *v_p, Mat4f m){

	Vec4f newV = {
		v_p->x * m.values[0][0] + v_p->y * m.values[0][1] + v_p->z * m.values[0][2] + v_p->w * m.values[0][3],
		v_p->x * m.values[1][0] + v_p->y * m.values[1][1] + v_p->z * m.values[1][2] + v_p->w * m.values[1][3],
		v_p->x * m.values[2][0] + v_p->y * m.values[2][1] + v_p->z * m.values[2][2] + v_p->w * m.values[2][3],
		v_p->x * m.values[3][0] + v_p->y * m.values[3][1] + v_p->z * m.values[3][2] + v_p->w * m.values[3][3],
	};

	*v_p = newV;

}

//MAT4F FUNCTIONS

void Mat4f_log(Mat4f m){

	printf("{\n");
	for(int i = 0; i < 4; i++){
		printf("\t");
		for(int j = 0; j < 4; j++){
			printf("%f, ", m.values[i][j]);
		}
		printf("\n");
	}
	printf("}\n");

}

void Mat4f_mulByMat4f(Mat4f *m1_p, Mat4f m2){

	Mat4f newMatrix;
	memset(newMatrix.values, 0, 16 * sizeof(float));

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				newMatrix.values[i][j] += m1_p->values[i][k] * m2.values[k][j];
			}
		}
	}

	memcpy(m1_p->values, newMatrix.values, 16 * sizeof(float));
	
}

Mat4f getIdentityMat4f(){

	Mat4f identityMat4f = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	return identityMat4f;

}

Mat4f getRotationMat4f(float x, float y, float z){

	Mat4f xRotationMat4f = {
		1, 0, 	   0, 		0,
		0, cos(x), -sin(x),	0,
		0, sin(x), cos(x), 	0,
		0, 0, 	   0, 		1,
	};

	Mat4f yRotationMat4f = {
		cos(y),  0, sin(y), 0,
		0, 		 1, 0, 		0,
		-sin(y), 0, cos(y), 0,
		0, 		 0, 0, 		1,
	};

	Mat4f zRotationMat4f = {
		cos(z),	-sin(z), 0, 0,
		sin(z),	cos(z),  0, 0,
		0, 		0, 		 1, 0,
		0, 		0, 		 0, 1,
	};

	Mat4f_mulByMat4f(&xRotationMat4f, yRotationMat4f);
	Mat4f_mulByMat4f(&xRotationMat4f, zRotationMat4f);

	return xRotationMat4f;
	
}

Mat4f getTranslationMat4f(float x, float y, float z){

	Mat4f translationMat4f = {
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1,
	};

	return translationMat4f;

}

Mat4f getScalingMat4f(float scale){

	Mat4f scalingMat4f = {
		scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0,
		0, 0, 0, 1,
	};

	return scalingMat4f;

}

Mat4f getPerspectiveMat4f(float fov, float aspectRatio){

	Mat4f perspectiveMat4f = {
		1 / (aspectRatio * tan(fov / 2)), 	0, 				  0,   0,
		0, 									1 / tan(fov / 2), 0,   0,
		0, 									0,				  1.0, 0, 
		0, 									0, 				  1,   0,
	};
	
	return perspectiveMat4f;


}

Mat4f getLookAtMat4f(Vec3f pos, Vec3f direction){

	Vec3f right = getCrossVec3f(getVec3f(0.0, 1.0, 0.0), direction);
	Vec3f up = getCrossVec3f(direction, right);

	Vec3f_normalize(&right);
	Vec3f_normalize(&up);

	Mat4f lookAtMat4f = { 
		right.x,	 right.y, 	  right.z, 	   0.0,
		up.x, 	   	 up.y, 		  up.z, 	   0.0,
		direction.x, direction.y, direction.z, 0.0,
		0.0, 		 0.0,  		  0.0, 		   1.0,
	};

	Mat4f_mulByMat4f(&lookAtMat4f, getTranslationMat4f(-pos.x, -pos.y, -pos.z));

	return lookAtMat4f;

}
