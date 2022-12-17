#include "engine/engine.h"
#include "engine/geometry.h"

#include "game.h"

#include "math.h"
#include "stdio.h"

void Game_initEditorState(Game *game_p){

}

void Game_editorState(Game *game_p){

	printf("---\n");

	//handle looking around
	game_p->cameraRotation.x += -Engine_pointer.movement.x * PLAYER_LOOK_SPEED;
	game_p->cameraRotation.y += -Engine_pointer.movement.y * PLAYER_LOOK_SPEED;

	if(game_p->cameraRotation.y > M_PI / 2 - 0.01){
		game_p->cameraRotation.y = M_PI / 2 - 0.01;
	}
	if(game_p->cameraRotation.y < -(M_PI / 2 - 0.01)){
		game_p->cameraRotation.y = -(M_PI / 2 - 0.01);
	}

	//handle camera movement
	game_p->lastCameraPos = game_p->cameraPos;

	if(Engine_keys[ENGINE_KEY_W].down){
		game_p->cameraPos.x += game_p->cameraDirection.x * PLAYER_SPEED;
		game_p->cameraPos.z += game_p->cameraDirection.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_S].down){
		game_p->cameraPos.x += -game_p->cameraDirection.x * PLAYER_SPEED;
		game_p->cameraPos.z += -game_p->cameraDirection.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_SPACE].down){
		game_p->cameraPos.y += PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_SHIFT].down){
		game_p->cameraPos.y += -PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_A].down){
		Vec3f left = getCrossVec3f(game_p->cameraDirection, getVec3f(0, 1.0, 0));
		Vec3f_normalize(&left);
		game_p->cameraPos.x += left.x * PLAYER_SPEED;
		game_p->cameraPos.z += left.z * PLAYER_SPEED;
	}
	if(Engine_keys[ENGINE_KEY_D].down){
		Vec3f right = getCrossVec3f(getVec3f(0, 1.0, 0), game_p->cameraDirection);
		Vec3f_normalize(&right);
		game_p->cameraPos.x += right.x * PLAYER_SPEED;
		game_p->cameraPos.z += right.z * PLAYER_SPEED;
	}

	//check if entity is pointed at
	{
		Vec3f closestIntersectionPoint;
		float closestIntersectionDistance = 100000.0;
		Vec3f hitNormal;
		Entity *hitEntity_p;
		bool hit = false;

		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity_p = &game_p->entities[i];

			for(int j = 0; j < game_p->cubeMesh.length / 3; j++){

				Vec3f t1 = game_p->cubeMesh.vertices[j * 3 + 0];
				Vec3f t2 = game_p->cubeMesh.vertices[j * 3 + 1];
				Vec3f t3 = game_p->cubeMesh.vertices[j * 3 + 2];

				Vec3f intersectionPoint;
				
				if(checkLineToTriangleIntersectionVec3f(
					game_p->cameraPos,
					getAddVec3f(game_p->cameraPos, game_p->cameraDirection),
					getAddVec3f(entity_p->pos, getMulVec3fFloat(t1, 0.5)),
					getAddVec3f(entity_p->pos, getMulVec3fFloat(t2, 0.5)),
					getAddVec3f(entity_p->pos, getMulVec3fFloat(t3, 0.5)),
					&intersectionPoint
				)){

					float distance = getMagVec3f(getSubVec3f(intersectionPoint, game_p->cameraPos));

					if(distance < closestIntersectionDistance){
						closestIntersectionDistance = distance;
						closestIntersectionPoint = intersectionPoint;
						hitEntity_p = entity_p;
						hitNormal = getCrossVec3f(getSubVec3f(t1, t2), getSubVec3f(t1, t3));
						Vec3f_normalize(&hitNormal);
						hit = true;
					}

				}

			}

		
		}

		if(hit){
			game_p->hoveredEntityID = hitEntity_p->ID;
		}else{
			game_p->hoveredEntityID = -1;
		}

		if(Engine_pointer.downed
		&& game_p->hoveredEntityID != -1){
			
			Game_addObstacle(game_p, getAddVec3f(hitEntity_p->pos, hitNormal));

		}

	}

}
