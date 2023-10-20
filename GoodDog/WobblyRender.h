#pragma once

#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

struct StringAndColor
{
	Color color;
	const char* str;
};

static StringAndColor stringsAndColors[26] = {
	{ YELLOW,  "A" },
	{ ORANGE,  "B" },
	{ RED,     "C" },
	{ GREEN,   "D" },
	{ SKYBLUE, "E" },
	{ BLUE,    "F" },
	{ PURPLE,  "G" },
	{ WHITE,   "H" },
	{ MAGENTA, "I" },
	{ RED,     "J" },
	{ ORANGE,  "K" },
	{ YELLOW,  "L" },
	{ GREEN,   "M" },
	{ SKYBLUE, "N" },
	{ BLUE,    "O" },
	{ PURPLE,  "P" },
	{ WHITE,   "Q" },
	{ MAGENTA, "R" },
	{ RED,     "S" },
	{ ORANGE,  "T" },
	{ YELLOW,  "U" },
	{ GREEN,   "V" },
	{ SKYBLUE, "W" },
	{ BLUE,    "X" },
	{ PURPLE,  "Y" },
	{ WHITE,   "Z" },
};

struct WobblyTexture
{
	float wobbleTime = 0.f;
	int wobbleState = 0;

	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& texture, Vector2 pos, Vector2 scale, float angle, bool hFlipped = false, bool stableWobble = false, float alpha = 1.f, bool lightning = false);
};

struct WobblyLine
{
	float wobbleTime;
	int wobbleState;

	WobblyLine();
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Vector2 start, Vector2 end);
};

struct WobblyRectangle
{
	WobblyLine top, bottom, left, right;

	WobblyRectangle() {}
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, Vector2 topLeft, Vector2 botRight, bool lightning);
};

void DrawPaintLine(Texture2D& paintTex, Vector2 start, Vector2 end, bool lightning);
void DrawButtonText(Font& font, Vector2 position, int button, bool lightning);