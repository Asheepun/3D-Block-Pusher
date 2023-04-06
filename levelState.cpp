#include "engine/engine.h"
#include "engine/geometry.h"

#include "game.h"

#include <vector>
#include <algorithm>

#include "math.h"
#include "stdio.h"
#include "string.h"

enum Actions{
	ACTION_MOVE_UP,
	ACTION_MOVE_DOWN,
	ACTION_MOVE_LEFT,
	ACTION_MOVE_RIGHT,
};

bool UNLOCK_LEVELS = true;

bool moving = false;
float moveTime = 0.0;
int timeNotMoving = 0;

int levelWidth = 200;
int levelHeight = 16;

int undoKeyHoldTime = 0;
int UNDO_KEY_START_DELAY = 10;
int UNDO_KEY_REPEAT_DELAY = 5;

int particleCounter = 0;

std::vector<size_t> entityIDGrid;

std::vector<enum Actions>actionQueue;
enum Actions lastAction;

std::vector<std::vector<Entity>>undoArray;
std::vector<Entity>staticUndoEntities;

std::vector<Vec3f> velocities;

int getEntityIDGridIndexFromPos(Vec3f pos){
	return (int)((pos.x + levelWidth / 2) * levelWidth * levelHeight + (pos.y + levelHeight / 2) * levelWidth + (pos.z + levelWidth / 2));
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

void updateNumberOfPlayers(Game *game_p){

	game_p->numberOfPlayers = 0;

	for(int i = 0; i < game_p->entities.size(); i++){
		if(game_p->entities[i].type == ENTITY_TYPE_PLAYER
		&& game_p->entities[i].playerID >= game_p->numberOfPlayers){
			game_p->numberOfPlayers = game_p->entities[i].playerID + 1;
		}
	}

}

void resetEntityGrid(Game *game_p){

	entityIDGrid.clear();

	for(int x = 0; x < levelWidth; x++){
		for(int y = 0; y < levelHeight; y++){
			for(int z = 0; z < levelWidth; z++){

				size_t nullID = -1;

				entityIDGrid.push_back(nullID);

			}
		}
	}

}

void Game_initLevelState(Game *game_p){

	game_p->particles.clear();

	game_p->numberOfPlayers = 0;

	for(int i = 0; i < game_p->entities.size(); i++){
		if(game_p->entities[i].type == ENTITY_TYPE_PLAYER){
			game_p->entities[i].playerID = game_p->numberOfPlayers;
			game_p->numberOfPlayers++;
		}
	}

	updateNumberOfPlayers(game_p);
	resetEntityGrid(game_p);

	undoArray.clear();

	if(strcmp(game_p->currentLevel, "levelhub") == 0
	|| strcmp(game_p->currentLevel, "levelhub2") == 0){

		//color completed level doors
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_LEVEL_DOOR){

				bool completed = false;
				bool open = false;

				for(int j = 0; j < game_p->completedLevels.size(); j++){
					if(strcmp(entity_p->levelName, game_p->completedLevels[j]) == 0){
						completed = true;
					}
				}
				for(int j = 0; j < game_p->openLevels.size(); j++){
					if(strcmp(entity_p->levelName, game_p->openLevels[j]) == 0){
						open = true;
					}
				}

				if(completed){
					entity_p->color = COMPLETED_LEVEL_DOOR_COLOR;
				}else if(open){
					entity_p->color = OPEN_LEVEL_DOOR_COLOR;
				}else{
					entity_p->color = LEVEL_DOOR_COLOR;
				}
			
			}

		}
	
	}

	game_p->needToRenderStaticShadows = true;

	printf("inited level state!\n");

}

void Game_levelState(Game *game_p){

	printf("---\n");

	if(Engine_keys[ENGINE_KEY_G].downed){
		game_p->currentGameState = GAME_STATE_EDITOR;
		game_p->mustInitGameState = true;
		return;
	}
	if(Engine_keys[ENGINE_KEY_ESCAPE].downed){
		game_p->currentGameState = GAME_STATE_MENU;
		game_p->mustInitGameState = true;
		return;
	}
	game_p->hoveredEntityID = -1;

	//game_p->cameraPos = getVec3f(0.0, 6.0, -6.0);
	//game_p->cameraRotation = getVec2f(M_PI / 2.0, -M_PI / 4.0);
	game_p->cameraPos = STANDARD_CAMERA_POS;
	game_p->cameraRotation = STANDARD_CAMERA_ROTATION;

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

		for(int i = 0; i < staticUndoEntities.size(); i++){
			game_p->entities.push_back(staticUndoEntities[i]);
		}

		for(int i = 0; i < undoArray[undoArray.size() - 1].size(); i++){
			game_p->entities.push_back(undoArray[undoArray.size() - 1][i]);
		}

		undoArray.pop_back();
		actionQueue.clear();

		updateNumberOfPlayers(game_p);
		resetEntityGrid(game_p);

		return;

	}

	if(Engine_keys[ENGINE_KEY_R].downed
	&& undoArray.size() > 0){

		game_p->entities.clear();

		for(int i = 0; i < staticUndoEntities.size(); i++){
			game_p->entities.push_back(staticUndoEntities[i]);
		}

		for(int i = 0; i < undoArray[0].size(); i++){
			game_p->entities.push_back(undoArray[0][i]);
		}

		undoArray.clear();
		actionQueue.clear();

		updateNumberOfPlayers(game_p);
		resetEntityGrid(game_p);

		return;

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

			lastAction = actionQueue[0];
			actionQueue.erase(actionQueue.begin());
		
		}

		std::vector<Entity> undoEntitiesCopy;

		if(didAction){
			//update static undo entities
			staticUndoEntities.clear();

			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity_p = &game_p->entities[i];

				if(isStaticEntity(*entity_p)){
					staticUndoEntities.push_back(*entity_p);
				}
			}

			//make copy of non static entities for undo
			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity_p = &game_p->entities[i];

				if(!isStaticEntity(*entity_p)){
					undoEntitiesCopy.push_back(*entity_p);
				}

			}
		}

		//check if player collides with level doors
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity1_p = &game_p->entities[i];

			if(entity1_p->type == ENTITY_TYPE_PLAYER){

				for(int j = 0; j < game_p->entities.size(); j++){

					Entity *entity2_p = &game_p->entities[j];

					if(i != j
					&& entity2_p->type == ENTITY_TYPE_LEVEL_DOOR
					&& checkEqualsVec3f(entity1_p->pos, entity2_p->pos, 0.001)
					&& !(strcmp(entity2_p->levelName, "") == 0)){

						//check if level door is open
						bool unlocked  = false;
						if(UNLOCK_LEVELS){
							unlocked = true;
						}
						for(int k = 0; k < game_p->openLevels.size(); k++){
							if(strcmp(game_p->openLevels[k], entity2_p->levelName) == 0){
								unlocked = true;
							}
						}

						if(unlocked){

							game_p->playerLevelHubPos = entity1_p->pos;
							game_p->playerLevelHubPos.z -= 1.0;

							game_p->mustInitGameState = true;
							Game_loadLevelByName(game_p, entity2_p->levelName);

							return;
						
						}

					}
					
				}
			
			}

		}

		//check if player covers goals
		{
			int numberOfGoals = 0;
			int numberOfCoveredGoals = 0;

			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity1_p = &game_p->entities[i];

				if(entity1_p->type == ENTITY_TYPE_GOAL){

					numberOfGoals++;

					for(int j = 0; j < game_p->entities.size(); j++){

						Entity *entity2_p = &game_p->entities[j];

						if(i != j
						&& entity2_p->type == ENTITY_TYPE_PLAYER
						&& checkEqualsVec3f(entity1_p->pos, entity2_p->pos, 0.001)){
							numberOfCoveredGoals++;
							break;
						}

					}

				}

			}

			//handle completing level
			if(numberOfCoveredGoals == numberOfGoals
			&& numberOfGoals > 0){

				SmallString completedLevelName;
				String_set(completedLevelName.value, game_p->currentLevel, SMALL_STRING_SIZE);

				game_p->completedLevels.push_back(completedLevelName);

				game_p->mustInitGameState = true;
				Game_loadLevelByName(game_p, "levelhub2");
				
				//set player pos
				for(int i = 0; i < game_p->entities.size(); i++){

					Entity *entity1_p = &game_p->entities[i];

					if(entity1_p->type == ENTITY_TYPE_PLAYER){
						entity1_p->pos = game_p->playerLevelHubPos;
						break;
					}

				}

				//unlock connected level doors
				for(int i = 0; i < game_p->entities.size(); i++){

					Entity *entity1_p = &game_p->entities[i];

					if(entity1_p->type == ENTITY_TYPE_LEVEL_DOOR
					&& strcmp(entity1_p->levelName, completedLevelName) == 0){

						for(int j = 0; j < game_p->entities.size(); j++){

							Entity *entity2_p = &game_p->entities[j];

							int lastK = j;
							int secondLastK = j;

							if(entity2_p->type == ENTITY_TYPE_LEVEL_CABLE
							&& getMagVec3f(getSubVec3f(entity1_p->pos, entity2_p->pos)) < 1.001){

								entity2_p->color = COMPLETED_LEVEL_DOOR_COLOR;
								
								for(int k = 0; k < game_p->entities.size(); k++){

									Entity *entity3_p = &game_p->entities[k];

									if(entity3_p->type == ENTITY_TYPE_LEVEL_CABLE
									&& getMagVec3f(getSubVec3f(entity2_p->pos, entity3_p->pos)) < 1.001
									&& k != lastK
									&& k != secondLastK){

										entity3_p->color = COMPLETED_LEVEL_DOOR_COLOR;

										entity2_p = entity3_p;
										secondLastK = lastK;
										lastK = k;
										k = -1;
										continue;

									}

									if(entity3_p->type == ENTITY_TYPE_LEVEL_DOOR
									&& getMagVec3f(getSubVec3f(entity2_p->pos, entity3_p->pos)) < 1.001
									&& k != i){

										SmallString name;
										String_set(name, entity3_p->levelName, SMALL_STRING_SIZE);
										
										//check if level is not already open
										bool alreadyOpen = false;
										for(int l = 0; l < game_p->openLevels.size(); l++){
											if(strcmp(game_p->openLevels[l], name) == 0){
												alreadyOpen = true;
											}
										}

										if(!alreadyOpen){
											game_p->openLevels.push_back(name);
										}

									}

									if(entity3_p->type == ENTITY_TYPE_OBSTACLE
									&& getMagVec3f(getSubVec3f(entity2_p->pos, entity3_p->pos)) < 1.001
									&& checkEqualsFloat(entity2_p->pos.y, entity3_p->pos.y, 0.001)
									&& k != i){
										
										entity3_p->type = ENTITY_TYPE_ROCK;
										entity3_p->color = ROCK_COLOR;

									}

								}

							}

						}
						
					}
					
				}

				return;

			}
		}

		//check if entities are oub
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity_p = &game_p->entities[i];

			if(fabs(entity_p->pos.x) > levelWidth / 2
			|| fabs(entity_p->pos.y) > levelHeight / 2
			|| entity_p->pos.y < -3.0
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

		//handle cloners
		{
			int numberOfEmptyCloners = 0;
			int numberOfFilledCloners = 0;
			Entity *cloneEntity_p = NULL;
			bool onlyOneCloneType = true;

			//check what is in cloners
			for(int i = 0; i < game_p->entities.size(); i++){
				
				Entity *entity1_p = &game_p->entities[i];

				if(entity1_p->type == ENTITY_TYPE_CLONER){

					int index = getEntityIDGridIndexFromPos(entity1_p->pos);
					
					if(entityIDGrid[index] == -1){
						numberOfEmptyCloners++;
					}else{

						numberOfFilledCloners++;
						
						Entity *entity2_p = Game_getEntityByID(game_p, entityIDGrid[index]);
						if(cloneEntity_p != NULL && cloneEntity_p->type != entity2_p->type){
							onlyOneCloneType = false;
						}
						cloneEntity_p = entity2_p;

					}

				}

			}

			//clone entities if necessary
			if(numberOfEmptyCloners > 0
			&& numberOfFilledCloners > 0
			&& onlyOneCloneType){

				for(int i = 0; i < game_p->entities.size(); i++){
					
					Entity *entity1_p = &game_p->entities[i];

					if(entity1_p->type == ENTITY_TYPE_CLONER){

						int index = getEntityIDGridIndexFromPos(entity1_p->pos);
						
						if(entityIDGrid[index] == -1){
							
							Entity entity2;
							Entity_init(&entity2, entity1_p->pos, cloneEntity_p->rotation, cloneEntity_p->scale, cloneEntity_p->modelName, cloneEntity_p->textureName, cloneEntity_p->color, cloneEntity_p->type);
							entity2.floating = cloneEntity_p->floating;

							if(entity2.type == ENTITY_TYPE_PLAYER){
								entity2.playerID = game_p->numberOfPlayers;
								game_p->numberOfPlayers++;
							}

							game_p->entities.push_back(entity2);

							//place entity into entity ID grid
							entityIDGrid[index] = entity2.ID;

						}

					}
				
				}

			}
		
		}

		//check if players are next to sticky rocks
		{
			bool sticked = true;
			while(sticked){

				sticked = false;

				for(int i = 0; i < game_p->entities.size(); i++){
					
					Entity *entity1_p = &game_p->entities[i];

					if(entity1_p->type == ENTITY_TYPE_STICKY_ROCK){

						std::vector<int>playerIDs;

						for(int j = 0; j < game_p->entities.size(); j++){
							
							Entity *entity2_p = &game_p->entities[j];

							if(entity2_p->type == ENTITY_TYPE_PLAYER
							&& getMagVec3f(getSubVec3f(entity1_p->pos, entity2_p->pos)) < 1.001){

								sticked = true;

								playerIDs.push_back(entity2_p->playerID);

							}

						}

						if(playerIDs.size() > 0){

							entity1_p->type = ENTITY_TYPE_PLAYER;
							entity1_p->color = PLAYER_COLOR;
							entity1_p->playerID = playerIDs[0];

							//connect players who were next to the same sticky rock
							for(int j = 1; j < playerIDs.size(); j++){
								for(int k = 0; k < game_p->entities.size(); k++){
							
									Entity *entity2_p = &game_p->entities[k];

									if(entity2_p->type == ENTITY_TYPE_PLAYER
									&& entity2_p->playerID == playerIDs[j]){
										entity2_p->playerID = playerIDs[0];
									}

								}
							}

						}

					}

				}
			
			}

		}

		/*
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_PLAYER){
				printf("index: %i, playerID: %i\n", i, entity_p->playerID);
			}

		}
		*/

		//clear velocities
		velocities.clear();

		//create velocities for players
		for(int i = 0; i < game_p->numberOfPlayers; i++){
			velocities.push_back(playerVelocity);
		}

		//create and set entity velocities and velocity indices
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			entity_p->velocityIndex = -1;

			if(entity_p->type == ENTITY_TYPE_ROCK
			|| entity_p->type == ENTITY_TYPE_STICKY_ROCK){

				Vec3f velocity = getVec3f(0.0, -1.0, 0.0);

				velocities.push_back(velocity);

				entity_p->velocityIndex = velocities.size() - 1;
			
			}

			if(entity_p->type == ENTITY_TYPE_PLAYER){
				entity_p->velocityIndex = entity_p->playerID;
			}

		}

		//create blocked checking
		bool blockedVelocityDirections[velocities.size()][6];
		for(int i = 0; i < velocities.size(); i++){
			for(int j = 0; j < 6; j++){
				blockedVelocityDirections[i][j] = false;
			}
		}

		//handle floating entities
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->floating){
				velocities[entity_p->velocityIndex].y = 0.0;
				blockedVelocityDirections[entity_p->velocityIndex][3] = true;
			}

		}

		//handle entities colliding with risers
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity1_p = &game_p->entities[i];

			if(entity1_p->type == ENTITY_TYPE_RISER){

				for(int j = 0; j < game_p->entities.size(); j++){
					
					Entity *entity2_p = &game_p->entities[j];

					if(entity1_p->ID != entity2_p->ID
					&& (entity2_p->type == ENTITY_TYPE_PLAYER
					|| entity2_p->type == ENTITY_TYPE_ROCK
					|| entity2_p->type == ENTITY_TYPE_STICKY_ROCK)
					&& checkEqualsVec3f(entity1_p->pos, entity2_p->pos, 0.001)){

						for(int c = 0; c < 3; c++){
							if(fabs(DIRECTION_VECTORS[entity1_p->pusherDirection][c]) > 0.001){
								velocities[entity2_p->velocityIndex][c] = DIRECTION_VECTORS[entity1_p->pusherDirection][c];
							}
						}

					}

				}
			
			}

		}

		//block entities next to risers
		for(int i = 0; i < game_p->entities.size(); i++){
			
			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_RISER){

				Vec3f checkPos = entity_p->pos;
				Vec3f_add(&checkPos, DIRECTION_VECTORS[entity_p->pusherDirection]);

				size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];

				if(ID != -1){

					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					int c = DIRECTION_COORDINATE_INDICES[entity_p->pusherDirection];

					enum Direction searchVelocityDirection = (enum Direction)(c * 2 + (entity_p->pusherDirection + 1) % 2);

					//searchVelocityDirection = (enum Direction)3;
					printf("check\n");

					velocities[checkEntity_p->velocityIndex][c] = 0.0;
					blockedVelocityDirections[checkEntity_p->velocityIndex][searchVelocityDirection] = true;
					
				}
				
			}

		}

		int numberOfPushingIterations = 10;

		//iterate pushing and friction handling
		for(int iterations = 0; iterations < numberOfPushingIterations; iterations++){

			//check if pushing can be done and set velocities accordingly
			for(int c = 0; c < 3; c++){

				for(int i = 0; i < game_p->entities.size(); i++){
					
					Entity *entity_p = &game_p->entities[i];

					if((entity_p->type == ENTITY_TYPE_PLAYER
					|| entity_p->type == ENTITY_TYPE_ROCK
					|| entity_p->type == ENTITY_TYPE_STICKY_ROCK)
					&& fabs(velocities[entity_p->velocityIndex][c]) > 0.001){

						float searchVelocity = velocities[entity_p->velocityIndex][c];
						enum Direction searchVelocityDirection = (enum Direction)(c * 2 + (int)ceil(-searchVelocity / 2.0));

						Vec3f checkPos = entity_p->pos;

						checkPos[c] += searchVelocity;

						size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

						bool stopped = false;

						while(ID != -1){
							
							if(checkEntity_p->type == ENTITY_TYPE_OBSTACLE
							|| blockedVelocityDirections[checkEntity_p->velocityIndex][searchVelocityDirection]){
								stopped = true;
								break;
							}

							checkPos[c] += searchVelocity;

							ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
							checkEntity_p = Game_getEntityByID(game_p, ID);

						}

						if(stopped){

							velocities[entity_p->velocityIndex][c] = 0.0;
							blockedVelocityDirections[entity_p->velocityIndex][searchVelocityDirection] = true;

						}

					}

				}
			}

			//stop entities from pushing into risers
			/*
			for(int i = 0; i < game_p->entities.size(); i++){
				
				Entity *entity_p = &game_p->entities[i];

				if(entity_p->type == ENTITY_TYPE_RISER){

					Vec3f pusherVector = DIRECTION_VECTORS[entity_p->pusherDirection];
					int c = DIRECTION_COORDINATE_INDICES[entity_p->pusherDirection];

					Vec3f checkPos = entity_p->pos;

					Vec3f_add(&checkPos, pusherVector);

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					while(ID != -1
					&& (checkEqualsFloat(pusherVector[c] * -1.0, velocities[checkEntity_p->velocityIndex][c], 0.001)
					|| checkEqualsFloat(0.0, velocities[checkEntity_p->velocityIndex][c], 0.001))){

						velocities[checkEntity_p->velocityIndex][c] = 0.0;
						
						Vec3f_add(&checkPos, pusherVector);

						ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						checkEntity_p = Game_getEntityByID(game_p, ID);

					}

				}

			}
			*/

			//handle pushing
			for(int c = 0; c < 3; c++){

				for(int i = 0; i < game_p->entities.size(); i++){
					
					Entity *entity_p = &game_p->entities[i];

					if((entity_p->type == ENTITY_TYPE_PLAYER
					|| entity_p->type == ENTITY_TYPE_ROCK
					|| entity_p->type == ENTITY_TYPE_STICKY_ROCK)
					&& fabs(velocities[entity_p->velocityIndex][c]) > 0.001){

						float searchVelocity = velocities[entity_p->velocityIndex][c];

						Vec3f checkPos = entity_p->pos;

						checkPos[c] += searchVelocity;

						size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
						Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

						bool stopped = false;

						while(ID != -1){
							
							velocities[checkEntity_p->velocityIndex][c] = velocities[entity_p->velocityIndex][c];

							checkPos[c] += searchVelocity;

							ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
							checkEntity_p = Game_getEntityByID(game_p, ID);

						}

					}

				}
			}

			//skip friction for last iteration
			if(iterations == numberOfPushingIterations - 1){
				continue;
			}

			std::vector<int> frictionedEntityIDs;

			//handle friction
			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity_p = &game_p->entities[i];

				bool alreadyFrictioned = false;
				for(int j = 0; j < frictionedEntityIDs.size(); j++){
					if(entity_p->ID == frictionedEntityIDs[j]){
						alreadyFrictioned = true;
					}
				}

				if(!alreadyFrictioned
				&& (!checkEqualsFloat(velocities[entity_p->velocityIndex].x, 0.0, 0.001)
				|| !checkEqualsFloat(velocities[entity_p->velocityIndex].z, 0.0, 0.001))
				&& (entity_p->type == ENTITY_TYPE_PLAYER
				|| entity_p->type == ENTITY_TYPE_ROCK
				|| entity_p->type == ENTITY_TYPE_STICKY_ROCK)){
					
					Vec3f checkPos = entity_p->pos;
					checkPos.y += 1.0;

					size_t ID = entityIDGrid[getEntityIDGridIndexFromPos(checkPos)];
					Entity *checkEntity_p = Game_getEntityByID(game_p, ID);

					if(ID != -1
					&& checkEntity_p->type != ENTITY_TYPE_OBSTACLE
					&& checkEntity_p->type != ENTITY_TYPE_PLAYER
					&& checkEqualsVec3f(velocities[checkEntity_p->velocityIndex], getVec3f(0.0, 0.0, 0.0), 0.001)){

						velocities[checkEntity_p->velocityIndex].x = velocities[entity_p->velocityIndex].x;
						velocities[checkEntity_p->velocityIndex].z = velocities[entity_p->velocityIndex].z;

						frictionedEntityIDs.push_back(checkEntity_p->ID);

					}

				}

			}
		
		}

		//handle velocity direction priorities
		{
			bool entityMovesY = false;
			for(int i = 0; i < game_p->entities.size(); i++){

				Entity *entity_p = &game_p->entities[i];

				if(entity_p->velocityIndex != -1
				&& fabs(velocities[entity_p->velocityIndex].y) > 0.001){
					entityMovesY = true;
					break;
				}

			}

			if(entityMovesY){
				for(int i = 0; i < game_p->entities.size(); i++){

					Entity *entity_p = &game_p->entities[i];

					if(entity_p->velocityIndex != -1){
						velocities[entity_p->velocityIndex].x = 0.0;
						velocities[entity_p->velocityIndex].z = 0.0;
					}

				}
			}
		
		}

		bool entityMoved = false;
		
		//move entities
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			Vec3f velocity = getVec3f(0.0, 0.0, 0.0);

			if(entity_p->velocityIndex != -1){
				velocity = velocities[entity_p->velocityIndex];
			}

			entity_p->startPos = entity_p->pos;
			entity_p->endPos = getAddVec3f(entity_p->pos, velocity);

			if(!checkEqualsVec3f(velocity, getVec3f(0.0, 0.0, 0.0), 0.001)){
				entityMoved = true;
			}
		
		}

		if(entityMoved){
			moving = true;
			moveTime = 0.0;
		}

		//save entities for undo
		if(didAction
		&& entityMoved){
			undoArray.push_back(undoEntitiesCopy);
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

	//set and riser player texture
	for(int i = 0; i < game_p->entities.size(); i++){

		Entity *entity_p = &game_p->entities[i];

		if(entity_p->type == ENTITY_TYPE_PLAYER){
			String_set(entity_p->textureName, "player", SMALL_STRING_SIZE);
		}
	
	}

	game_p->lightPos = getVec3f(-10.0, 20.0, -10.0);

	//make camera and shadow map follow player if in level hub
	if(strcmp(game_p->currentLevel, "levelhub") == 0
	|| strcmp(game_p->currentLevel, "levelhub2") == 0){

		//find player position
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			if(entity_p->type == ENTITY_TYPE_PLAYER){

				Vec3f_add(&game_p->cameraPos, entity_p->pos);

				game_p->lightPos = entity_p->pos;
				game_p->lightPos.y = 20.0;
				game_p->lightPos.x -= 10.0;
				game_p->lightPos.z -= 5.0;

				break;
			}

		}

	}

	//add particles to floating entities
	for(int i = 0; i < game_p->entities.size(); i++){

		Entity *entity_p = &game_p->entities[i];
		
		if(entity_p->floating
		&& particleCounter % 2 == 0){

			Particle particle;

			Particle_init(&particle);

			Vec2f direction = getVec2f(getRandom() - 0.5, getRandom() - 0.5);
			Vec2f_normalize(&direction);
			Vec2f_mulByFloat(&direction, 1.0 - 0.2 * fabs(fabs(direction.x) - fabs(direction.y)));//make direction more like a square

			Vec3f pos = getVec3f(entity_p->pos.x + direction.x * entity_p->scale * 0.8, entity_p->pos.y - entity_p->scale + getRandom() * entity_p->scale * 0.1, entity_p->pos.z + direction.y * entity_p->scale * 0.8);

			float accelerationValue = 0.03;
			Vec3f acceleration = getVec3f(direction.x * accelerationValue, (getRandom() - 0.5) * 0.01, direction.y * accelerationValue);

			float resistanceValue = 0.93;
			Vec3f resistance = getVec3f(resistanceValue, resistanceValue, resistanceValue);

			particle.pos = pos;
			particle.scale = 0.05;
			particle.velocity = acceleration;
			particle.resistance = resistance;

			game_p->particles.push_back(particle);
			
		}

	}

	particleCounter++;

	//handle particles
	for(int i = 0; i < game_p->particles.size(); i++){

		Particle *particle_p = &game_p->particles[i];

		Vec3f_add(&particle_p->velocity, particle_p->acceleration);

		Vec3f_mulByVec3f(&particle_p->velocity, particle_p->resistance);

		Vec3f_add(&particle_p->pos, particle_p->velocity);

		float fadeAwayTime = 60.0;

		if(particleCounter > fadeAwayTime / 2.0){
			//particle_p->color.w = 1.0 - (float)(particle_p->counter - fadeAwayTime / 2.0) / fadeAwayTime;
		}

		particle_p->counter++;

		if(particle_p->counter > fadeAwayTime){
			//remove particle
			game_p->particles.erase(game_p->particles.begin() + i);
			i--;
			continue;
		}
	
	}

}

