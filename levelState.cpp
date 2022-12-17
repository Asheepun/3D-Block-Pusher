#include "engine/engine.h"
#include "engine/geometry.h"

#include "game.h"

#include "math.h"

void Game_initLevelState(Game *game_p){

}

void Game_levelState(Game *game_p){

	game_p->cameraRotation = getVec2f(M_PI / 2.0, -M_PI / 4.0);

	Vec3f playerVelocity = getVec3f(0.0, 0.0, 0.0);

	if(Engine_keys[ENGINE_KEY_A].downed){
		playerVelocity.x = -1.0;
	}
	if(Engine_keys[ENGINE_KEY_D].downed){
		playerVelocity.x = 1.0;
	}
	if(Engine_keys[ENGINE_KEY_W].downed){
		playerVelocity.z = 1.0;
	}
	if(Engine_keys[ENGINE_KEY_S].downed){
		playerVelocity.z = -1.0;
	}

	for(int i = 0; i < game_p->entities.size(); i++){

		Entity *entity_p = &game_p->entities[i];

		if(entity_p->type == ENTITY_TYPE_PLAYER){
			entity_p->velocity = playerVelocity;
		}
	
	}
	
	for(int i = 0; i < game_p->entities.size(); i++){

		Entity *entity_p = &game_p->entities[i];

		Vec3f_add(&entity_p->pos, entity_p->velocity);
	
	}

}

