#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/3d.h"
#include "engine/shaders.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <cstring>
#include <vector>

typedef struct Entity{
	Vec3f pos;
	Vec3f rotation;
	float scale;
	char modelName[SMALL_STRING_SIZE];
	char textureName[SMALL_STRING_SIZE];
}Entity;

typedef struct PointLight{
	Vec3f pos;
	float strength;
}PointEntity;

int TERRAIN_WIDTH = 50;

float terrainScale = 100.0;
float terrainTextureScale = 10.0;

float PLAYER_SPEED = 0.10;
float PLAYER_LOOK_SPEED = 0.0015;

Model teapotModel;
unsigned int modelShader;
unsigned int shadowMapShader;

Texture sheepTexture;

int WIDTH = 1920;
int HEIGHT = 1080;

unsigned int shadowMapFBO;
unsigned int shadowMapTexture;
int SHADOW_MAP_WIDTH = 2000;
int SHADOW_MAP_HEIGHT = 2000;
float shadowMapScale = 100.0;

float fov = M_PI / 4;

std::vector<Entity> entities;
std::vector<PointLight> pointLights;
std::vector<Model> models;
std::vector<Texture> textures;

Vec3f cameraPos = { 0.0, 3.0, 0.0 };
Vec3f lastCameraPos = { 0.0, 3.0, 0.0 };
Vec2f cameraRotation = { M_PI / 2, 0.0 };
Vec3f cameraDirection = { 0.0, 0.0, 1.0 };

Vec3f lightPos = { 0.0, 50.0, 0.0 };
Vec3f lightDirection = { 0.1, -1.0, 0.0 };

int viewMode = 0;

void Entity_init(Entity *entity_p, Vec3f pos, Vec3f rotation, float scale, const char *modelName, const char *textureName){

	entity_p->pos = pos;
	entity_p->rotation = rotation;
	entity_p->scale = scale;
	String_set(entity_p->modelName, modelName, SMALL_STRING_SIZE);
	String_set(entity_p->textureName, textureName, SMALL_STRING_SIZE);

}

void Engine_start(){

	printf("Starting the engine\n");

	Engine_setWindowSize(WIDTH / 2, HEIGHT / 2);

	Engine_centerWindow();

	Engine_setFPSMode(true);

	//generate terrain
	{
		Vec3f *vertices = (Vec3f *)malloc(sizeof(Vec3f) * TERRAIN_WIDTH * TERRAIN_WIDTH);

		for(int x = 0; x < TERRAIN_WIDTH; x++){
			for(int z = 0; z < TERRAIN_WIDTH; z++){
				float y = 2.0 * getRandom() / terrainScale;
				*(vertices + x * TERRAIN_WIDTH + z) = getVec3f((float)x / (float)TERRAIN_WIDTH - 0.5, y, (float)z / (float)TERRAIN_WIDTH - 0.5);
			}
		}

		std::vector<Vec3f> triangles;

		for(int x = 0; x < TERRAIN_WIDTH - 1; x++){
			for(int z = 0; z < TERRAIN_WIDTH - 1; z++){

				//first triangle in square
				triangles.push_back(*(vertices + x * TERRAIN_WIDTH + z));
				triangles.push_back(*(vertices + (x + 1) * TERRAIN_WIDTH + z + 1));
				triangles.push_back(*(vertices + (x + 1) * TERRAIN_WIDTH + z));

				//second triangle in square
				triangles.push_back(*(vertices + x * TERRAIN_WIDTH + z));
				triangles.push_back(*(vertices + (x) * TERRAIN_WIDTH + z + 1));
				triangles.push_back(*(vertices + (x + 1) * TERRAIN_WIDTH + z + 1));

			}
		}

		int componentSize = sizeof(Vec3f) * 2 + sizeof(Vec2f);

		unsigned char *mesh = (unsigned char *)malloc(triangles.size() * componentSize);

		for(int i = 0; i < triangles.size(); i++){

			memcpy(mesh + i * componentSize, &triangles[i], sizeof(Vec3f));

			Vec2f textureCoord = getVec2f(0.5 + triangles[i].x, 0.5 + triangles[i].z);
			Vec2f_mulByFloat(&textureCoord, terrainTextureScale);

			memcpy(mesh + i * componentSize + sizeof(Vec3f), &textureCoord, sizeof(Vec2f));

			int baseIndex = (int)floor((float)i / 3.0) * 3;
			Vec3f normal = getCrossVec3f(getSubVec3f(triangles[baseIndex], triangles[baseIndex + 1]), getSubVec3f(triangles[baseIndex], triangles[baseIndex + 2]));
			Vec3f_normalize(&normal);

			memcpy(mesh + i * componentSize + sizeof(Vec3f) + sizeof(Vec2f), &normal, sizeof(Vec3f));
		
		}

		Model model;

		Model_initFromMeshData(&model, mesh, triangles.size() / 3);

		String_set(model.name, "terrain", STRING_SIZE);

		models.push_back(model);

		free(mesh);
		free(vertices);

	}

	{
		Model model;

		Model_initFromFile_obj(&model, "assets/models/untitled.obj");

		String_set(model.name, "cube", STRING_SIZE);

		models.push_back(model);
	}
	
	/*
	{
		Model model;

		printf("began loading the dragon\n");

		Model_initFromFile_obj(&model, "assets/models/dragon.obj");

		String_set(model.name, "dragon", STRING_SIZE);

		models.push_back(model);

		printf("loaded the dragon\n");
	}
	*/
	
	{
		Entity entity;
		Entity_init(&entity, getVec3f(0.0, 0.0, 0.0), getVec3f(0.0, 0.0, 0.0), terrainScale, "terrain", "grass");
		entities.push_back(entity);
	}

	{
		Entity entity;
		Entity_init(&entity, getVec3f(0.0, 6.0, 10.0), getVec3f(0.0, 0.0, 0.0), 1.0, "cube", "sheep");
		entities.push_back(entity);
	}

	{
		Entity entity;
		Entity_init(&entity, getVec3f(0.0, 6.0, 5.0), getVec3f(0.0, 0.0, 0.0), 0.2, "cube", "sheep");
		entities.push_back(entity);
	}
	
	{
		Entity entity;
		Entity_init(&entity, getVec3f(0.0, 4.0, -5.0), getVec3f(0.0, 0.0, 0.0), 1.0, "cube", "blank");
		entities.push_back(entity);
	}
	
	{
		Entity entity;
		Entity_init(&entity, getVec3f(3.0, 4.0, 3.0), getVec3f(0.0, 0.0, 0.0), 1.0, "dragon", "sheep");
		entities.push_back(entity);
	}

	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/wrapped-sheep.png", "sheep");
		textures.push_back(texture);
	}
	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/grass.jpg", "grass");
		textures.push_back(texture);
	}
	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/blank.png", "blank");
		textures.push_back(texture);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	//compile shaders
	{
		unsigned int vertexShader = getCompiledShader("shaders/vertex-shader.glsl", GL_VERTEX_SHADER);
		unsigned int fragmentShader = getCompiledShader("shaders/fragment-shader.glsl", GL_FRAGMENT_SHADER);

		unsigned int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		modelShader = shaderProgram;
	}

	{
		unsigned int vertexShader = getCompiledShader("shaders/shadow-map-vertex-shader.glsl", GL_VERTEX_SHADER);
		unsigned int fragmentShader = getCompiledShader("shaders/shadow-map-fragment-shader.glsl", GL_FRAGMENT_SHADER);

		unsigned int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		shadowMapShader = shaderProgram;
	}

	//generate shadow map frame buffer
	glGenFramebuffers(1, &shadowMapFBO);

	glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  

}

int time = 0;

void Engine_update(float deltaTime){

	if(Engine_keys[ENGINE_KEY_Q].down){
		Engine_quit();
	}
	if(Engine_keys[ENGINE_KEY_F].downed){
		Engine_toggleFullscreen();
	}
	if(Engine_keys[ENGINE_KEY_V].downed){
		viewMode++;
	}
	if(viewMode > 1){
		viewMode = 0;
	}

	//handle camera movement
	cameraRotation.x += -Engine_pointer.movement.x * PLAYER_LOOK_SPEED;
	cameraRotation.y += -Engine_pointer.movement.y * PLAYER_LOOK_SPEED;

	if(cameraRotation.y > M_PI / 2 - 0.01){
		cameraRotation.y = M_PI / 2 - 0.01;
	}
	if(cameraRotation.y < -(M_PI / 2 - 0.01)){
		cameraRotation.y = -(M_PI / 2 - 0.01);
	}

	cameraDirection.y = sin(cameraRotation.y);
	cameraDirection.x = cos(cameraRotation.x) * cos(cameraRotation.y);
	cameraDirection.z = sin(cameraRotation.x) * cos(cameraRotation.y);
	Vec3f_normalize(&cameraDirection);

	lastCameraPos = cameraPos;

	if(Engine_keys[ENGINE_KEY_W].down){
		cameraPos.x += cameraDirection.x * PLAYER_SPEED;
		cameraPos.z += cameraDirection.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_S].down){
		cameraPos.x += -cameraDirection.x * PLAYER_SPEED;
		cameraPos.z += -cameraDirection.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_SPACE].down){
		cameraPos.y += PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_SHIFT].down){
		cameraPos.y += -PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_A].down){
		Vec3f left = getCrossVec3f(cameraDirection, getVec3f(0, 1.0, 0));
		Vec3f_normalize(&left);
		cameraPos.x += left.x * PLAYER_SPEED;
		cameraPos.z += left.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_D].down){
		Vec3f right = getCrossVec3f(getVec3f(0, 1.0, 0), cameraDirection);
		Vec3f_normalize(&right);
		cameraPos.x += right.x * PLAYER_SPEED;
		cameraPos.z += right.z * PLAYER_SPEED;
	}

	time += 1;

	//entities[0].rotation.x = (float)time / 50;
	entities[1].rotation.y = (float)time / 100.0;

	entities[4].rotation.z = (float)time / 50.0;
	entities[3].pos.y = 2.0 + sin((float)time / 20.0);
	//entities[3].rotation.y = (float)time / 100.0;

}

void Engine_draw(){

	glViewport(0, 0, Engine_clientWidth, Engine_clientHeight);

	//setup camera matrix
	Mat4f cameraMat4f = getLookAtMat4f(cameraPos, cameraDirection);

	//setup perspective matrix
	Mat4f perspectiveMat4f = getPerspectiveMat4f(fov, (float)Engine_clientWidth / (float)Engine_clientHeight);

	//setup shadow map camera matrix
	Vec3f_normalize(&lightDirection);
	Mat4f lightCameraMat4f = getLookAtMat4f(lightPos, lightDirection);

	//render shadow maps
	if(viewMode == 0){
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);  
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	}
	if(viewMode == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  
		glViewport(0, 0, Engine_clientWidth, Engine_clientHeight);
	}

	glCullFace(GL_BACK);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw entities for shadow map
	for(int i = 0; i < entities.size(); i++){

		Entity *entity_p = &entities[i];

		Model *model_p;

		for(int j = 0; j < models.size(); j++){
			if(strcmp(entity_p->modelName, models[j].name) == 0){
				model_p = &models[j];
			}
		}

		unsigned int currentShaderProgram = shadowMapShader;

		glUseProgram(currentShaderProgram);

		glBindBuffer(GL_ARRAY_BUFFER, model_p->VBO);
		glBindVertexArray(model_p->VAO);

		glBindTexture(GL_TEXTURE_2D, sheepTexture.ID);

		Mat4f modelRotationMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelRotationMat4f, getRotationMat4f(entity_p->rotation.x, entity_p->rotation.y, entity_p->rotation.z));

		Mat4f modelMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelMat4f, getTranslationMat4f(entity_p->pos.x, entity_p->pos.y, entity_p->pos.z));

		Mat4f_mulByMat4f(&modelMat4f, getScalingMat4f(entity_p->scale));

		GL3D_uniformMat4f(currentShaderProgram, "modelMatrix", modelMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "modelRotationMatrix", modelRotationMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "cameraMatrix", lightCameraMat4f);
		GL3D_uniformFloat(currentShaderProgram, "shadowMapScale", shadowMapScale);

		glDrawArrays(GL_TRIANGLES, 0, model_p->numberOfTriangles * 3);

	}

	//draw world
	if(viewMode == 0){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  
		glViewport(0, 0, Engine_clientWidth, Engine_clientHeight);
	}
	if(viewMode == 1){
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);  
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	}

	glCullFace(GL_FRONT);

	glClearColor(0.5, 0.5, 0.9, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw entities
	for(int i = 0; i < entities.size(); i++){

		Entity *entity_p = &entities[i];

		Model *model_p;

		for(int j = 0; j < models.size(); j++){
			if(strcmp(entity_p->modelName, models[j].name) == 0){
				model_p = &models[j];
			}
		}

		Texture *texture_p;

		for(int j = 0; j < textures.size(); j++){
			if(strcmp(entity_p->textureName, textures[j].name) == 0){
				printf("%s\n", textures[j].name);
				texture_p = &textures[j];
			}
		}

		//unsigned int currentShaderProgram = shadowMapShader;
		unsigned int currentShaderProgram = modelShader;

		glUseProgram(currentShaderProgram);

		glBindBuffer(GL_ARRAY_BUFFER, model_p->VBO);
		glBindVertexArray(model_p->VAO);

		//glBindTexture(GL_TEXTURE_2D, sheepTexture.ID);
		//glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

		unsigned int location1 = glGetUniformLocation(currentShaderProgram, "colorTexture");
		glUniform1i(location1, 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texture_p->ID);

		unsigned int location2  = glGetUniformLocation(currentShaderProgram, "shadowMapTexture");
		glUniform1i(location2,  1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

		Mat4f modelRotationMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelRotationMat4f, getRotationMat4f(entity_p->rotation.x, entity_p->rotation.y, entity_p->rotation.z));

		Mat4f modelMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelMat4f, getTranslationMat4f(entity_p->pos.x, entity_p->pos.y, entity_p->pos.z));

		Mat4f_mulByMat4f(&modelMat4f, getScalingMat4f(entity_p->scale));

		GL3D_uniformMat4f(currentShaderProgram, "modelMatrix", modelMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "modelRotationMatrix", modelRotationMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "cameraMatrix", cameraMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "perspectiveMatrix", perspectiveMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "lightCameraMatrix", lightCameraMat4f);
		GL3D_uniformFloat(currentShaderProgram, "shadowMapScale", shadowMapScale);
		GL3D_uniformVec3f(currentShaderProgram, "lightDirection", lightDirection);

		glDrawArrays(GL_TRIANGLES, 0, model_p->numberOfTriangles * 3);
	
	}

}

void Engine_finnish(){

}
