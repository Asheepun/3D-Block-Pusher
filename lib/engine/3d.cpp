#include "engine/geometry.h"
#include "engine/files.h"
#include "engine/3d.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#ifdef _WIN32
#include "glad/wgl.h"
#endif
#include "glad/gl.h"

#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include <vector>

typedef struct Face{
	unsigned int indices[9];
}Face;

void Model_initFromMeshData(Model *model_p, const unsigned char *mesh, int numberOfTriangles){

	//printf("%i\n", numberOfTriangles);

	int componentSize = 2 * sizeof(Vec3f) + sizeof(Vec2f);

	glGenBuffers(1, &model_p->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, model_p->VBO);
	glBufferData(GL_ARRAY_BUFFER, componentSize * 3 * numberOfTriangles, mesh, GL_STATIC_DRAW);

	glGenVertexArrays(1, &model_p->VAO);
	glBindVertexArray(model_p->VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	model_p->numberOfTriangles = numberOfTriangles;

}

void Model_initFromFile_mesh(Model *model_p, const char *path){

	long int fileSize;
	char *data = getFileData_mustFree(path, &fileSize);

	int numberOfTriangles = fileSize / (sizeof(float) * 8 * 3);

	Model_initFromMeshData(model_p, (unsigned char *)data, numberOfTriangles);

}

void VertexMesh_initFromFile_mesh(VertexMesh *vertexMesh_p, const char *path){

	long int fileSize;
	char *data = getFileData_mustFree(path, &fileSize);

	int numberOfTriangles = fileSize / (sizeof(float) * 8 * 3);

	vertexMesh_p->length = numberOfTriangles * 3;
	vertexMesh_p->vertices = (Vec3f *)malloc(vertexMesh_p->length * sizeof(Vec3f));
	
	for(int i = 0; i < numberOfTriangles * 3; i++){
		
		memcpy(vertexMesh_p->vertices + i, data + i * 8 * sizeof(float), sizeof(Vec3f));

	}

}

void Texture_init(Texture *texture_p, const char *name, unsigned char *data, int width, int height){

	String_set(texture_p->name, name, SMALL_STRING_SIZE);

	glGenTextures(1, &texture_p->ID);

	glBindTexture(GL_TEXTURE_2D, texture_p->ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void Texture_initAsDepthMap(Texture *texture_p, int width, int height){

	glGenTextures(1, &texture_p->ID);
	glBindTexture(GL_TEXTURE_2D, texture_p->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void Texture_initAsColorMap(Texture *texture_p, int width, int height){

	glGenTextures(1, &texture_p->ID);
	glBindTexture(GL_TEXTURE_2D, texture_p->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void Texture_initFromFile(Texture *texture_p, const char *path, const char *name){

	int width, height, channels;
	unsigned char *data = stbi_load(path, &width, &height, &channels, 4);

	Texture_init(texture_p, name, data, width, height);

	free(data);

}

void TextureAtlas_init(TextureAtlas *textureAtlas_p, const char **pathsAndNames, int numberOfPaths){

	int atlasWidth = 0;
	int atlasHeight = 0;

	unsigned char *atlasData;
	unsigned char *textureData[numberOfPaths];
	Vec2f textureDataDimensions[numberOfPaths];

	for(int i = 0; i < numberOfPaths; i++){

		int width, height, channels;

		textureData[i] = stbi_load(pathsAndNames[i * 2], &width, &height, &channels, 4);

		textureDataDimensions[i].x = width;
		textureDataDimensions[i].y = height;

		atlasWidth += width;

		if(height > atlasHeight){
			atlasHeight = height;
		}

		SmallString name;
		String_set(name, pathsAndNames[i * 2 + 1], SMALL_STRING_SIZE);
		textureAtlas_p->names.push_back(name);

	}

	//create texture atlas data
	atlasData = (unsigned char *)malloc(4 * sizeof(unsigned char) * atlasWidth * atlasHeight);

	for(int y = 0; y < atlasHeight; y++){

		int x = 0;

		for(int i = 0; i < numberOfPaths; i++){

			if(y < (int)textureDataDimensions[i].y){

				unsigned char *dest = atlasData + 4 * (y * atlasWidth + x);
				unsigned char *src = textureData[i] + 4 * (y * (int)textureDataDimensions[i].y);
				int size = (int)textureDataDimensions[i].x * 4 * sizeof(unsigned char);

				memcpy(dest, src, size);

			}
				
			x += textureDataDimensions[i].x;

		}
	
	}

	//set texture coordinates
	{
		float x = 0.0;

		for(int i = 0; i < numberOfPaths; i++){

			Vec4f coordinates = getVec4f(x / atlasWidth, 1.0 - textureDataDimensions[i].y / atlasHeight, textureDataDimensions[i].x / atlasWidth, textureDataDimensions[i].y / atlasHeight);

			textureAtlas_p->textureCoordinates.push_back(coordinates);
			
			x += textureDataDimensions[i].x;
		
		}
	}

	Texture_init(&textureAtlas_p->texture, "atlas", atlasData, atlasWidth, atlasHeight);

	//free data
	free(atlasData);
	for(int i = 0; i < numberOfPaths; i++){
		free(textureData[i]);
	}

}

void TextureBuffer_init(TextureBuffer *textureBuffer_p, void *data, int size){

	glGenBuffers(1, &textureBuffer_p->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer_p->VBO);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	glGenTextures(1, &textureBuffer_p->TB);
	glBindTexture(GL_TEXTURE_BUFFER, textureBuffer_p->TB);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, textureBuffer_p->VBO);

}

void TextureBuffer_free(TextureBuffer *textureBuffer_p){
	
	glDeleteBuffers(1, &textureBuffer_p->VBO);
	glDeleteTextures(1, &textureBuffer_p->TB);

}

void GL3D_uniformMat4f(unsigned int shaderProgram, const char *locationName, Mat4f m){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);

	glUniformMatrix4fv(location, 1, GL_FALSE, (float *)m.values);

}

void GL3D_uniformVec3f(unsigned int shaderProgram, const char *locationName, Vec3f v){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);

	glUniform3f(location, v.x, v.y, v.z);

}

void GL3D_uniformVec4f(unsigned int shaderProgram, const char *locationName, Vec4f v){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);

	glUniform4f(location, v.x, v.y, v.z, v.w);

}

void GL3D_uniformInt(unsigned int shaderProgram, const char *locationName, int x){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);

	glUniform1i(location, x);

}

void GL3D_uniformFloat(unsigned int shaderProgram, const char *locationName, float x){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);

	glUniform1f(location, x);

}

void GL3D_uniformTexture(unsigned int shaderProgram, const char *locationName, unsigned int locationIndex, unsigned int textureID){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);
	glUniform1i(location, locationIndex);
	glActiveTexture(GL_TEXTURE0 + locationIndex);
	glBindTexture(GL_TEXTURE_2D, textureID);

}

void GL3D_uniformTextureBuffer(unsigned int shaderProgram, const char *locationName, unsigned int locationIndex, unsigned int textureID){

	unsigned int location = glGetUniformLocation(shaderProgram, locationName);
	glUniform1i(location, locationIndex);
	glActiveTexture(GL_TEXTURE0 + locationIndex);
	glBindTexture(GL_TEXTURE_BUFFER, textureID);

}
