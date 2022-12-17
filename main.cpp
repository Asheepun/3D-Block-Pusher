#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/3d.h"
#include "engine/shaders.h"
#include "engine/renderer2d.h"
#include "engine/igui.h"

#include "game.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include <cstring>
#include <vector>

Game game;

Renderer2D_Renderer renderer2D;

int TERRAIN_WIDTH = 50;

float terrainScale = 100.0;
float terrainTextureScale = 10.0;

unsigned int modelShader;
unsigned int shadowMapShader;

int WIDTH = 1920;
int HEIGHT = 1080;

unsigned int shadowMapFBO;
unsigned int shadowMapTexture;
int SHADOW_MAP_WIDTH = 5000;
int SHADOW_MAP_HEIGHT = 5000;
float shadowMapScale = 20.0;

float fov = M_PI / 4;

Vec3f lightPos = { -10.0, 20.0, -10.0 };
Vec3f lightDirection = { 0.5, -1.0, 0.5 };

int viewMode = 0;

static size_t currentEntityID = 0;

void Entity_init(Entity *entity_p, Vec3f pos, Vec3f rotation, float scale, const char *modelName, const char *textureName, Vec4f color, enum EntityType type){

	entity_p->ID = currentEntityID;
	currentEntityID++;

	entity_p->pos = pos;
	entity_p->rotation = rotation;
	entity_p->scale = scale;
	entity_p->type = type;
	entity_p->color = color;
	String_set(entity_p->modelName, modelName, SMALL_STRING_SIZE);
	String_set(entity_p->textureName, textureName, SMALL_STRING_SIZE);

	entity_p->velocity = getVec3f(0.0, 0.0, 0.0);

}

void Game_addPlayer(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", getVec4f(0.0, 0.0, 1.0, 1.0), ENTITY_TYPE_PLAYER);

	game_p->entities.push_back(entity);

}

void Game_addObstacle(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", getVec4f(1.0, 1.0, 1.0, 1.0), ENTITY_TYPE_OBSTACLE);

	game_p->entities.push_back(entity);

}

void Engine_start(){

	printf("Starting the engine\n");

	Engine_setWindowSize(WIDTH / 2, HEIGHT / 2);

	Engine_centerWindow();

	Engine_setFPSMode(true);

	Renderer2D_init(&renderer2D, WIDTH, HEIGHT);

	//init game
	{

		game.currentGameState = GAME_STATE_EDITOR;
		//game.currentGameState = GAME_STATE_LEVEL;

		game.cameraPos = getVec3f(0.0, 6.0, -6.0);
		game.lastCameraPos = game.cameraPos;
		game.cameraRotation = getVec2f(M_PI / 2.0, -M_PI / 4.0);
		game.cameraDirection = getVec3f(0.0, 0.0, 1.0);

		VertexMesh_initFromFile_mesh(&game.cubeMesh,  "assets/models/untitled.mesh");

		game.hoveredEntityID = -1;

	}

	{
		Model model;

		Model_initFromFile_mesh(&model, "assets/models/untitled.mesh");

		String_set(model.name, "cube", STRING_SIZE);

		game.models.push_back(model);
	}
	
	Game_addPlayer(&game, getVec3f(0.0, 0.0, 0.0));

	for(float x = -5.0; x < 6; x += 1.0){
		for(float z = -5.0; z < 6; z += 1.0){
			Game_addObstacle(&game, getVec3f(x, -1.0, z));
		}
	}

	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/wrapped-sheep.png", "sheep");
		game.textures.push_back(texture);
	}
	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/cube-borders.png", "cube-borders");
		game.textures.push_back(texture);
	}
	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/grass.jpg", "grass");
		game.textures.push_back(texture);
	}
	{
		Texture texture;
		Texture_initFromFile(&texture, "assets/textures/blank.png", "blank");
		game.textures.push_back(texture);
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

int gameTime = 0;

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

	if(game.currentGameState == GAME_STATE_LEVEL){
		Game_levelState(&game);
	}
	if(game.currentGameState == GAME_STATE_EDITOR){
		Game_editorState(&game);
	}

	//set camera direction based on camera rotation
	game.cameraDirection.y = sin(game.cameraRotation.y);
	game.cameraDirection.x = cos(game.cameraRotation.x) * cos(game.cameraRotation.y);
	game.cameraDirection.z = sin(game.cameraRotation.x) * cos(game.cameraRotation.y);
	Vec3f_normalize(&game.cameraDirection);

}

clock_t startTicks = 0;
clock_t endTicks = 0;

void Engine_draw(){

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glViewport(0, 0, Engine_clientWidth, Engine_clientHeight);

	//setup camera matrix
	Mat4f cameraMat4f = getLookAtMat4f(game.cameraPos, game.cameraDirection);

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

	startTicks = clock();

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw entities for shadow map
	for(int i = 0; i < game.entities.size(); i++){

		Entity *entity_p = &game.entities[i];

		Model *model_p;

		for(int j = 0; j < game.models.size(); j++){
			if(strcmp(entity_p->modelName, game.models[j].name) == 0){
				model_p = &game.models[j];
			}
		}

		unsigned int currentShaderProgram = shadowMapShader;

		glUseProgram(currentShaderProgram);

		glBindBuffer(GL_ARRAY_BUFFER, model_p->VBO);
		glBindVertexArray(model_p->VAO);

		Mat4f modelRotationMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelRotationMat4f, getRotationMat4f(entity_p->rotation.x, entity_p->rotation.y, entity_p->rotation.z));

		Mat4f modelMat4f = getIdentityMat4f();

		Mat4f_mulByMat4f(&modelMat4f, getTranslationMat4f(entity_p->pos.x, entity_p->pos.y, entity_p->pos.z));

		Mat4f_mulByMat4f(&modelMat4f, getScalingMat4f(entity_p->scale));

		GL3D_uniformMat4f(currentShaderProgram, "modelMatrix", modelMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "modelRotationMatrix", modelRotationMat4f);
		GL3D_uniformMat4f(currentShaderProgram, "cameraMatrix", lightCameraMat4f);
		GL3D_uniformFloat(currentShaderProgram, "shadowMapScale", shadowMapScale);
		GL3D_uniformVec3f(currentShaderProgram, "lightDirection", lightDirection);

		glDrawArrays(GL_TRIANGLES, 0, model_p->numberOfTriangles * 3);

	}

	endTicks = clock();

	clock_t shadowMapTicks = endTicks - startTicks;

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

	startTicks = clock();

	glClearColor(0.5, 0.5, 0.9, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw entities
	for(int i = 0; i < game.entities.size(); i++){

		Entity *entity_p = &game.entities[i];

		Model *model_p;

		for(int j = 0; j < game.models.size(); j++){
			if(strcmp(entity_p->modelName, game.models[j].name) == 0){
				model_p = &game.models[j];
			}
		}

		Texture *texture_p;

		for(int j = 0; j < game.textures.size(); j++){
			if(strcmp(entity_p->textureName, game.textures[j].name) == 0){
				texture_p = &game.textures[j];
			}
		}

		//unsigned int currentShaderProgram = shadowMapShader;
		unsigned int currentShaderProgram = modelShader;

		Vec4f color = entity_p->color;

		if(entity_p->ID == game.hoveredEntityID){
			color.x *= 0.5;
			color.y *= 0.5;
			color.z *= 0.5;
		}

		glUseProgram(currentShaderProgram);

		glBindBuffer(GL_ARRAY_BUFFER, model_p->VBO);
		glBindVertexArray(model_p->VAO);

		GL3D_uniformTexture(currentShaderProgram, "colorTexture", 0, texture_p->ID);
		GL3D_uniformTexture(currentShaderProgram, "shadowMapTexture", 1, shadowMapTexture);

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
		GL3D_uniformVec4f(currentShaderProgram, "inputColor", color);

		glDrawArrays(GL_TRIANGLES, 0, model_p->numberOfTriangles * 3);
	
	}

	//draw HUD
	Renderer2D_updateDrawSize(&renderer2D, Engine_clientWidth, Engine_clientHeight);

	if(game.currentGameState == GAME_STATE_EDITOR){
		Renderer2D_drawColoredRectangle(&renderer2D, WIDTH / 2 - 3, HEIGHT / 2 - 3, 6, 6, Renderer2D_getColor(0.7, 0.7, 0.7), 1.0);
	}

	endTicks = clock();

	clock_t worldTicks = endTicks - startTicks;

	//printf("\nRENDER TIMES\n");
	//printf("shadow map: %f ms\n", (float)(shadowMapTicks / (float)(CLOCKS_PER_SEC / 1000)));
	//printf("world:      %f ms\n", (float)(worldTicks / (float)(CLOCKS_PER_SEC / 1000)));

}

void Engine_finnish(){

}
