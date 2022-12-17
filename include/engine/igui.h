#ifndef IGUI_H_
#define IGUI_H_

#include "engine/geometry.h"
#include "engine/renderer2d.h"
#include "stdbool.h"

typedef struct IGUI_SliderData{
	float value;
}IGUI_SliderData;

typedef struct IGUI_TextInputData{
	char text[STRING_SIZE];
	int cursorPosition;
	bool focused;
}IGUI_TextInputData;

extern bool IGUI_hoveringOverGUI;

void IGUI_init(int, int);

void IGUI_updatePointerScale();

void IGUI_render(Renderer2D_Renderer *);

bool IGUI_textButton_click(const char *, Vec2f, int, bool);

void IGUI_SliderData_init(IGUI_SliderData *, float);

void IGUI_slider(Vec2f, IGUI_SliderData *);

void IGUI_TextInputData_init(IGUI_TextInputData *, const char *, int);

void IGUI_textInput(Vec2f, IGUI_TextInputData *);

#endif
