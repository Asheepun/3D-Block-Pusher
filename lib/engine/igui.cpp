#include "engine/igui.h"

#include "engine/renderer2d.h"
#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/text.h"
#include "engine/strings.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include <vector>

typedef struct TextButton{
	char text[STRING_SIZE];
	int fontSize;
	Vec2f pos;
	Vec2f size;
	int paddingX;
	Renderer2D_Color textColor;
	Renderer2D_Color buttonColor;
}TextButton;

typedef struct Slider{
	Vec2f pos;
	Vec2f size;
	float value;
	Renderer2D_Color color;
}Slider;

typedef struct TextInput{
	Vec2f pos;
	Vec2f size;
	int fontSize;
	Renderer2D_Color color;
	char text[STRING_SIZE];
	int cursorPosition;
	bool focused;
}TextInput;

bool IGUI_hoveringOverGUI = false;

Renderer2D_Color textColor = { 0.0, 0.0, 0.0 };
Renderer2D_Color buttonColor = { 0.8, 0.8, 0.5 };

Renderer2D_Color buttonColorHover = { 0.7, 0.7, 0.6 };
Renderer2D_Color buttonColorSelected = { 0.6, 0.6, 0.5 };

std::vector<TextButton> textButtons;
std::vector<Slider> sliders;
std::vector<TextInput> textInputs;

Font font;

int IGUI_width;
int IGUI_height;

Engine_Pointer IGUI_pointer;

void IGUI_init(int width, int height){

	IGUI_width = width;
	IGUI_height = height;
	
	//Array_init(&textButtons, sizeof(TextButton));
	//Array_init(&sliders, sizeof(Slider));
	//Array_init(&textInputs, sizeof(TextInput));

	font = getFont("assets/fonts/times.ttf", 100);

}

void IGUI_updatePointerScale(){

	IGUI_pointer = Engine_pointer;

	IGUI_pointer.pos.x /= (float)Engine_clientWidth / (float)IGUI_width;
	IGUI_pointer.pos.y /= (float)Engine_clientHeight / (float)IGUI_height;
	IGUI_pointer.lastDownedPos.x /= (float)Engine_clientWidth / (float)IGUI_width;
	IGUI_pointer.lastDownedPos.y /= (float)Engine_clientHeight / (float)IGUI_height;
	IGUI_pointer.lastUppedPos.x /= (float)Engine_clientWidth / (float)IGUI_width;
	IGUI_pointer.lastUppedPos.y /= (float)Engine_clientHeight / (float)IGUI_height;

}

void IGUI_render(Renderer2D_Renderer *renderer_p){

	Renderer2D_Color color = { 0.8, 0.8, 0.5 };
	float alpha = 1.0;

	for(int i = 0; i < textButtons.size(); i++){

		TextButton *textButton_p = &textButtons[i];

		Renderer2D_drawColoredRectangle(renderer_p, textButton_p->pos.x - renderer_p->offset.x, textButton_p->pos.y - renderer_p->offset.y, textButton_p->size.x, textButton_p->size.y, textButton_p->buttonColor, alpha);

		Renderer2D_drawText(renderer_p, textButton_p->text, textButton_p->pos.x + textButton_p->paddingX - renderer_p->offset.x, textButton_p->pos.y - renderer_p->offset.y, textButton_p->fontSize, font, alpha);
	
	}

	for(int i = 0; i < sliders.size(); i++){

		Slider *slider_p = &sliders[i];

		int knobWidth = 10;
		int knobHeight = 20;
		int knobY = slider_p->pos.y - 5 - renderer_p->offset.y;

		int knobX = slider_p->pos.x + (float)(slider_p->size.x - knobWidth) * slider_p->value - renderer_p->offset.x;

		Renderer2D_drawColoredRectangle(renderer_p, slider_p->pos.x - renderer_p->offset.x, slider_p->pos.y - renderer_p->offset.y, slider_p->size.x, slider_p->size.y, slider_p->color, alpha);

		Renderer2D_drawColoredRectangle(renderer_p, knobX, knobY, knobWidth, knobHeight, slider_p->color, alpha);
	
	}

	//render text inputs
	for(int i = 0; i < textInputs.size(); i++){
		
		TextInput *textInput_p = &textInputs[i];

		Renderer2D_drawColoredRectangle(renderer_p, textInput_p->pos.x - renderer_p->offset.x, textInput_p->pos.y - renderer_p->offset.y, textInput_p->size.x, textInput_p->size.y, textInput_p->color, alpha);

		Renderer2D_drawText(renderer_p, textInput_p->text, textInput_p->pos.x - renderer_p->offset.x, textInput_p->pos.y - renderer_p->offset.y, textInput_p->size.y, font, alpha);

	}

	textButtons.clear();
	sliders.clear();
	textInputs.clear();

	IGUI_hoveringOverGUI = false;

}

bool checkPointInRect(Vec2f point, Vec2f rectPos, Vec2f rectSize){

	if(point.x > rectPos.x && point.x < rectPos.x + rectSize.x
	&& point.y > rectPos.y && point.y < rectPos.y + rectSize.y){
		return true;
	}

	return false;

}

bool IGUI_textButton_click(const char *text, Vec2f pos, int fontSize, bool selected){

	TextButton textButton;

	textButton.fontSize = fontSize;
	textButton.pos = pos;

	String_set(textButton.text, text, STRING_SIZE);

	int width, height;

	char *data = getImageDataFromFontAndString_mustFree(font, text, &width, &height);
	free(data);

	width *= (float)fontSize / (float)height;
	height = fontSize;

	textButton.size = getVec2f(width, height);

	textButton.paddingX = 5;
	textButton.size.x += textButton.paddingX * 2;

	textButton.buttonColor = buttonColor;

	bool hover = false;
	bool clicked = false;
	bool hasBeenDowned = false;

	//check status
	if(checkPointInRect(IGUI_pointer.pos, textButton.pos, textButton.size)){
		hover = true;
		IGUI_hoveringOverGUI = true;
	}
	if(checkPointInRect(IGUI_pointer.lastDownedPos, textButton.pos, textButton.size)){
		hasBeenDowned = true;
		IGUI_hoveringOverGUI = true;
	}

	if(selected){
		textButton.buttonColor = buttonColorSelected;
	}

	if(hover){
		textButton.buttonColor = buttonColorHover;

		if(IGUI_pointer.upped
		&& hasBeenDowned){
			clicked = true;
		}
	
	}

	textButtons.push_back(textButton);

	return clicked;

}

void IGUI_SliderData_init(IGUI_SliderData *sliderData_p, float value){
	sliderData_p->value = value;
}

void IGUI_slider(Vec2f pos, IGUI_SliderData *sliderData_p){

	Slider slider;
	//Slider *slider_p = Array_addItem(&sliders);

	slider.pos = pos;
	
	slider.size = getVec2f(150, 10);

	slider.color = buttonColor;

	//check status

	bool hover = false;
	bool clicked = false;
	bool hasBeenDowned = false;
	bool hoverX = false;

	//check status
	if(checkPointInRect(IGUI_pointer.pos, slider.pos, slider.size)){
		hover = true;
		IGUI_hoveringOverGUI = true;
	}
	if(checkPointInRect(IGUI_pointer.lastDownedPos, slider.pos, slider.size)){
		hasBeenDowned = true;
		IGUI_hoveringOverGUI = true;
	}
	if(IGUI_pointer.pos.x > slider.pos.x && IGUI_pointer.pos.x < slider.pos.x + slider.size.x){
		hoverX = true;
	}

	if(hover){
		slider.color = buttonColorHover;
	}

	if(hasBeenDowned
	&& IGUI_pointer.down){

		if(hoverX){
			sliderData_p->value = (IGUI_pointer.pos.x - slider.pos.x) / slider.size.x;
		}

		if(IGUI_pointer.pos.x > slider.pos.x + slider.size.x){
			sliderData_p->value = 1.0;
		}
		if(IGUI_pointer.pos.x < slider.pos.x){
			sliderData_p->value = 0.0;
		}

		slider.color = buttonColorHover;

	}

	//set value
	slider.value = sliderData_p->value;

}

void IGUI_TextInputData_init(IGUI_TextInputData *textInputData_p, const char *startText, int startTextLength){

	String_set(textInputData_p->text, "", STRING_SIZE);

	textInputData_p->cursorPosition = 0;
	textInputData_p->focused = false;

	if(startText != NULL){
		String_set(textInputData_p->text, startText, startTextLength);
		textInputData_p->cursorPosition = startTextLength - 1;
	}

}

void IGUI_textInput(Vec2f pos, IGUI_TextInputData *textInputData_p){

	//TextInput *textInput_p = Array_addItem(&textInputs);
	TextInput textInput;

	textInput.pos = pos;
	textInput.size = getVec2f(600, 100);

	//textInput.color = Renderer2D_getColor(1.0, 1.0, 1.0);
	textInput.color = buttonColor;

	if(IGUI_pointer.downed){

		if(checkPointInRect(IGUI_pointer.pos, textInput.pos, textInput.size)){
			textInputData_p->focused = true;
		}else{
			textInputData_p->focused = false;
		}

	}

	if(checkPointInRect(IGUI_pointer.pos, textInput.pos, textInput.size)){
		textInput.color = buttonColorHover;
	}

	textInput.focused = textInputData_p->focused;
	textInput.cursorPosition = textInputData_p->cursorPosition;

	if(textInputData_p->focused){

		for(int i = 0; i < Engine_textInput.size(); i++){

			char text[STRING_SIZE];
			String_set(text, "", STRING_SIZE);
			text[0] = Engine_textInput[i];

			if(text[0] == 8){//handle backspace
				if(strlen(textInputData_p->text) > 0){
					textInputData_p->text[strlen(textInputData_p->text) - 1] = *"\0";
				}
			}else{
				String_append(textInputData_p->text, text);
			}
		
		}

		textInput.color = buttonColorSelected;

	}

	String_set(textInput.text, textInputData_p->text, STRING_SIZE);

	//String_set(textInput.text, "hello", STRING_SIZE);

	textInput.cursorPosition = 0;

	textInputs.push_back(textInput);

}
