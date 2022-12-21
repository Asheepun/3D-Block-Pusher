#ifndef GAME_H_
#define GAME_H_

#include "engine/geometry.h"
#include "engine/3d.h"
#include "engine/igui.h"
#include "engine/strings.h"

#include <vector>

enum GameState{
	GAME_STATE_LEVEL,
	GAME_STATE_EDITOR,
};

enum EntityType{
	ENTITY_TYPE_OBSTACLE,
	ENTITY_TYPE_ROCK,
	ENTITY_TYPE_STICKY_ROCK,
	ENTITY_TYPE_GOAL,
	ENTITY_TYPE_PLAYER,
	NUMBER_OF_ENTITY_TYPES,
};

typedef struct Entity{
	size_t ID;
	enum EntityType type;
	Vec3f pos;
	Vec3f startPos;
	Vec3f endPos;
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
	bool mustInitGameState;

	Vec3f cameraPos;
	Vec3f lastCameraPos;
	Vec2f cameraRotation;
	Vec3f cameraDirection;

	size_t hoveredEntityID;
	//char currentLevel[STRING_SIZE];
	IGUI_TextInputData levelNameTextInputData;

}Game;

//GLOBAL VARIABLES

static int WIDTH = 1920;
static int HEIGHT = 1080;

static float PLAYER_SPEED = 0.10;
static float PLAYER_LOOK_SPEED = 0.0015;

static Vec4f PLAYER_COLOR = { 0.1, 0.1, 0.9, 1.0 };
static Vec4f OBSTACLE_COLOR = { 0.8, 0.6, 0.3, 1.0 };
static Vec4f ROCK_COLOR = { 0.7, 0.7, 0.7, 1.0 };
static Vec4f STICKY_ROCK_COLOR = { 0.1, 1.0, 0.2, 1.0 };
static Vec4f GOAL_COLOR = { 0.1, 0.1, 0.9, 0.5 };

static const char *ENTITY_TYPE_NAMES[] = {
	"Obstacle",
	"Rock",
	"Sticky Rock",
	"Goal",
	"Player",
};

//FILE: world.cpp

void Entity_init(Entity *, Vec3f, Vec3f, float, const char *, const char *, Vec4f, enum EntityType);
Entity *Game_getEntityByID(Game *, size_t);
void Game_removeEntityByID(Game *, size_t);

void Game_addPlayer(Game *, Vec3f);
void Game_addObstacle(Game *, Vec3f);
void Game_addRock(Game *, Vec3f);
void Game_addStickyRock(Game *, Vec3f);
void Game_addGoal(Game *, Vec3f);

//FILE: levelState.cpp

void Game_initLevelState(Game *);

void Game_levelState(Game *);

//FILE: editorState.cpp

void Game_initEditorState(Game *);

void Game_editorState(Game *);

//FILE: levels.cpp

void Game_writeCurrentLevelStateToFile(Game *, const char *);

void Game_loadLevelFile(Game *, const char *);

void Game_loadLevelByName(Game *, const char *);

#endif
