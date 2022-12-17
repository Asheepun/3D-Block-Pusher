#version 100

attribute vec2 vertexPosition_attribute;

uniform float posX;
uniform float posY;

uniform float width;
uniform float height;

varying vec2 vertexPosition;

void main(){

	vertexPosition = vertexPosition_attribute;

	gl_Position = vec4(
		posX + width + -1.0 + vertexPosition.x * width,
		-posY + -height + 1.0 + vertexPosition.y * height,
		0.0,
		1.0
	);

}
