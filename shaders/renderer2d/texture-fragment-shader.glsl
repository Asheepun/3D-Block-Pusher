#version 330 core

precision mediump float;

in vec2 textureCoord;

uniform sampler2D tex;

uniform float alpha;

void main(){

	gl_FragColor = texture2D(tex, textureCoord) * vec4(1.0, 1.0, 1.0, alpha);

	gl_FragDepth = 0.0;

}
