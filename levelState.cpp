#include "engine/engine.h"
#include "engine/geometry.h"

#include "game.h"

#include <vector>

#include "math.h"
#include "stdio.h"

enum Actions{
	ACTION_MOVE_UP,
	ACTION_MOVE_DOWN,
	ACTION_MOVE_LEFT,
	ACTION_MOVE_RIGHT,
};

bool moving = false;
float moveTime = 0.0;
int timeNotMoving = 0;

int levelWidth = 20;
int levelHeight = 10;

int undoKeyHoldTime = 0;
int UNDO_KEY_START_DELAY = 10;
int UNDO_KEY_REPEAT_DELAY = 5;

std::vector<size_t> entityIDGrid;

std::vector<enum Actions>actionQueue;

std::vector<std::vector<Entity>>undoArray;

int getEntityIDGridIndexFromPos(Vec3f pos){
	return (int)((pos.x + levelWidth / 2) * levelWidth * levelHeight + (pos.y + levelHeight / 2) * levelHeight + (pos.z + levelWidth / 2));
}

Vec3f moveFunc(Vec3f startPos, Vec3f endPos, float t){
	
	Vec3f pos = startPos;
	Vec3f diff = getSubVec3f(endPos, startPos);

	//smooth step
	float x = t * t * (3 - 2 * t);

	Vec3f_add(&pos, getMulVec3fFloat(diff, x));

	if(t >= 1.0){
		pos = endPos;
	}

	return pos;

}

void Game_initLevelState(Game *game_p){

	entityIDGrid.clear();
	undoArray.clear();

	for(int x = 0; x < levelWidth; x++){
		for(int y = 0; y < levelHeight; y++){
			for(int z = 0; z < levelWidth; z++){

				size_t nullID = -1;

				entityIDGrid.push_back(nullID);

			}
		}
	}

}

void Game_levelState(Game *game_p){

	if(Engine_keys[ENGINE_KEY_G].downed){
		game_p->currentGameState = GAME_STATE_EDITOR;
		game_p->mustInitGameState = true;
	}

	game_p->hoveredEntityID = -1;

	game_p->cameraPos = getVec3f(0.0, 6.0, -6.0);
	game_p->cameraRotation = getVec2f(M_PI / 2.0, -M_PI / 4.0);

	if(Engine_keys[ENGINE_KEY_A].downed
	|| (Engine_keys[ENGINE_KEY_A].down)
	&& !moving && timeNotMoving == 3){
		actionQueue.push_back(ACTION_MOVE_LEFT);
	}
	if(Engine_keys[ENGINE_KEY_D].downed
	|| (Engine_keys[ENGINE_KEY_D].down)
	&& !moving && timeNotMoving == 3){
		actionQueue.push_back(ACTION_MOVE_RIGHT);
	}
	if(Engine_keys[ENGINE_KEY_W].downed
	|| (Engine_keys[ENGINE_KEY_W].down)
	&& !moving && timeNotMoving == 3){
		actionQueue.push_back(ACTION_MOVE_UP);
	}
	if(Engine_keys[ENGINE_KEY_S].downed
	|| (Engine_keys[ENGINE_KEY_S].down)
	&& !moving && timeNotMoving == 3){
		actionQueue.push_back(ACTION_MOVE_DOWN);
	}

	if(Engine_keys[ENGINE_KEY_Z].down){
		undoKeyHoldTime++;
	}else{
		undoKeyHoldTime = 0;
	}

	if((Engine_keys[ENGINE_KEY_Z].downed
	|| undoKeyHoldTime >= UNDO_KEY_START_DELAY && undoKeyHoldTime % UNDO_KEY_REPEAT_DELAY == 0)
	&& undoArray.size() > 0){

		game_p->entities.clear();

		for(int i = 0; i < undoArray[undoArray.size() - 1].size(); i++){
			game_p->entities.push_back(undoArray[undoArray.size() - 1][i]);
		}

		undoArray.pop_back();
		actionQueue.clear();
	}

	if(Engine_keys[ENGINE_KEY_R].downed
	&& undoArray.size() > 0){

		game_p->entities.clear();

		for(int i = 0; i < undoArray[0].size(); i++){
			game_p->entities.push_back(undoArray[0][i]);
		}

		undoArray.clear();
		actionQueue.clear();

	}

	if(!moving){
		
		Vec3f playerVelocity = getVec3f(0.0, -1.0, 0.0);

		bool didAction = false;

		if(actionQueue.size() > 0
		&& timeNotMoving >= 2){

			enum Actions action = actionQueue[0];

			if(action == ACTION_MOVE_LEFT){
				playerVelocity.x = -1.0;
			}
			if(action == ACTION_MOVE_RIGHT){
				playerVelocity.x = 1.0;
			}
			if(action == ACTION_MOVE_UP){
				playerVelocity.z = 1.0;
			}
			if(action == ACTION_MOVE_DOWN){
				playerVelocity.z = -1.0;
			}

			didAction = true;

			actionQueue.erase(actionQueue.begin());
		
		}

		//save entities for undo
		if(didAction){
			undoArray.push_back(game_p->entities);
		}

		//check if entities are oub
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity_p = &game_p->entities[i];

			if(fabs(entity_p->pos.x) > levelWidth / 2
			|| fabs(entity_p->pos.y) > levelHeight / 2
			|| fabs(entity_p->pos.z) > levelWidth / 2){

				game_p->entities.erase(game_p->entities.begin() + i);

				i--;
				continue;
			
			}

		}

		//clear grid
		for(int i = 0; i < levelWidth * levelWidth * levelHeight; i++){
			entityIDGrid[i] = -1;
		}

		//put entity IDs into grid
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_PLAYER
			|| entity_p->type == ENTITY_TYPE_OBSTACLE
			|| entity_p->type == ENTITY_TYPE_ROCK
			|| entity_p->type == ENTITY_TYPE_STICKY_ROCK){

				int index = getEntityIDGridIndexFromPos(entity_p->pos);

				entityIDGrid[index] = entity_p->ID;
			
			}

		}

		//check if players are next to sticky rocks
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity1_p = &game_p->entities[i];

			if(entity1_p->type == ENTITY_TYPE_PLAYER){

				for(int j = 0; j < game_p->entities.size(); j++){
					
					Entity *entity2_p = &game_p->entities[j];

					if(entity2_p->type == ENTITY_TYPE_STICKY_ROCK
					&& getMagVec3f(getSubVec3f(entity1_p->pos, entity2_p->pos)) <= 1.001){
						entity2_p->type = ENTITY_TYPE_PLAYER;
						entity2_p->color = PLAYER_COLOR;
						//VERY SHADY AND WEIRD SWITCH. PROBABLY GONNA CAUSE BUGS
						entity1_p = entity2_p;
						j = -1;
						continue;
					}

				}

			}

		}

		//apply basic velocities to entities
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_PLAYER){
				entity_p->velocity = playerVelocity;
			}
			if(entity_p->type == ENTITY_TYPE_ROCK
			|| entity_p->type == ENTITY_TYPE_STICKY_ROCK){
				entity_p->velocity = getVec3f(0.0, -1.0, 0.0);
			}
		
		}

		//check if players can move
		for(int c = 0; c < 3; c++){

			for(int i = 0; i < game_p->entities.size(); i++){
				
				Entity *entity_p = &game_p->entities[i];

				if(entity_p->type == ENTITY_TYPE_PLAYER
				&& fabs(entity_p->velocity[c]) > 0.001){

					Vec3f checkPos = entity_p->pos;

					checkPos[c] += entity_p->velocity[c];

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					while(ID != -1){
						
						if(checkEntity_p->type == ENTITY_TYPE_OBSTACLE){
							playerVelocity[c] = 0.0;
							break;
						}

						checkPos[c] += entity_p->velocity[c];

						ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						checkEntity_p = Game_getEntityByID(game_p, ID);

					}

				}

			}

		}

		//set player velocities second time
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_PLAYER){
				entity_p->velocity = playerVelocity;
			}

		}

		//handle player pushing
		for(int c = 0; c < 3; c++){
			for(int i = 0; i < game_p->entities.size(); i++){
				
				Entity *entity_p = &game_p->entities[i];

				if(entity_p->type == ENTITY_TYPE_PLAYER
				&& fabs(entity_p->velocity[c]) > 0.001){

					Vec3f checkPos = entity_p->pos;

					checkPos[c] += entity_p->velocity[c];

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					bool stopped = false;

					while(ID != -1){
						
						if(checkEntity_p->type == ENTITY_TYPE_OBSTACLE){
							stopped = true;
							break;
						}

						checkEntity_p->velocity[c] = entity_p->velocity[c];

						checkPos[c] += entity_p->velocity[c];

						ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						checkEntity_p = Game_getEntityByID(game_p, ID);

					}

					if(stopped){

						while(!checkEqualsVec3f(checkPos, entity_p->pos, 0.001)){

							checkPos[c] -= entity_p->velocity[c];

							ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
							checkEntity_p = Game_getEntityByID(game_p, ID);
							
							checkEntity_p->velocity[c] = 0.0;
						
						}
					
					}

				}

			}
		}

		//handle friction
		//for(int c = 0; c < 3; c++){
			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity_p = &game_p->entities[i];

				if(!checkEqualsVec3f(entity_p->velocity, getVec3f(0.0, 0.0, 0.0), 0.001)){
					
					Vec3f checkPos = entity_p->pos;
					checkPos.y += 1.0;

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					while(ID != -1){

						if(checkEntity_p->type == ENTITY_TYPE_OBSTACLE){
							break;
						}

						checkEntity_p->velocity.x = entity_p->velocity.x;
						checkEntity_p->velocity.z = entity_p->velocity.z;

						checkPos.y += 1.0;

						ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						checkEntity_p = Game_getEntityByID(game_p, ID);
					
					}

				}

			}
		//}

		//check if players and rocks collide with obstacles
		for(int c = 0; c < 3; c++){
			for(int i = 0; i < game_p->entities.size(); i++){
				
				Entity *entity_p = &game_p->entities[i];

				if((entity_p->type == ENTITY_TYPE_PLAYER
				|| entity_p->type == ENTITY_TYPE_ROCK
				|| entity_p->type == ENTITY_TYPE_STICKY_ROCK)
				&& fabs(entity_p->velocity[c]) > 0.001){

					Vec3f checkPos = entity_p->pos;

					checkPos[c] += entity_p->velocity[c];

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					bool stopped = false;

					while(ID != -1){
						
						if(checkEntity_p->type == ENTITY_TYPE_OBSTACLE){
							stopped = true;
							break;
						}

						checkPos[c] += entity_p->velocity[c];

						ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						checkEntity_p = Game_getEntityByID(game_p, ID);

					}

					if(stopped){

						while(!checkEqualsVec3f(checkPos, entity_p->pos, 0.001)){

							checkPos[c] -= entity_p->velocity[c];

							ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
							checkEntity_p = Game_getEntityByID(game_p, ID);
							
							checkEntity_p->velocity[c] = 0.0;
						
						}

					}
				
				}

			}
		}

		bool entityMoved = false;
		
		//move entities
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			entity_p->startPos = entity_p->pos;
			entity_p->endPos = getAddVec3f(entity_p->pos, entity_p->velocity);

			if(!checkEqualsVec3f(entity_p->velocity, getVec3f(0.0, 0.0, 0.0), 0.001)){
				entityMoved = true;
			}
		
		}

		if(entityMoved){
			moving = true;
			moveTime = 0.0;
		}
	
	}

	if(moving){

		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			entity_p->pos = moveFunc(entity_p->startPos, entity_p->endPos, moveTime);
		
		}

		if(moveTime >= 1.0){
			moving = false;
			timeNotMoving = 0;
		}

		float timeSpeed = 6.7;

		if(actionQueue.size() > 0
		|| Engine_keys[ENGINE_KEY_A].down
		|| Engine_keys[ENGINE_KEY_D].down
		|| Engine_keys[ENGINE_KEY_W].down
		|| Engine_keys[ENGINE_KEY_S].down){
			timeSpeed *= 1.1;
		}
		if(actionQueue.size() > 2){
			timeSpeed *= 1.1;
		}

		moveTime += timeSpeed / 60.0;
	
	}

	timeNotMoving++;

}

