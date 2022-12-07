#ifndef TEXT_H_
#define TEXT_H_

#include "stb/stb_truetype.h"

typedef struct Glyph{

	unsigned char *data;

	int width;
	int height;

	int west;
	int east;
	int north;
	int south;

	int leastSignificantBit;

}Glyph;

typedef struct Font{

	Glyph glyphs[255];

	int size;

	stbtt_fontinfo info;

	int ascent;
	int descent;
	int lineGap;

	float scale;

	char name[255];

}Font;

Font getFont(char *, int);

char *getImageDataFromFontAndString_mustFree(Font, char *, int *, int *);

#endif
