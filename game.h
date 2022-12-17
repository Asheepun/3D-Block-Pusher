#ifndef GAME_H_
#define GAME_H_

#include "engine/geometry.h"
#include "engine/3d.h"

#include <vector>

enum GameState{
	GAME_STATE_LEVEL,
	GAME_STATE_EDITOR,
};

enum EntityType{
	ENTITY_TYPE_PLAYER,
	ENTITY_TYPE_OBSTACLE,
	ENTITY_TYPE_ROCK,
	ENTITY_TYPE_STICKY_ROCK,
	ENTITY_TYPE_GOAL,
};

typedef struct Entity{
	size_t ID;
	enum EntityType type;
	Vec3f pos;
	Vec3f rotation;
	Vec3f velocity;
	float scale;
	char modelName[SMALL_STRING_SIZE];
	char textureName[SMALL_STRING_SIZE];
	Vec4f color;
}Entity;

typedef struct Game{

	std::vector<Entity> entities;

	VertexMesh cubeMesh;

	std::vector<Model> models;
	std::vector<Texture> textures;

	enum GameState currentGameState;

	Vec3f cameraPos;
	Vec3f lastCameraPos;
	Vec2f cameraRotation;
	Vec3f cameraDirection;

	size_t hoveredEntityID;

}Game;

//GLOBAL VARIABLES

static float PLAYER_SPEED = 0.10;
static float PLAYER_LOOK_SPEED = 0.0015;

//FILE: world.c

void Game_addPlayer(Game *, Vec3f);

void Game_addObstacle(Game *, Vec3f);

//FILE: levelState.c

void Game_initLevelState(Game *);

void Game_levelState(Game *);

//FILE: editorState.c

void Game_initEditorState(Game *);

void Game_editorState(Game *);

#endif
