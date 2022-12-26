#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/strings.h"

#include "game.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include <cstring>
#include <vector>

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

	String_set(entity_p->levelName, "", SMALL_STRING_SIZE);
	entity_p->velocity = getVec3f(0.0, 0.0, 0.0);

}

Entity *Game_getEntityByID(Game *game_p, size_t ID){

	for(int i = 0; i < game_p->entities.size(); i++){
		
		if(game_p->entities[i].ID == ID){
			return &game_p->entities[i];
		}

	}

	return NULL;

}

void Game_removeEntityByID(Game *game_p, size_t ID){

	for(int i = 0; i < game_p->entities.size(); i++){
		
		if(game_p->entities[i].ID == ID){
			game_p->entities.erase(game_p->entities.begin() + i);
			break;
		}

	}

}

void Game_addPlayer(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", PLAYER_COLOR, ENTITY_TYPE_PLAYER);

	game_p->entities.push_back(entity);

}

void Game_addObstacle(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", OBSTACLE_COLOR, ENTITY_TYPE_OBSTACLE);

	game_p->entities.push_back(entity);

}

void Game_addRock(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", ROCK_COLOR, ENTITY_TYPE_ROCK);

	game_p->entities.push_back(entity);

}

void Game_addStickyRock(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", STICKY_ROCK_COLOR, ENTITY_TYPE_STICKY_ROCK);

	game_p->entities.push_back(entity);

}

void Game_addGoal(Game *game_p, Vec3f pos){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", GOAL_COLOR, ENTITY_TYPE_GOAL);

	game_p->entities.push_back(entity);

}

void Game_addLevelDoor(Game *game_p, Vec3f pos, const char *levelName){

	Entity entity;

	Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", LEVEL_DOOR_COLOR, ENTITY_TYPE_LEVEL_DOOR);

	String_set(entity.levelName, levelName, SMALL_STRING_SIZE);

	game_p->entities.push_back(entity);

}


