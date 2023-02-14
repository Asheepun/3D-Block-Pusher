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

void Game_initMenuState(Game *game_p){

	Engine_setFPSMode(false);

}

void Game_menuState(Game *game_p){

	Vec2f pos = getVec2f(100, 100);

	if(IGUI_textButton_click("Exit Level", pos, 100, false)){

		game_p->currentGameState = GAME_STATE_LEVEL;
		game_p->mustInitGameState = true;

		Game_loadLevelByName(game_p, "levelhub");
		
		//set player pos
		for(int i = 0; i < game_p->entities.size(); i++){

			Entity *entity1_p = &game_p->entities[i];

			if(entity1_p->type == ENTITY_TYPE_PLAYER){
				entity1_p->pos = game_p->playerLevelHubPos;
				break;
			}

		}

		return;

	}
	pos.y += 140;

	if(IGUI_textButton_click("Return", pos, 100, false)
	|| Engine_keys[ENGINE_KEY_ESCAPE].downed){
		game_p->currentGameState = GAME_STATE_LEVEL;
		return;
	}
	pos.y += 140;

	if(IGUI_textButton_click("Quit", pos, 100, false)){
		Engine_quit();
		return;
	}
	pos.y += 140;

}
