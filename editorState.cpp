#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/igui.h"
#include "engine/strings.h"
#include "engine/files.h"

#include "game.h"

#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "dirent.h"

enum EditingMode{
	EDITING_MODE_PLACE,
	EDITING_MODE_REMOVE,
	EDITING_MODE_EDIT,
}EditingMode;

enum InterfaceMode{
	INTERFACE_MODE_MENU,
	INTERFACE_MODE_EDITING,
}InterfaceMode;

enum EntityType currentEntityType;
enum EditingMode currentEditingMode;
enum InterfaceMode currentInterfaceMode;

int openLevelScroll = 0;
bool openingLevel = false;
bool firstInit = false;

bool editingEntity;
size_t editingEntityID;

void Game_initEditorState(Game *game_p){

	Game_loadLevelByName(game_p, "working");

	currentInterfaceMode = INTERFACE_MODE_MENU;

	IGUI_TextInputData_init(&game_p->levelDoorNameTextInputData, "", 0);
	editingEntity = false;
	editingEntityID = -1;

	if(!firstInit){

		long int fileSize;
		char *lastOpenedLevelName = getFileData_mustFree("lastOpenedLevelName.txt", &fileSize);
		for(int i = 0; i < fileSize; i++){
			if(lastOpenedLevelName[i] == *"}"){
				lastOpenedLevelName[i] = *"\0";
			}
		}
		//lastOpenedLevelName[strlen(lastOpenedLevelName) - 1] = *"\0";

		IGUI_TextInputData_init(&game_p->levelNameTextInputData, lastOpenedLevelName, strlen(lastOpenedLevelName));
		//IGUI_TextInputData_init(&game_p->levelNameTextInputData, "", 0);

		free(lastOpenedLevelName);

		Game_loadLevelByName(game_p, game_p->levelNameTextInputData.text);

		Game_writeCurrentLevelStateToFile(game_p, "levels/working.level");

		currentEntityType = ENTITY_TYPE_OBSTACLE;
		currentEditingMode = EDITING_MODE_PLACE;

		firstInit = true;
	}

}

void Game_editorState(Game *game_p){

	if(!game_p->levelNameTextInputData.focused
	&& !game_p->levelDoorNameTextInputData.focused){
		if(Engine_keys[ENGINE_KEY_G].downed){
			game_p->currentGameState = GAME_STATE_LEVEL;
			game_p->mustInitGameState = true;
			return;
		}

		if(Engine_keys[ENGINE_KEY_C].downed){
			if(currentInterfaceMode == INTERFACE_MODE_MENU){
				currentInterfaceMode = INTERFACE_MODE_EDITING;
			}else if(currentInterfaceMode == INTERFACE_MODE_EDITING){
				currentInterfaceMode = INTERFACE_MODE_MENU;
			}
		}

		if(Engine_keys[ENGINE_KEY_B].downed){
			//game_p->cameraPos = getVec3f(0.0, 6.0, -6.0);
			//game_p->cameraRotation = getVec2f(M_PI / 2.0, -M_PI / 4.0);
			game_p->cameraPos = STANDARD_CAMERA_POS;
			game_p->cameraRotation = STANDARD_CAMERA_ROTATION;
		}
	}

	bool madeEdit = false;

	//handle GUI
	if(currentInterfaceMode == INTERFACE_MODE_MENU){

		Engine_setFPSMode(false);

		Vec2f pos = getVec2f(20, 20);
		if(IGUI_textButton_click("Place", pos, 100, currentEditingMode == EDITING_MODE_PLACE)){
			currentEditingMode = EDITING_MODE_PLACE;
		}
		pos.y += 120;
		if(IGUI_textButton_click("Remove", pos, 100, currentEditingMode == EDITING_MODE_REMOVE)){
			currentEditingMode = EDITING_MODE_REMOVE;
		}
		pos.y += 120;
		if(IGUI_textButton_click("Edit", pos, 100, currentEditingMode == EDITING_MODE_EDIT)){
			currentEditingMode = EDITING_MODE_EDIT;
		}

		pos = getVec2f(400, 20);

		if(currentEditingMode == EDITING_MODE_PLACE){
			for(int i = 0; i < NUMBER_OF_ENTITY_TYPES; i++){
				
				if(IGUI_textButton_click(ENTITY_TYPE_NAMES[i], pos, 80, currentEntityType == i)){
					currentEntityType = (enum EntityType)i;
				}

				pos.y += 100;

			}
		}

		if(currentEditingMode != EDITING_MODE_EDIT){
			editingEntity = false;
			editingEntityID = -1;
		}
		if(currentEditingMode == EDITING_MODE_EDIT
		&& editingEntity){

			Entity *editingEntity_p = Game_getEntityByID(game_p, editingEntityID);

			if(editingEntity_p->type == ENTITY_TYPE_LEVEL_DOOR){
				IGUI_textInput(getVec2f(400, 20), &game_p->levelDoorNameTextInputData);

				String_set(editingEntity_p->levelName, game_p->levelDoorNameTextInputData.text, SMALL_STRING_SIZE);
			}
			if(editingEntity_p->type == ENTITY_TYPE_LEVEL_CABLE){

				if(IGUI_textButton_click("Rotate", getVec2f(400, 20), 100, false)){
					editingEntity_p->rotation.y += M_PI / 2;
					madeEdit = true;
				}

			}
			if(editingEntity_p->type == ENTITY_TYPE_RISER){

				const char *textureNames[] = {
					"pusher-up",
					"blank",
					"pusher-north",
					"pusher-south",
					"pusher-east",
					"pusher-west",
				};

				Vec2f pos = getVec2f(400, 20);

				for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++){

					//if(i == DIRECTION_DOWN){
						//continue;
					//}

					if(IGUI_textButton_click(DIRECTION_NAMES[i], pos, 100, editingEntity_p->pusherDirection == i)){
						editingEntity_p->pusherDirection = (enum Direction)i;
						String_set(editingEntity_p->textureName, textureNames[editingEntity_p->pusherDirection], SMALL_STRING_SIZE);
						madeEdit = true;
					}

					pos.y += 120;

				}
			
			}

			if(editingEntity_p->type == ENTITY_TYPE_PLAYER
			|| editingEntity_p->type == ENTITY_TYPE_ROCK
			|| editingEntity_p->type == ENTITY_TYPE_STICKY_ROCK){

				if(IGUI_textButton_click("Floating", getVec2f(400, 20), 100, editingEntity_p->floating)){

					editingEntity_p->floating = !editingEntity_p->floating;

					madeEdit = true;
				}
			
			}
			
		}

		IGUI_textInput(getVec2f(WIDTH - 900, 20), &game_p->levelNameTextInputData);

		if(openingLevel){

			openLevelScroll += Engine_pointer.scroll;

			//lookup level names
			char dirPath[STRING_SIZE];
			String_set(dirPath, "./levels/", STRING_SIZE);

			DIR *dataDir = opendir(dirPath);
			struct dirent* dirEntry;

			std::vector<SmallString> levelNames;

			while((dirEntry = readdir(dataDir)) != NULL){

				if(strcmp(dirEntry->d_name, ".") != 0
				&& strcmp(dirEntry->d_name, "..") != 0){

					char fileName[STRING_SIZE];
					String_set(fileName, dirEntry->d_name, STRING_SIZE);

					char path[STRING_SIZE];
					String_set(path, dirPath, STRING_SIZE);
					String_append(path, fileName);
					
					char levelNameTmp[STRING_SIZE];
					String_set(levelNameTmp, fileName, STRING_SIZE);
					memset(strrchr(levelNameTmp, *"."), *"\0", 1);

					SmallString levelName;
					String_set(levelName, levelNameTmp, SMALL_STRING_SIZE);

					levelNames.push_back(levelName);

				}
			
			}

			closedir(dataDir);

			//sort level names
			std::vector <char *>sortedLevelNames;
			char *lastLowestName = NULL;

			for(int i = 0; i < levelNames.size(); i++){

				char *currentLowestName = NULL;

				for(int j = 0; j < levelNames.size(); j++){

					if((lastLowestName == NULL || strcmp(lastLowestName, levelNames[j]) < 0)
					&& (currentLowestName == NULL || strcmp(currentLowestName, levelNames[j]) > 0)){
						currentLowestName = levelNames[j];
					}
					
				}

				lastLowestName = currentLowestName;
				sortedLevelNames.push_back(currentLowestName);

			}

			//create level name buttons
			Vec2f listPos = getVec2f(WIDTH - 850, 140);
			Vec2f scrollPos = getVec2f(0, openLevelScroll * 10 + 1);

			for(int i = 0; i < sortedLevelNames.size(); i++){

				if(scrollPos.y > 0 && scrollPos.y < 800){

					char *levelName = sortedLevelNames[i];

					if(IGUI_textButton_click(levelName, getAddVec2f(listPos, scrollPos), 80, false)){

						String_set(game_p->levelNameTextInputData.text, levelName, STRING_SIZE);

						Game_loadLevelByName(game_p, game_p->levelNameTextInputData.text);

						editingEntity = false;
						editingEntityID = -1;

						madeEdit = true;

					}
				
				}

				scrollPos.y += 100.0;

				if(i > (int)(sortedLevelNames.size() / 2)
				&& scrollPos.x < 1.0){
					scrollPos.x += 300.0;
					scrollPos.y = openLevelScroll * 10 + 1;
				}
			
			}

			if(Engine_pointer.upped){
				openingLevel = false;
			}

		}

		pos = getVec2f(WIDTH - 250, 20);

		if(IGUI_textButton_click("Open", pos, 100, false)){

			openingLevel = true;
			openLevelScroll = 0;

		}

		pos.y += 120;

		if(IGUI_textButton_click("Save", pos, 100, false)
		&& !(strcmp(game_p->levelNameTextInputData.text, "") == 0)){

			char path[STRING_SIZE];
			String_set(path, "levels/", STRING_SIZE);
			String_append(path, game_p->levelNameTextInputData.text);
			String_append(path, ".level");
			
			Game_writeCurrentLevelStateToFile(game_p, path);

		}

	}

	if(currentInterfaceMode == INTERFACE_MODE_EDITING){

		if(!Engine_fpsModeOn){
			Engine_pointer.movement = getVec2f(0.0, 0.0);
		}
		Engine_setFPSMode(true);

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

			if(currentEditingMode == EDITING_MODE_PLACE){
				
				Vec3f placePos = getAddVec3f(hitEntity_p->pos, hitNormal);

				if(currentEntityType == ENTITY_TYPE_OBSTACLE){
					Game_addObstacle(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_PLAYER){
					Game_addPlayer(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_ROCK){
					Game_addRock(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_STICKY_ROCK){
					Game_addStickyRock(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_GOAL){
					Game_addGoal(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_LEVEL_DOOR){
					Game_addLevelDoor(game_p, placePos, "");
				}
				if(currentEntityType == ENTITY_TYPE_LEVEL_CABLE){
					Game_addLevelCable(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_RISER){
					Game_addRiser(game_p, placePos);
				}
				if(currentEntityType == ENTITY_TYPE_CLONER){
					Game_addCloner(game_p, placePos);
				}

				madeEdit = true;
			}

			if(currentEditingMode == EDITING_MODE_REMOVE){

				Game_removeEntityByID(game_p, hitEntity_p->ID);

				game_p->hoveredEntityID = -1;

				madeEdit = true;

			}

			if(currentEditingMode == EDITING_MODE_EDIT){
				
				Entity *hoveredEntity_p = Game_getEntityByID(game_p, game_p->hoveredEntityID);

				if(hoveredEntity_p->type == ENTITY_TYPE_LEVEL_DOOR){
					editingEntity = true;
					editingEntityID = game_p->hoveredEntityID;

					IGUI_TextInputData_init(&game_p->levelDoorNameTextInputData, hoveredEntity_p->levelName, SMALL_STRING_SIZE);

					currentInterfaceMode = INTERFACE_MODE_MENU;
				}

				if(hoveredEntity_p->type == ENTITY_TYPE_LEVEL_CABLE
				|| hoveredEntity_p->type == ENTITY_TYPE_RISER
				|| hoveredEntity_p->type == ENTITY_TYPE_PLAYER
				|| hoveredEntity_p->type == ENTITY_TYPE_ROCK
				|| hoveredEntity_p->type == ENTITY_TYPE_STICKY_ROCK){
					editingEntity = true;
					editingEntityID = game_p->hoveredEntityID;

					currentInterfaceMode = INTERFACE_MODE_MENU;
				}

			}

		}

	}

	if(madeEdit){

		Game_writeCurrentLevelStateToFile(game_p, "levels/working.level");

	}

}
