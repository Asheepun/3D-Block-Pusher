#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/strings.h"
#include "engine/files.h"

#include "game.h"

#include <vector>

#include "math.h"
#include "stdio.h"
#include "string.h"

void Game_writeCurrentLevelStateToFile(Game *game_p, const char *path){

	int dataSize = game_p->entities.size() * 5 * SMALL_STRING_SIZE;
	char *data = (char *)malloc(dataSize);
	memset(data, 0, dataSize);

	for(int i = 0; i < game_p->entities.size(); i++){

		Entity *entity_p = &game_p->entities[i];

		String_append(data, ":begin-entity\n");

		String_append(data, "-type\n");
		String_append_int(data, entity_p->type);
		String_append(data, "\n");

		String_append(data, "-pos\n");
		String_append_float(data, entity_p->pos.x);
		String_append(data, " ");
		String_append_float(data, entity_p->pos.y);
		String_append(data, " ");
		String_append_float(data, entity_p->pos.z);
		String_append(data, "\n");

		String_append(data, "-color\n");
		String_append_float(data, entity_p->color.x);
		String_append(data, " ");
		String_append_float(data, entity_p->color.y);
		String_append(data, " ");
		String_append_float(data, entity_p->color.z);
		String_append(data, " ");
		String_append_float(data, entity_p->color.w);
		String_append(data, "\n");

		String_append(data, ":end-entity\n");
	
	}

	writeDataToFile(path, data, dataSize);

}

void Game_loadLevelFile(Game *game_p, const char *path){

	printf("hello\n");
	printf("%s\n", path);

	int numberOfLines;
	FileLine *fileLines = getFileLines_mustFree(path, &numberOfLines);

	game_p->entities.clear();

	enum EntityType type;
	Vec3f pos;
	Vec4f color;

	char *ptr = NULL;

	for(int i = 0; i < numberOfLines; i++){

		if(strcmp(fileLines[i], "-type") == 0){
			type = (enum EntityType)strtol(fileLines[i + 1], &ptr, 10);
		}

		if(strcmp(fileLines[i], "-pos") == 0){
			ptr = fileLines[i + 1];

			for(int i = 0; i < 3; i++){
				pos[i] = strtof(ptr, &ptr);
				ptr += 1;
			}
		}

		if(strcmp(fileLines[i], "-color") == 0){
			ptr = fileLines[i + 1];

			for(int i = 0; i < 4; i++){
				color[i] = strtof(ptr, &ptr);
				ptr += 1;
			}
		}

		if(strcmp(fileLines[i], ":end-entity") == 0){

			Entity entity;

			Entity_init(&entity, pos, getVec3f(0.0, 0.0, 0.0), 0.5, "cube", "cube-borders", color, type);

			game_p->entities.push_back(entity);

		}

	}

	free(fileLines);

}

void Game_loadLevelByName(Game *game_p, const char *name){

	char path[STRING_SIZE];
	String_set(path, "levels/", STRING_SIZE);
	String_append(path, name);
	String_append(path, ".level");

	Game_loadLevelFile(game_p, path);

}
