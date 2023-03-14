#ifndef GAME_H_
#define GAME_H_

#include "engine/geometry.h"
#include "engine/3d.h"
#include "engine/igui.h"
#include "engine/strings.h"

#include <vector>

#include "math.h"

enum Direction{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_NORTH,
	DIRECTION_SOUTH,
	DIRECTION_EAST,
	DIRECTION_WEST,
	NUMBER_OF_DIRECTIONS,
};

enum GameState{
	GAME_STATE_LEVEL,
	GAME_STATE_EDITOR,
	GAME_STATE_MENU,
};

enum EntityType{
	ENTITY_TYPE_OBSTACLE,
	ENTITY_TYPE_ROCK,
	ENTITY_TYPE_STICKY_ROCK,
	ENTITY_TYPE_GOAL,
	ENTITY_TYPE_PLAYER,
	ENTITY_TYPE_LEVEL_DOOR,
	ENTITY_TYPE_LEVEL_CABLE,
	ENTITY_TYPE_RISER,
	ENTITY_TYPE_CLONER,
	NUMBER_OF_ENTITY_TYPES,
};

typedef struct Entity{
	size_t ID;
	int playerID;
	enum EntityType type;
	Vec3f pos;
	Vec3f startPos;
	Vec3f endPos;
	Vec3f rotation;
	long int velocityIndex;
	//Vec3f velocity;
	float scale;
	char modelName[SMALL_STRING_SIZE];
	char textureName[SMALL_STRING_SIZE];
	char levelName[SMALL_STRING_SIZE];
	Vec4f color;
	enum Direction pusherDirection;
	bool floating;
}Entity;

struct Particle{
	Vec3f pos;
	Vec3f velocity;
	Vec3f acceleration;
	Vec3f resistance;
	float scale;
	Vec4f color;
	int counter;
};

typedef struct Game{

	std::vector<Entity> entities;
	std::vector<Particle> particles;

	Vec3f playerLevelHubPos;

	int numberOfPlayers;

	VertexMesh cubeMesh;

	bool needToRenderStaticShadows;

	std::vector<Model> models;
	std::vector<Texture> textures;
	TextureAtlas textureAtlas;

	enum GameState currentGameState;
	bool mustInitGameState;

	Vec3f cameraPos;
	Vec3f lastCameraPos;
	Vec2f cameraRotation;
	Vec3f cameraDirection;

	size_t hoveredEntityID;
	//char currentLevel[STRING_SIZE];
	IGUI_TextInputData levelNameTextInputData;
	IGUI_TextInputData levelDoorNameTextInputData;

	char currentLevel[STRING_SIZE];

	std::vector<SmallString> completedLevels;
	std::vector<SmallString> openLevels;

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
static Vec4f LEVEL_DOOR_COLOR = { 1.0, 1.0, 1.0, 0.5 };
static Vec4f COMPLETED_LEVEL_DOOR_COLOR = { 1.0, 1.0, 0.0, 0.5 };
static Vec4f OPEN_LEVEL_DOOR_COLOR = { 0.0, 1.0, 0.0, 0.5 };
static Vec4f LEVEL_CABLE_COLOR = { 1.0, 1.0, 1.0, 0.5 };
static Vec4f RISER_COLOR = { 1.0, 0.6, 0.0, 0.5 };
static Vec4f CLONER_COLOR = { 1.0, 0.0, 1.0, 0.5 };

//static Vec3f STANDARD_CAMERA_POS = { 0.0, 6.0, -6.0 };
//static Vec2f STANDARD_CAMERA_ROTATION = { M_PI / 2.0, -M_PI / 4.0 };
static Vec3f STANDARD_CAMERA_POS = { 0.0, 8.0, -8.0 };
static Vec2f STANDARD_CAMERA_ROTATION = { M_PI / 2.0, -M_PI / 4.0 };

static const char *ENTITY_TYPE_NAMES[] = {
	"Obstacle",
	"Rock",
	"Sticky Rock",
	"Goal",
	"Player",
	"Level Door",
	"Level Cable",
	"Pusher",
	"Cloner",
};

static const char *DIRECTION_NAMES[] = {
	"UP",
	"DOWN",
	"NORTH",
	"SOUTH",
	"EAST",
	"WEST",
};

static Vec3f DIRECTION_VECTORS[] = {
	0.0, 1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, -1.0,
	1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
};

static int MAX_NUMBER_OF_PARTICLES = 1024;

//FILE: world.cpp

void Entity_init(Entity *, Vec3f, Vec3f, float, const char *, const char *, Vec4f, enum EntityType);
Entity *Game_getEntityByID(Game *, size_t);
void Game_removeEntityByID(Game *, size_t);

void Game_addPlayer(Game *, Vec3f);
void Game_addObstacle(Game *, Vec3f);
void Game_addRock(Game *, Vec3f);
void Game_addStickyRock(Game *, Vec3f);
void Game_addGoal(Game *, Vec3f);
void Game_addLevelDoor(Game *, Vec3f, const char *);
void Game_addLevelCable(Game *, Vec3f);
void Game_addRiser(Game *, Vec3f);
void Game_addCloner(Game *, Vec3f);

void Particle_init(Particle *);

bool isStaticEntity(Entity);

//FILE: levelState.cpp

void Game_initLevelState(Game *);

void Game_levelState(Game *);

//FILE: editorState.cpp

void Game_initEditorState(Game *);

void Game_editorState(Game *);

//FILE: menuState.cpp

void Game_initMenuState(Game *);

void Game_menuState(Game *);

//FILE: levels.cpp

void Game_writeCurrentLevelStateToFile(Game *, const char *);

void Game_loadLevelFile(Game *, const char *);

void Game_loadLevelByName(Game *, const char *);

#endif
